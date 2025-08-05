#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <mpi/mpi.h>


#define MPI_ROOT_RANK 0

void Get_input(
    int my_rank, 
    int comm_sz,
    int **vector,
    int *vector_sz
);

int Read_vector(FILE *file, int**vector, int *vector_sz);
void Display_vector(int*vector, int vector_sz);
void Full_Trim(char *s, size_t *len);
void Print_Local_Vectors(int my_rank, int comm_sz, int *local_vector, int local_vector_sz);
int Calculate_Parnter(int my_rank, int comm_sz, int phase);
void Local_Sort(int*vector, size_t vector_sz);

int main(){
    int comm_sz, my_rank; // mpi configs 
    int *vector;
    int vector_sz = 0; 

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Barrier(MPI_COMM_WORLD);


    double local_start, local_finish, local_elapsed, elapsed;
    local_start = MPI_Wtime();


    // Retrieve Input from the user
    Get_input(my_rank, comm_sz, &vector, &vector_sz);

    // Broadcast vector_sz to all other elements so they know how big is the real array
    MPI_Bcast(&vector_sz, 1, MPI_INT, MPI_ROOT_RANK, MPI_COMM_WORLD);

    int *sendcounts = NULL;
    int *displs = NULL;

    int base = vector_sz / comm_sz;
    int remainder = vector_sz % comm_sz;
    int local_vector_sz = base + (my_rank < remainder ? 1 : 0);
    int *local_vector = malloc(sizeof(int) * local_vector_sz);

    if (my_rank == MPI_ROOT_RANK){
        sendcounts = malloc(sizeof(int) * comm_sz);
        displs = malloc(sizeof(int) * comm_sz);


        int offset = 0;
        for (size_t i = 0; i < comm_sz; i++){
            sendcounts[i] = base + (i < remainder ? 1 : 0);
            displs[i] = offset; // displs contains the starting point to take elements from for each rank 
            offset+=sendcounts[i];
        }
    }

    // Distribute the vector to all the processes
    MPI_Scatterv(vector, sendcounts, displs, MPI_INT, local_vector, local_vector_sz, MPI_INT, MPI_ROOT_RANK, MPI_COMM_WORLD);
    // Print them in order
    // Print_Local_Vectors(my_rank, comm_sz, local_vector, local_vector_sz);

    // Sort the Vectors
    for (int phase = 0; phase < comm_sz; phase++){
        int *partner_vector;
        int partner_vector_sz;

        int partner = Calculate_Parnter(my_rank, comm_sz, phase);
        if (partner == MPI_PROC_NULL) continue;
 

        // Using Sendrecv
        MPI_Sendrecv(&local_vector_sz, 1, MPI_INT, partner, 0, &partner_vector_sz, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        partner_vector = malloc(sizeof(int)*partner_vector_sz);
        MPI_Sendrecv(local_vector, local_vector_sz, MPI_INT, partner, 0, partner_vector, partner_vector_sz, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Share the vectors between partners 
/*         if (my_rank < partner) {
            // Exchange the size of the local vector with partner, then allocate 
            MPI_Send(&local_vector_sz, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(&partner_vector_sz, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            partner_vector = malloc(sizeof(int) * partner_vector_sz); 

            // Exchange the vectors 
            MPI_Send(local_vector, local_vector_sz, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(partner_vector, partner_vector_sz, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

     
        } else {
            // Exchange the size of the local vector with partner, then allocate 
            MPI_Recv(&partner_vector_sz, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&local_vector_sz, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            partner_vector = malloc(sizeof(int) * partner_vector_sz);

            // Exchange the vectors 
            MPI_Recv(partner_vector, partner_vector_sz, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(local_vector, local_vector_sz, MPI_INT, partner, 0, MPI_COMM_WORLD);
        }
 */
        // Perform the sorting and redistribute the vectors
        int merged_sz = local_vector_sz + partner_vector_sz;
        int *merged = malloc(sizeof(int) * merged_sz);
        memcpy(merged, local_vector, local_vector_sz * sizeof(int));
        memcpy(merged+local_vector_sz, partner_vector, partner_vector_sz * sizeof(int));  
    
        // Sort the merged array 
        Local_Sort(merged, merged_sz);

        if (my_rank < partner) {
            memcpy(local_vector, merged, sizeof(int) * local_vector_sz);
        } else {
            memcpy(local_vector, merged + partner_vector_sz, sizeof(int) * local_vector_sz);
        }
    
        // Deallocate  
        free(partner_vector);
        free(merged);
    }

    // Reassamble the vector
    if (my_rank == MPI_ROOT_RANK){
        MPI_Gatherv(local_vector, local_vector_sz, MPI_INT, vector, sendcounts, displs, MPI_INT, MPI_ROOT_RANK, MPI_COMM_WORLD);
    } else {
        MPI_Gatherv(local_vector, local_vector_sz, MPI_INT, NULL, NULL, NULL, MPI_INT, MPI_ROOT_RANK, MPI_COMM_WORLD);
    }

    local_finish = MPI_Wtime();
    local_elapsed = local_finish - local_start;

    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank == 0){
        printf("Elapsed time = %f seconds\n", elapsed);
        printf("output: \n");
        Display_vector(vector, vector_sz);
    }

    // Clean up
    free(local_vector);
    if (my_rank == MPI_ROOT_RANK){
        free(sendcounts);
        free(displs);
        free(vector);
    }


    MPI_Finalize();

    return 0;
}

/* Get User Input */ 
/* void Get_input(int my_rank, int comm_sz, int **vector, int *vector_sz){
    if (my_rank == 0) {
        printf("Input the vector as whitespace separated integer array (e.g. 1 2 3 4 5 6): \n");
        if (Read_vector(vector, vector_sz) == 1){
            exit(EXIT_FAILURE);
        }
    }
} */

/* Get Input from an input.txt file */
void Get_input(int my_rank, int comm_sz, int **vector, int *vector_sz){
    if (my_rank == 0) {
        FILE *file = fopen("input.txt", "r");
        if (!file) {
            perror("Failed to open input file");
            exit(EXIT_FAILURE);
        }

        if (Read_vector(file, vector, vector_sz) == 1){
            fclose(file);
            exit(EXIT_FAILURE);
        }

        fclose(file);
    }
}

int Read_vector(FILE *file, int **vector, int *vector_sz){
    char *input = NULL;
    size_t bufsize = 0;
    ssize_t line_size = getline(&input, &bufsize, file);
    
    if (line_size == -1) {
        perror("Failed to read from file");
        free(input);
        return EXIT_FAILURE;
    }

    size_t len = strlen(input);
    Full_Trim(input, &len);

    // Count spaces to determine size
    *vector_sz = 0;
    for (size_t i = 0; i < len; i++){
        if (input[i] == ' ') (*vector_sz)++;
    }
    if (len > 1) (*vector_sz)++;

    *vector = (int *)malloc(*vector_sz * sizeof(int));
    if (*vector == NULL){
        perror("Failed to allocate vector");
        free(input);
        return EXIT_FAILURE;
    }

    size_t index = 0;
    char *tok = strtok(input, " ");
    while(tok != NULL){
        (*vector)[index++] = atoi(tok);
        tok = strtok(NULL, " ");
    }

    free(input);
    return 0;
}




/* Read a vector from user input */
/* int Read_vector(int **vector, int *vector_sz){
    char input[1024];
    fgets(input, sizeof(input), stdin);
    size_t len = strlen(input);
    Full_Trim(input, &len);

    for (size_t i = 0; i < len; i++){
        if (input[i] == ' ') (*vector_sz)++;
    }
    if (len > 1) (*vector_sz)++; // if len == 1, the list is empty
    
    // Allocate space for the vector
    *vector = (int *)malloc(*vector_sz * sizeof(int));
    if (*vector == NULL){
        perror("Failed reading vector!\n");
        return EXIT_FAILURE;
    }

    // Split the input string into tokens and built the integer vector
    size_t index = 0;
    char *tok = strtok(input, " ");
    while(tok != NULL){
        (*vector)[index++] = atoi(tok);
        tok = strtok(NULL, " ");
    }

    return 0;
} */


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

void Full_Trim(char *s, size_t *len){
    // Remove the line feed  
    if (len > 0 && s[*len-1] == '\n') s[*len-1] = '\0';

    // Sanitize the input removing double whitespaces
    for (size_t i = 0; i < *len; i++){
        if (s[i] == ' ' && i < *len-1 && s[i+1] == ' '){
            size_t j = i+1;
            while(j++ < *len){
                if (s[j] != ' '){
                    size_t z = j; 
                    size_t j = i+1;
                    while (z < *len){
                        s[j++] = s[z++];
                    }
                    *len -= (z-j);
                    break;
                }
            }
        }
    }

    // Trim before
    bool whitespace_found = false;
    for (size_t i = 0; i < *len; i++){
        if (whitespace_found) break;
        if (s[i] != ' ') break; // there is no whitespace at the beginning
        if (s[i] == ' '){
            whitespace_found = true;
            size_t j = i;
            while (j < *len){
                if (s[j] != ' '){
                    size_t z = j;
                    size_t j = i;
                    while (z < *len){
                        s[j++] = s[z++];
                    }
                    *len -= (z-j);
                    break;
                }
                j++;
            }
        }
    }

    // set the correct length 
    size_t i = *len;
    while (i > 0){
        if (s[i]!='\0' && s[i] != ' '){
            s[i+1] = '\0'; // make sure last char is \0 and not ' '  
            *len = i + 2; // account for \0 in the length
            break;
        }
        i--;
    }
}

void Print_Local_Vectors(int my_rank, int comm_sz, int *local_vector, int local_vector_sz){
    // Print the Local Vectors in order
    if (my_rank != MPI_ROOT_RANK){
        MPI_Recv(NULL, 0, MPI_CHAR, my_rank -1 , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } 

    printf("Proc %d ", my_rank);
    Display_vector(local_vector, local_vector_sz);

    if (my_rank != comm_sz-1){
        MPI_Send(NULL, 0, MPI_CHAR, my_rank + 1, 0, MPI_COMM_WORLD);
    }

}

int Calculate_Parnter(int my_rank, int comm_sz, int phase){
    int partner;
    if (phase % 2 == 0){
        // Even Phase
        if (my_rank % 2 == 0) {
            partner = my_rank + 1;
        }else {
            partner = my_rank - 1;
        }
    } else {
        // Odd Phase
        if (my_rank % 2 == 1){
            partner = my_rank + 1;
        } else {
            partner = my_rank - 1;
        }
    }

    if (partner < MPI_ROOT_RANK || partner >= comm_sz) partner = MPI_PROC_NULL;

    return partner;
}

void Local_Sort(int *vector, size_t vector_sz){
    bool sorted = false;

    while (!sorted){
        sorted = true;

        // Even Phase
        for (size_t i = 0; i < vector_sz - 1; i+=2){
            if (vector[i+1] < vector[i]){
                int tmp = vector[i];
                vector[i] = vector[i+1];
                vector[i+1] = tmp;
                sorted = false;
            }
        }

        // Odd Phase
        for (size_t i = 1; i < vector_sz - 1; i+=2){
             if (vector[i+1] < vector[i]){
                int tmp = vector[i];
                vector[i] = vector[i+1];
                vector[i+1] = tmp;
                sorted = false;
            }
        }
    }
}