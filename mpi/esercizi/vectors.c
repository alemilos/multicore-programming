#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi/mpi.h>

void Get_input(
    int my_rank, 
    int comm_sz,
    int **vector_a,
    int *vector_a_sz,
    int **vector_b,
    int *vector_b_sz,
    int *scalar
);
int Read_vector(
    double   *local_a,
    int      local_n,
    int      n,
    char     *vec_name,
    int      my_rank,
    MPI_Comm comm
);

void Vector_scalar_product(int my_rank, int comm_sz, int *vector_in, int vector_sz, int**vector_out, int scalar);
void Dot_product(int my_rank, int comm_sz, int *vector_a, int *vector_b, int vectors_sz, int *result);
void Display_vector(int*vector, int vector_sz);

int main(){
    int comm_sz, my_rank; // mpi configs 
    int *vector_a, *vector_b, scalar; // user input
    int vector_a_sz, vector_b_sz;

    int *vector_scalar, dot_product; // results
    
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    // Execute the multiplication by a scalar and the dot product in parallel  
    Get_input(my_rank, comm_sz, &vector_a, &vector_a_sz, &vector_b, &vector_b_sz, &scalar);

    Display_vector(vector_a, vector_a_sz);
    Display_vector(vector_b, vector_b_sz);

    Vector_scalar_product(0, 0, vector_a, vector_a_sz, &vector_scalar, scalar);
    Display_vector(vector_scalar, vector_a_sz);
    free(vector_scalar);

    Vector_scalar_product(0,0,vector_b, vector_b_sz, &vector_scalar, scalar);
    Display_vector(vector_scalar, vector_b_sz);

    if (vector_a_sz == vector_b_sz){
        Dot_product(0,0, vector_a, vector_b, vector_a_sz, &dot_product);
        printf("Dot Product: %d\n", dot_product);
    }

    // Free Mem
    free(vector_scalar);
    free(vector_a);
    free(vector_b);

    // print the result 
    MPI_Finalize();

    return 0;
}

void Get_input(int my_rank, int comm_sz, int **vector_a, int *vector_a_sz, int **vector_b, int*vector_b_sz, int *scalar){

    if (my_rank == 0) {
        printf("Input the 1st vector as comma separated integers (e.g. 1,2,3,...,5): \n");
        if (Read_vector(vector_a, vector_a_sz) == 1){
            exit(EXIT_FAILURE);
        }
        printf("Input the 2nd vector as comma separated integers (e.g. 1,2,3,...,5): \n");
        if (Read_vector(vector_b, vector_b_sz) == 1){
            exit(EXIT_FAILURE);
        };
        printf("Input the scalar value: \n");
        scanf("%d", scalar);
    }
}


int Read_vector(double *local_a, int local_n, int n, char *vec_name, int my_rank, MPI_Comm comm){
    double *a = NULL;
    int i;
    if (my_rank == 0){
        a = malloc(n * sizeof(double));
        printf("Enter the vector %s\n", vec_name);
        for (i = 0; i < n; i++){
            scanf("%lf", &a[i]);
        }

        MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0, comm);
        free(a);
    }else {
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0, comm);
    }
}

void Vector_scalar_product(int my_rank, int comm_sz, int *vector, int vector_sz, int **vector_out,  int scalar){
    *vector_out = (int*)malloc(vector_sz * sizeof(int));
    for (size_t i = 0; i < vector_sz;i++){
        *(*vector_out+i) = scalar * *(vector+i);
    }
}

void Dot_product(int my_rank, int comm_sz, int *vector_a, int*vector_b, int vectors_sz, int *result){
    *result = 0;
    for (size_t i= 0; i < vectors_sz; i++){
        *result += *(vector_a+i) * *(vector_b+i);
    }
}

void Display_vector(int*vector, int vector_sz){
    printf("[");
    for (size_t i = 0; i < vector_sz; i++){
        if (i < vector_sz-1){
            printf("%d, ", vector[i]);
        }else {
            printf("%d]\n", vector[i]);
        }
    }
}