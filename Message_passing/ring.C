#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int send_data, recv_data;
    int counter = 0;
    int next, prev;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    if (size <= 1) {
        if (rank == 0)
            printf("Number of processes must be greater than 1\n");
        MPI_Finalize();
        return 0;
    }

   
    next = (rank + 1) % size;    
    prev = (rank - 1 + size) % size;

    
    if (rank == 0) {
        send_data = rank;
        MPI_Send(&send_data, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }


    for (int i = 0; i < 10; i++) {

        MPI_Recv(&recv_data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        counter += recv_data;

        send_data = rank;

        MPI_Send(&send_data, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }

    printf("Rank %d: Final Counter = %d\n", rank, counter);

    MPI_Finalize();
    return 0;
}