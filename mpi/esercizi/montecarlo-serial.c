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

double Random(void);
double pi_estimate_serial(int argc, char const *argv[]);

int main(int argc, char const *argv[]){
    double pi_estimate = pi_estimate_serial(argc, argv);
    printf("The PI estimate is %f\n\n",pi_estimate);
    return 0;
}


/**
 * Random integer between -1 and 1 
 */
double Random(){
    return ((double)rand()/RAND_MAX) * 2.0 - 1.0;
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

