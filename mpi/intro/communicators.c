#include "stdio.h"
#include "mpi.h"

int main(int argc, char* argv[]){
    int comm_sz, my_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    printf("Hello world from process %d out of %d\n", my_rank, comm_sz);
    MPI_Finalize();
    return 0;
}