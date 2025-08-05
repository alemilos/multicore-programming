#include <mpi/mpi.h>
#include "utils.h"

#define MPI_ROOT_RANK 0

int main(){
    int comm_sz, my_rank; // mpi configs 
    int *vector;
    int vector_sz = 0; 

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Barrier(MPI_COMM_WORLD);

    // Retrieve Input from the user
    if (my_rank == 0){
        Get_input(&vector, &vector_sz);
    }

    // Clean up
 
    MPI_Finalize();

    return 0;
}

