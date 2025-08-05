#include <stdlib.h>
#include <stdio.h>

#include <mpi/mpi.h>

/**
 * One measurement is not enough for determining how long a computation takes.
 * A common problem is: When each process start ?  
 * To face this issue, we can use MPI_Barrier (which is not good for production since it could have different exit times, but for our use case it's ok)
 * 
 */

int main(){
    int comm_sz, my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Barrier(MPI_COMM_WORLD);


    double local_start, local_finish, local_elapsed, elapsed;
    local_start = MPI_Wtime();
    printf("Proc %d > Started at %e\n",my_rank, local_start);

    /**
     * Some computation
     */

    local_finish = MPI_Wtime();
    local_elapsed = local_finish - local_start;

    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank == 0){
        printf("Elapsed time = %e seconds\n", elapsed);
    }

    MPI_Finalize();
}
