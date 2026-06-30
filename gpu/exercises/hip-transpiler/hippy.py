#!/usr/bin/env python3
"""
hippy — a tiny scaffold generator that turns a Python-ish .hippy file
into a compilable HIP C++ (.hip) file.

This is NOT a full language. It's a boilerplate generator:
  - kernel defs   -> __global__ void with index/bounds-check helpers expanded
  - buffer decl   -> GPUBuffer<T> declarations (RAII wrapper, included inline)
  - upload/down   -> .upload()/.download() calls
  - launch        -> <<<blocks, threads>>> launch with block math computed

Anything inside a kernel body is copied through with light line-by-line
translation (python `if`/indentation -> braces). If a line can't be
confidently translated, it's emitted as a TODO comment for manual editing.

Usage:
    python3 hippy.py input.hippy -o output.hip
"""

import sys
import re
import argparse
from lark import Lark, Token


GRAMMAR_PATH = "grammar.lark"


class BodyIndenter:
    """
    Custom postlex pass (not lark.indenter.Indenter, since our BODY_LINE
    tokens already include their own trailing newline rather than being
    split into separate content/NEWLINE tokens).

    Inserts _INDENT before the first BODY_LINE following a KERNEL_HEADER,
    and _DEDENT once indentation drops back to non-BODY_LINE territory.
    """

    always_accept = ()

    def process(self, stream):
        in_body = False
        for tok in stream:
            if tok.type == "KERNEL_HEADER":
                if in_body:
                    yield Token("_DEDENT", "")
                    in_body = False
                yield tok
            elif tok.type == "BODY_LINE":
                if not in_body:
                    yield Token("_INDENT", "")
                    in_body = True
                yield tok
            else:
                if in_body:
                    yield Token("_DEDENT", "")
                    in_body = False
                yield tok
        if in_body:
            yield Token("_DEDENT", "")


def build_parser():
    with open(GRAMMAR_PATH) as f:
        grammar = f.read()
    return Lark(
        grammar,
        parser="lalr",
        lexer="basic",
        postlex=BodyIndenter(),
        propagate_positions=True,
    )


# ----------------------------------------------------------------------
# Kernel body line translation (best-effort, not a full Python parser)
# ----------------------------------------------------------------------

def translate_kernel_line(line: str, indent_level: int):
    """
    Best-effort translation of one line of pseudo-python kernel body
    into C++. Returns (cpp_line, opens_brace: bool).
    Falls back to a TODO comment if it doesn't recognize the pattern.
    """
    stripped = line.strip()
    pad = "    " * indent_level

    if stripped == "":
        return "", False

    # global_index() -> standard HIP thread index expansion
    if "global_index()" in stripped:
        stripped = stripped.replace(
            "global_index()",
            "(blockIdx.x * blockDim.x + threadIdx.x)"
        )

    # if <cond>:  ->  if (<cond>) {
    if stripped.startswith("if ") and stripped.endswith(":"):
        cond = stripped[3:-1].strip()
        return f"{pad}if ({cond}) {{", True

    # elif <cond>: -> } else if (<cond>) {
    if stripped.startswith("elif ") and stripped.endswith(":"):
        cond = stripped[5:-1].strip()
        return f"{pad}}} else if ({cond}) {{", True

    # else: -> } else {
    if stripped == "else:":
        return f"{pad}}} else {{", True

    # for i in range(n): -> for (int i = 0; i < n; i++) {
    if stripped.startswith("for ") and stripped.endswith(":"):
        try:
            # for VAR in range(N):
            rest = stripped[4:-1].strip()
            var, rng = rest.split(" in ", 1)
            var = var.strip()
            if rng.strip().startswith("range(") and rng.strip().endswith(")"):
                bound = rng.strip()[6:-1]
                return f"{pad}for (int {var} = 0; {var} < {bound}; {var}++) {{", True
        except ValueError:
            pass
        return f"{pad}// TODO: translate this manually\n{pad}// {stripped}", False

    # plain assignment / expression statement -> add semicolon
    if stripped.endswith(":"):
        # unrecognized block opener
        return f"{pad}// TODO: translate this manually\n{pad}// {stripped}", False

    if not stripped.endswith(";"):
        stripped += ";"

    # naive auto-declare: `i = <expr>;` (no prior `int`/`float` keyword,
    # no array index on the left) -> assume int, since this is almost
    # always a thread-index variable. TODO comment flags it for review.
    bare_assign = re.match(r"^([A-Za-z_]\w*)\s*=\s*[^=]", stripped)
    if bare_assign and "[" not in stripped.split("=")[0]:
        var = bare_assign.group(1)
        stripped = f"int {stripped}  // TODO: hippy guessed 'int' for {var}; fix the type if wrong"

    return f"{pad}{stripped}", False


def emit_kernel_body(lines, base_indent=1):
    """
    Convert kernel body lines (already separated by source indentation
    via the grammar's NEWLINE-delimited kernel_line tokens) into C++.
    We track indentation by counting leading whitespace in the raw line,
    relative to the first line's indentation, and open/close braces for
    if/for blocks heuristically based on trailing ':' and a one-shot
    "closes after this line's children" model (Python-block style).
    """
    out = []
    # Determine base indentation (in spaces) of the first non-blank line
    raw_lines = [l for l in lines if l.strip() != ""]
    if not raw_lines:
        return out

    def leading_spaces(s):
        return len(s) - len(s.lstrip(" "))

    base = leading_spaces(raw_lines[0])
    # Stack of (source_indent, emitted) for brace closing
    block_stack = []  # holds source indent levels of currently open braces

    for raw in lines:
        if raw.strip() == "":
            continue
        cur_indent = leading_spaces(raw)
        depth = max(0, (cur_indent - base) // 4) + base_indent
        stripped = raw.strip()
        is_chain_continuation = stripped.startswith("elif ") or stripped == "else:"

        # Close braces for any block we've dedented out of.
        # For elif/else, the matching `if` block at this same indent is
        # closed by the "} else {" / "} else if (...) {" line itself
        # (see translate_kernel_line), so we pop it here WITHOUT emitting
        # an extra brace, but still emit braces for any deeper blocks.
        while block_stack and cur_indent <= block_stack[-1][0]:
            close_depth = block_stack[-1][1]
            same_level = cur_indent == block_stack[-1][0]
            block_stack.pop()
            if is_chain_continuation and same_level:
                break  # let translate_kernel_line's "} else {" supply this brace
            out.append("    " * close_depth + "}")

        cpp_line, opens = translate_kernel_line(raw, depth)
        out.append(cpp_line)
        if opens:
            block_stack.append((cur_indent, depth))

    # Close any remaining open blocks
    while block_stack:
        close_depth = block_stack[-1][1]
        out.append("    " * close_depth + "}")
        block_stack.pop()

    return out


# ----------------------------------------------------------------------
# Code generation from parsed tree
# ----------------------------------------------------------------------

GPU_BUFFER_TEMPLATE = """\
template<typename T>
struct GPUBuffer {
    T* ptr = nullptr;
    size_t n = 0;

    GPUBuffer() = default;
    explicit GPUBuffer(size_t n_) : n(n_) {
        hipMalloc(&ptr, n * sizeof(T));
    }
    ~GPUBuffer() {
        if (ptr) hipFree(ptr);
    }

    // movable, not copyable (owns a device allocation)
    GPUBuffer(const GPUBuffer&) = delete;
    GPUBuffer& operator=(const GPUBuffer&) = delete;
    GPUBuffer(GPUBuffer&& other) noexcept : ptr(other.ptr), n(other.n) {
        other.ptr = nullptr;
    }

    void upload(const T* host) {
        hipMemcpy(ptr, host, n * sizeof(T), hipMemcpyHostToDevice);
    }
    void download(T* host) {
        hipMemcpy(host, ptr, n * sizeof(T), hipMemcpyDeviceToHost);
    }

    operator T*() { return ptr; }
};

inline int hippy_blocks(int n, int threads = 256) {
    return (n + threads - 1) / threads;
}
"""

TYPE_MAP = {
    "float*": "float*", "int*": "int*", "double*": "double*",
    "float": "float", "int": "int", "double": "double",
}

KERNEL_HEADER_RE = re.compile(
    r"kernel\s+([A-Za-z_]\w*)\s*\((.*?)\)\s*:\s*$"
)
BUFFER_LINE_RE = re.compile(r"buffer\s+([A-Za-z_]\w*\*?)\s+(.*)$")
BUFFER_ITEM_RE = re.compile(r"([A-Za-z_]\w*)\s*\(\s*([A-Za-z_]\w*)\s*\)")
LAUNCH_LINE_RE = re.compile(
    r"launch\s+([A-Za-z_]\w*)\s*\(([^()]*)\)\s*\((.*)\)\s*$"
)
INIT_LINE_RE = re.compile(r"init\s+(.+)$")


def parse_params(param_str):
    """ 'a: float*, b: float*, n: int' -> [('a','float*'), ...] """
    params = []
    if not param_str.strip():
        return params
    for part in param_str.split(","):
        part = part.strip()
        if not part:
            continue
        pname, ptype = part.split(":")
        params.append((pname.strip(), ptype.strip()))
    return params


def gen_kernel(kernel_def_node):
    header_line = str(kernel_def_node.children[0]).strip()
    m = KERNEL_HEADER_RE.match(header_line)
    if not m:
        raise ValueError(f"Could not parse kernel header: {header_line!r}")
    name, param_str = m.group(1), m.group(2)
    params = parse_params(param_str)
    params_str = ", ".join(f"{t} {n}" for n, t in params)

    body_node = kernel_def_node.children[1]  # kernel_body
    body_lines = [str(tok).rstrip("\n") for tok in body_node.children]

    cpp_body = emit_kernel_body(body_lines, base_indent=1)

    out = [f"__global__ void {name}({params_str}) {{"]
    out.extend(cpp_body)
    out.append("}")
    return "\n".join(out), name, [n for n, _ in params]


def gen_const(line):
    """'const int N = 1024' -> 'constexpr int N = 1024;' at global scope."""
    body = line.strip()[len("const"):].strip()
    return f"constexpr {body};"


def parse_buffer_info(line):
    """Return {name: (elem_type, size_or_None)} for every item on a buffer line."""
    line = line.strip()
    m = BUFFER_LINE_RE.match(line)
    if not m:
        return {}
    type_tok, rest = m.group(1), m.group(2)
    cpp_type = TYPE_MAP.get(type_tok, type_tok)
    elem_type = cpp_type.replace("*", "")

    info = {}
    for item in re.split(r",\s*", rest.strip()):
        item = item.strip()
        if not item:
            continue
        am = re.match(r"^([A-Za-z_]\w*)\s*\(\s*([A-Za-z_]\w*)\s*\)$", item)
        sm = re.match(r"^([A-Za-z_]\w*)$", item)
        if am:
            info[am.group(1)] = (elem_type, am.group(2))
        elif sm:
            info[sm.group(1)] = (elem_type, None)
    return info


def _add_type_suffix(val, elem_type):
    """Append C++ literal suffix so float 1.0 doesn't silently become double."""
    if elem_type == "float" and re.match(r"^[0-9]*\.?[0-9]+([eE][+-]?[0-9]+)?$", val):
        return val + "f"
    return val


def gen_init(line, buffer_info):
    """Generate host-side declarations from 'init name=val, name=val, ...'"""
    line = line.strip()
    m = INIT_LINE_RE.match(line)
    if not m:
        return f"// TODO: could not parse init line: {line}"

    out = []
    for assign in m.group(1).split(","):
        assign = assign.strip()
        if "=" not in assign:
            out.append(f"// TODO: could not parse init assignment: {assign}")
            continue
        name, val = (s.strip() for s in assign.split("=", 1))

        if name not in buffer_info:
            out.append(f"// TODO: init {name} = {val}  — not found in buffer; declare manually")
            continue

        elem_type, size = buffer_info[name]
        val_typed = _add_type_suffix(val, elem_type)

        if size is None:
            # scalar — simple variable declaration
            out.append(f"{elem_type} {name} = {val_typed};")
        else:
            # array — host allocation + fill loop
            out.append(f"{elem_type} h_{name}[{size}];")
            out.append(f"for (int i = 0; i < {size}; i++) h_{name}[i] = {val_typed};")

    return "\n".join(out)


def gen_buffer_decl(line):
    line = line.strip()
    m = BUFFER_LINE_RE.match(line)
    if not m:
        return f"// TODO: could not parse buffer line: {line}"
    type_tok, rest = m.group(1), m.group(2)
    cpp_type = TYPE_MAP.get(type_tok, type_tok)
    elem_type = cpp_type.replace("*", "")

    decls = []
    for item_m in BUFFER_ITEM_RE.finditer(rest):
        bname, bsize = item_m.group(1), item_m.group(2)
        decls.append(f"GPUBuffer<{elem_type}> {bname}({bsize});")
    return "\n".join(decls)


def gen_upload(line):
    line = line.strip()
    names = [n.strip() for n in line[len("upload"):].strip().split(",") if n.strip()]
    return "\n".join(
        f"{n}.upload(h_{n});  // TODO: ensure h_{n} is your host-side array" for n in names
    )


def gen_download(line):
    line = line.strip()
    names = [n.strip() for n in line[len("download"):].strip().split(",") if n.strip()]
    return "\n".join(
        f"{n}.download(h_{n});  // TODO: ensure h_{n} is your host-side array" for n in names
    )


def gen_launch(line):
    line = line.strip()
    m = LAUNCH_LINE_RE.match(line)
    if not m:
        return f"// TODO: could not parse launch line: {line}"
    kname, size_args, arglist = m.group(1), m.group(2), m.group(3)

    size_parts = [p.strip() for p in size_args.split(",")]
    size_expr = size_parts[0]
    threads = "256"
    for p in size_parts[1:]:
        if p.startswith("threads"):
            threads = p.split("=")[1].strip()

    args_str = arglist.strip()

    return (
        f"{kname}<<<hippy_blocks({size_expr}, {threads}), {threads}>>>({args_str});\n"
        f"hipDeviceSynchronize();  // TODO: remove if you're handling sync/streams yourself"
    )


def generate(tree, source_name):
    consts_cpp = []
    kernels_cpp = []
    main_body = []

    # First pass: collect buffer type/size info needed by gen_init
    buffer_info = {}
    for item in tree.children:
        node = item.children[0]
        if node.data == "buffer_decl":
            buffer_info.update(parse_buffer_info(str(node.children[0])))

    has_init = any(item.children[0].data == "init_stmt" for item in tree.children)

    # Second pass: generate code in source order
    for item in tree.children:
        node = item.children[0]
        if node.data == "kernel_def":
            cpp, kname, params = gen_kernel(node)
            kernels_cpp.append(cpp)
        elif node.data == "const_decl":
            consts_cpp.append(gen_const(str(node.children[0])))
        elif node.data == "buffer_decl":
            main_body.append(gen_buffer_decl(str(node.children[0])))
        elif node.data == "init_stmt":
            main_body.append(gen_init(str(node.children[0]), buffer_info))
        elif node.data == "upload_stmt":
            main_body.append(gen_upload(str(node.children[0])))
        elif node.data == "download_stmt":
            main_body.append(gen_download(str(node.children[0])))
        elif node.data == "launch_stmt":
            main_body.append(gen_launch(str(node.children[0])))

    out = []
    out.append(f"// Auto-generated by hippy from {source_name}")
    out.append("// Generated scaffold — edit freely from here on.")
    out.append("#include <hip/hip_runtime.h>")
    out.append("#include <cstdio>")
    out.append("#include <cstdlib>")
    out.append("")
    out.append(GPU_BUFFER_TEMPLATE)
    if consts_cpp:
        out.append("// ---- constants ----")
        out.extend(consts_cpp)
        out.append("")
    out.append("// ---- kernels ----")
    out.append("")
    out.extend(kernels_cpp)
    out.append("")
    out.append("// ---- host code ----")
    if not has_init:
        out.append("// TODO: declare and fill your host-side arrays (h_a, h_b, ...) before this point")
    out.append("int main() {")
    for block in main_body:
        for line in block.split("\n"):
            out.append("    " + line)
        out.append("")
    out.append("    return 0;")
    out.append("}")
    return "\n".join(out)


def main():
    ap = argparse.ArgumentParser(description="hippy: .hippy -> .hip scaffold generator")
    ap.add_argument("input", help="input .hippy file")
    ap.add_argument("-o", "--output", help="output .hip file", default=None)
    args = ap.parse_args()

    with open(args.input) as f:
        source = f.read()
    if not source.endswith("\n"):
        source += "\n"

    parser = build_parser()
    try:
        tree = parser.parse(source)
    except Exception as e:
        print(f"hippy: parse error in {args.input}:\n{e}", file=sys.stderr)
        sys.exit(1)

    cpp_code = generate(tree, args.input)

    out_path = args.output or (args.input.rsplit(".", 1)[0] + ".hip")
    with open(out_path, "w") as f:
        f.write(cpp_code)

    print(f"hippy: wrote {out_path}")


if __name__ == "__main__":
    main()
