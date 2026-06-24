import re
import pandas as pd

def parse_consolidated(filepath="consolidated.out"):
    records = []
    current_file = None
    block = {}

    with open(filepath) as f:
        for line in f:
            line = line.strip()

            # New file header
            m = re.match(r"={5,}\s+(\S+)\s+={5,}", line)
            if m:
                if block:
                    records.append(block)
                current_file = m.group(1)
                block = {"filename": current_file}
                continue

            if current_file is None:
                continue

            # Simulation parameters
            m = re.match(
                r"Simulation parameters: height: (\d+) width: (\d+) length: (\d+) time steps: (\d+)",
                line,
            )
            if m:
                block["height"]     = int(m.group(1))
                block["width"]      = int(m.group(2))
                block["length"]     = int(m.group(3))
                block["time_steps"] = int(m.group(4))
                continue

            # MPI tasks + decomposition
            m = re.match(
                r"Number of MPI tasks: (\d+) \((\d+) x (\d+) x (\d+)\)", line
            )
            if m:
                block["mpi_tasks"]  = int(m.group(1))
                block["mpi_x"]      = int(m.group(2))
                block["mpi_y"]      = int(m.group(3))
                block["mpi_z"]      = int(m.group(4))
                continue

            # OpenMP threads
            m = re.match(r"Number of OpenMP threads: (\d+)", line)
            if m:
                block["omp_threads"] = int(m.group(1))
                continue

            # Average temperature at start
            m = re.match(r"Average temperature at start: ([\d.]+)", line)
            if m:
                block["avg_temp_start"] = float(m.group(1))
                continue

            # Iteration time
            m = re.match(r"Iteration took ([\d.]+) seconds\.", line)
            if m:
                block["iteration_time_s"] = float(m.group(1))
                continue

            # MPI time
            m = re.match(r"MPI\s+([\d.]+) s\.", line)
            if m:
                block["mpi_time_s"] = float(m.group(1))
                continue

            # Compute time
            m = re.match(r"Compute\s+([\d.]+) s\.", line)
            if m:
                block["compute_time_s"] = float(m.group(1))
                continue

            # Average temperature at end
            m = re.match(r"Average temperature: ([\d.]+)", line)
            if m:
                block["avg_temp_end"] = float(m.group(1))
                continue

    # Don't forget the last block
    if block and "filename" in block:
        records.append(block)

    df = pd.DataFrame(records)

    # Derive node count from filename  (slurm-n2-..., slurm-n16-..., etc.)
    df["nodes"] = (
        df["filename"]
        .str.extract(r"slurm-n(\d+)-")
        .astype(float)          # NaN for the baseline 'scalability' run
    )

    # Useful derived columns
    df["mpi_fraction"]     = df["mpi_time_s"]     / df["iteration_time_s"]
    df["compute_fraction"] = df["compute_time_s"]  / df["iteration_time_s"]

    # Speedup relative to the slowest run
    baseline = df["iteration_time_s"].max()
    df["speedup"] = baseline / df["iteration_time_s"]

    # Sort by MPI task count
    df = df.sort_values("mpi_tasks").reset_index(drop=True)

    return df


if __name__ == "__main__":
    df = parse_consolidated("consolidated.out")
    df.to_csv('consolidated.tsv', sep='\t')
    pd.set_option("display.max_columns", None)
    pd.set_option("display.width", 120)
    print(df.to_string(index=False))
    print("\nColumns:", df.columns.tolist())
    print("\nDtypes:\n", df.dtypes)