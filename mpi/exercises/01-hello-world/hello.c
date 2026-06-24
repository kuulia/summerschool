#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("Hello from rank %d of %d!\n", rank, size);

    if (rank == 0) {
        printf("Program has %d processes!\n", size);
    }

    if (rank == 42) {
        printf("I'm the Answer to the Ultimate Question of Life, the Universe, and Everything!\n");
    }

    if (rank == size-1) {
        printf("This is the last process (rank %d)!\n", rank);
        printf("I'm the last but not least!\n");
    }

    

    MPI_Finalize();
    return 0;
}