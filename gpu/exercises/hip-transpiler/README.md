# hippy

A tiny scaffold generator that turns a Python-ish `.hippy` file into a
compilable HIP C++ (`.hip`) file. It is **not** a full language — it
generates the boilerplate (memory allocation, host<->device copies,
kernel launch math) and leaves the rest of the file as normal, editable
C++. Open the generated `.hip` file and keep going by hand.

## Usage

```bash
python3 hippy.py examples/add_vectors.hippy -o add_vectors.hip
hipcc add_vectors.hip -o add_vectors      # on LUMI: module load rocm first
```

## Supported syntax

```python
kernel add_vectors(a: float*, b: float*, c: float*, n: int):
    i = global_index()
    if i < n:
        c[i] = a[i] + b[i]

buffer float a(N), b(N), c(N)
upload a, b
launch add_vectors(N)(a, b, c, n)
download c
```

- `kernel name(param: type, ...):` -> `__global__ void`
- `global_index()` -> `blockIdx.x * blockDim.x + threadIdx.x`
- `if` / `elif` / `else` / `for x in range(n)` -> braces
- `buffer TYPE name(SIZE), ...` -> `GPUBuffer<T>` (RAII wrapper, generated inline)
- `upload a, b` / `download c` -> `.upload()` / `.download()` calls
- `launch kernel(N, threads=256)(args...)` -> `<<<blocks, threads>>>` with block math

Anything inside a kernel body that isn't recognized is emitted as a
`// TODO: translate this manually` comment rather than guessed at.

## Known limitations (this is a v1 prototype)

- Kernel body translation is line-based and heuristic, not a real parser.
  Complex expressions, nested loops, or anything beyond simple
  if/elif/else/for will likely need manual fixing.
- Only one block of unindented blank lines is tolerated; weird mixed
  indentation will probably confuse it.
- No type checking at all — `int`/`float*` mismatches are your problem.
- Bare assignment lines (`i = ...`) are guessed to be `int`; check the
  TODO comment hippy leaves and fix if wrong.
