compile with:  hipcc --offload-arch=gfx90a -O3 -o divergence divergence.cpp
run with: srun -p dev-g -A project_462001452  -n 1 -c 7 --gpus 1 rocprofv3 --output-format csv --kernel-trace -S true -- ./divergence
