// Author Linus Lind: MIT License
// Parallel Cholesky Decomposition using MPI
//
// Strategy: 1D cyclic column distribution
//
//   Columns are assigned round-robin across ranks:
//     rank r owns global columns r, r+nprocs, r+2*nprocs, ...
//
//   Algorithm (right-looking):
//     for k = 0 .. N-1:
//       owner of column k computes the pivot column (sqrt + divide)
//       owner broadcasts column k to all ranks              <- MPI_Bcast
//       every rank updates its trailing local columns       <- local BLAS-1
//
//   Distribution / collection:
//     rank 0 sends each column to its owner                <- MPI_Send / MPI_Recv
//     rank 0 gathers results at the end                    <- MPI_Send / MPI_Recv
//
// Compile: mpicxx -O2 -o cholesky matrix_operations_mpi.cpp -lm
// Run:     mpirun -n 4 ./cholesky

#include <mpi.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

const int N = 8; // matrix dimension (keep small while debugging)

// Local storage convention:
//   local_A[l * N + i]  =  A[i][ l*nprocs + rank ]
//   i.e. local columns are stored contiguously (column-major within each rank)

// ---------------------------------------------------------------------------
// Helpers (already implemented)
// ---------------------------------------------------------------------------

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
        A[i*n+i] = rowsum + 1.0; // diagonal dominance => SPD
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

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // Count local columns (cyclic assignment)
    int ncols_local = 0;
    for (int j = rank; j < N; j += nprocs)
        ncols_local++;

    std::vector<double> local_A(N * ncols_local, 0.0);
    std::vector<double> A_full, A_ref;

    // --- Setup ---

    if (rank == 0) {
        generate_spd(A_full, N);
        A_ref = A_full;
        cholesky_serial(A_ref, N); // reference result for verification
    }

    // TODO 1: Distribute columns of A_full from rank 0 to all ranks.
    //
    // Global column j belongs to rank (j % nprocs), with local index (j / nprocs).
    //
    // On rank 0: send column j (N doubles starting at A_full[0*N+j], stride N)
    //   to its owner using MPI_Send. Pack the column into a temporary buffer first.
    //
    // On every other rank: receive each of your columns with MPI_Recv into
    //   local_A[l * N ... l * N + N - 1]  where l is the local column index.
    //
    // Rank 0 must also copy its own local columns from A_full directly.

    if (rank == 0) {
        double col_buf[N];
        for (int j = 0; j < N; j++) {          // iterate over columns
            for (int j = 0; j < N; j++)
                col_buf[j] = A_full[i*N+j];
            int owner = i % nprocs;
            MPI_Send(col_buf, N, MPI_DOUBLE, owner, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Status status;
        for (int l = 0; l < ncols_local; l++) {
            int j = l * nprocs + rank;
            MPI_Recv(&local_A[l * N], N, MPI_DOUBLE, 0, j, MPI_COMM_WORLD, &status);
        }
    }


    // --- Parallel Cholesky ---

    std::vector<double> pivot_col(N, 0.0);

    for (int k = 0; k < N; k++) {
        int owner   = k % nprocs;
        int k_local = k / nprocs; // local index of column k on the owner rank

        // TODO 2: Owner computes the pivot column (in-place on local_A).
        //
        // if (rank == owner):
        //   (a) diagonal element:
        //       local_A[k_local*N + k] = sqrt(local_A[k_local*N + k])
        //   (b) subcolumn (i = k+1 .. N-1):
        //       local_A[k_local*N + i] /= local_A[k_local*N + k]
        //   (c) copy the updated column into pivot_col:
        //       pivot_col[i] = local_A[k_local*N + i]  for i = k .. N-1


        // TODO 3: Broadcast pivot_col[k .. N-1] from owner to all ranks.
        //
        // MPI_Bcast(&pivot_col[k], N - k, MPI_DOUBLE, owner, MPI_COMM_WORLD);


        // TODO 4: Each rank updates its trailing local columns.
        //
        // For each local column index l on this rank (global col j = l*nprocs + rank):
        //   if j > k:
        //     for i = j .. N-1:
        //       local_A[l*N + i] -= pivot_col[i] * pivot_col[j]
        //
        // This is the rank-1 update:  A[i][j] -= A[i][k] * A[j][k]
        // Both A[i][k] and A[j][k] are available in pivot_col after the broadcast.
    }

    // --- Gather and verify ---

    // TODO 5: Gather local columns back to rank 0 into A_full.
    //
    // Mirror of TODO 1: each rank sends its local columns to rank 0.
    // Rank 0 unpacks each received column into A_full (column j at A_full[i*N+j]).


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
