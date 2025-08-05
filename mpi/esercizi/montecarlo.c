/**
 * Randomicamente lancio freccette su un quadrato con 2 metri di lato.
 * All'interno del quadrato c'è un cerchio che ha raggio 1 metro e area PI metri quadri. 
 * Usiamo la seguente formula per determinare il valore di PI.
 * 
 * Numero freccette nel cerchio/PI = numero totale di lanci /4
 * Il cerchio deve rispettare la condizione x^2 + y^2 <= r^2
 *  
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi/mpi.h"

double Random(void);
double pi_estimate_serial(int argc, char const *argv[]);
bool Is_in_circle(double,double);

void Get_input(
    int my_rank,  /* in  */
    int comm_sz,  /* in  */
    int* tosses_n /* out */ 
);   



int main(int argc, char const *argv[]){
    srand((unsigned int)clock());

    int my_rank, comm_sz, tosses_n;
    int local_tosses;
    int total_darts_in_circle, local_darts_in_circle = 0;
    double x,y;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // 1. Get the input on process 0
    Get_input(my_rank, comm_sz, &tosses_n);
    local_tosses = tosses_n / comm_sz;

    // 2. Make each process generate random couples (x,y) 
    for (size_t i = 0; i < local_tosses; i++){
        x = Random();
        y = Random();
        if (Is_in_circle(x,y)) local_darts_in_circle++;
    }

    // 3. Use the reduce to make the final sum
    MPI_Reduce(&local_darts_in_circle, &total_darts_in_circle, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // If we want we can broadcast to all the processes the resulting sum
    // MPI_Bcast(&total_darts_in_circle, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Or by removing the MPI_Reduce above, adding the next line
    // MPI_Allreduce(&local_darts_in_circle, &total_darts_in_circle, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    // printf("%d is the total for proc %d\n", total_darts_in_circle, my_rank);

    if (my_rank == 0){
        double pi_estimate = 4*total_darts_in_circle/(double)tosses_n;
        printf("The PI estimate is %f\n\n",pi_estimate);
    }

    MPI_Finalize();
    return 0;
}


/**
 * Random integer between -1 and 1 
 */
double Random(){
    return ((double)rand()/RAND_MAX) * 2.0 - 1.0;
}

bool Is_in_circle(double x, double y){
    return x*x + y*y <= 1;
}

void Get_input(int my_rank, int comm_sz, int *tosses_n){
    if (my_rank == 0){
        printf("Input the number of tosses: \n");
        scanf("%d", tosses_n);
    }

    // Broadcast the input to all the other processes
    MPI_Bcast(tosses_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

/**
 * The serial implementation of the monte carlo problem 
 */
double pi_estimate_serial(int argc, char const *argv[]){
    // Random number generator initializer
    srand((unsigned int)clock());

    if (argc != 2) {
        fprintf(stderr, "Error:\n- usage: %s tosses_number\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int tosses = atoi(argv[1]);
    if (tosses <= 0){
        perror("Error: tosses_number must be a positive integer\n");
        exit(EXIT_FAILURE);
    }

    double x, y;
    int numbers_in_circle = 0;
    for (size_t i = 0; i < tosses; i++){
        x = Random();
        y = Random();
        // Check (x,y) is in the circle (note: r = 1 -> r^2 = 1)
        if (x*x + y*y <= 1*1) {
            numbers_in_circle++;
        }
    }

    double pi_estimate = 4*numbers_in_circle/(double)tosses;
    return pi_estimate;
}

