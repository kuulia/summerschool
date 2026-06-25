//Author Linus Lind: MIT License

// Solution: Parallel Cholesky Decomposition using MPI
// See the exercise skeleton for algorithm description.

#include <mpi.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

const int N = 8;

void generate_spd(std::vector<double>& A, int n)
{
    srand(42);
    A.assign(n * n, 0.0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            double v = (rand() / (double)RAND_MAX) * 0.5;
            A[i*n+j] = A[j*n+i] = v;
        }
        double rowsum = 0.0;
        for (int j = 0; j < n; j++)
            if (j != i) rowsum += std::abs(A[i*n+j]);
        A[i*n+i] = rowsum + 1.0;
    }
}

void cholesky_serial(std::vector<double>& A, int n)
{
    for (int k = 0; k < n; k++) {
        A[k*n+k] = std::sqrt(A[k*n+k]);
        for (int i = k+1; i < n; i++)
            A[i*n+k] /= A[k*n+k];
        for (int j = k+1; j < n; j++)
            for (int i = j; i < n; i++)
                A[i*n+j] -= A[i*n+k] * A[j*n+k];
    }
}

void print_lower(const char* label, const std::vector<double>& A, int n)
{
    printf("%s:\n", label);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++)
            printf("%8.4f ", A[i*n+j]);
        printf("\n");
    }
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int ncols_local = 0;
    for (int j = rank; j < N; j += nprocs)
        ncols_local++;

    std::vector<double> local_A(N * ncols_local, 0.0);
    std::vector<double> A_full, A_ref;

    if (rank == 0) {
        generate_spd(A_full, N);
        A_ref = A_full;
        cholesky_serial(A_ref, N);
    }

    // --- TODO 1 SOLUTION: Distribute columns ---

    if (rank == 0) {
        // Pack and send each column to its owner
        std::vector<double> col_buf(N);
        for (int j = 0; j < N; j++) {
            int owner   = j % nprocs;
            int l       = j / nprocs; // local index on owner
            // Pack column j from row-major A_full
            for (int i = 0; i < N; i++)
                col_buf[i] = A_full[i*N + j];
            if (owner == 0) {
                // Copy directly into local storage
                for (int i = 0; i < N; i++)
                    local_A[l*N + i] = col_buf[i];
            } else {
                MPI_Send(col_buf.data(), N, MPI_DOUBLE, owner, j, MPI_COMM_WORLD);
            }
        }
    } else {
        // Receive each column that belongs to this rank
        MPI_Status status;
        for (int l = 0; l < ncols_local; l++) {
            int j = l * nprocs + rank; // global column index
            MPI_Recv(&local_A[l * N], N, MPI_DOUBLE, 0, j, MPI_COMM_WORLD, &status);
        }
    }

    // --- Parallel Cholesky ---

    std::vector<double> pivot_col(N, 0.0);

    for (int k = 0; k < N; k++) {
        int owner   = k % nprocs;
        int k_local = k / nprocs;

        // TODO 2 SOLUTION: Owner computes pivot column
        if (rank == owner) {
            local_A[k_local*N + k] = std::sqrt(local_A[k_local*N + k]);
            for (int i = k+1; i < N; i++)
                local_A[k_local*N + i] /= local_A[k_local*N + k];
            for (int i = k; i < N; i++)
                pivot_col[i] = local_A[k_local*N + i];
        }

        // TODO 3 SOLUTION: Broadcast pivot column
        MPI_Bcast(&pivot_col[k], N - k, MPI_DOUBLE, owner, MPI_COMM_WORLD);

        // TODO 4 SOLUTION: Update trailing local columns
        for (int l = 0; l < ncols_local; l++) {
            int j = l * nprocs + rank; // global column index
            if (j > k) {
                for (int i = j; i < N; i++)
                    local_A[l*N + i] -= pivot_col[i] * pivot_col[j];
            }
        }
    }

    // --- TODO 5 SOLUTION: Gather results to rank 0 ---

    if (rank == 0) {
        // Unpack own local columns
        for (int l = 0; l < ncols_local; l++) {
            int j = l * nprocs; // rank 0, so j = l*nprocs + 0
            for (int i = 0; i < N; i++)
                A_full[i*N + j] = local_A[l*N + i];
        }
        // Receive from other ranks
        std::vector<double> col_buf(N);
        MPI_Status status;
        for (int j = 0; j < N; j++) {
            int owner = j % nprocs;
            int l     = j / nprocs;
            if (owner != 0) {
                MPI_Recv(col_buf.data(), N, MPI_DOUBLE, owner, j, MPI_COMM_WORLD, &status);
                for (int i = 0; i < N; i++)
                    A_full[i*N + j] = col_buf[i];
            }
        }
    } else {
        for (int l = 0; l < ncols_local; l++) {
            int j = l * nprocs + rank;
            MPI_Send(&local_A[l * N], N, MPI_DOUBLE, 0, j, MPI_COMM_WORLD);
        }
    }

    // --- Verify ---

    if (rank == 0) {
        bool ok = true;
        for (int i = 0; i < N && ok; i++)
            for (int j = 0; j <= i && ok; j++)
                if (std::abs(A_full[i*N+j] - A_ref[i*N+j]) > 1e-10) {
                    printf("MISMATCH at (%d,%d): got %.10f, expected %.10f\n",
                           i, j, A_full[i*N+j], A_ref[i*N+j]);
                    ok = false;
                }
        if (ok) printf("Result is CORRECT!\n");

        if (N <= 12) {
            print_lower("L (MPI)", A_full, N);
            print_lower("L (ref)", A_ref, N);
        }
    }

    MPI_Finalize();
    return 0;
}
