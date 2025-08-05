#include "utils_serial.h"

void Read_Input(int **vector, int *vector_sz);
int Read_vector_from_file(FILE *file, int **vector, int *vector_sz);
void OddEvenSort(int *vector, int vector_sz);

int main(){
    int *vector;
    int vector_sz; 
    Read_Input(&vector, &vector_sz);
    Print_Array(vector, vector_sz);
    OddEvenSort(vector, vector_sz);
    Print_Array(vector, vector_sz);
    
}


void OddEvenSort(int *vector, int vector_sz){
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


void Read_Input(int **vector, int *vector_sz){
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    if (Read_vector_from_file(file, vector, vector_sz) == 1){
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

int Read_vector_from_file(FILE *file, int **vector, int *vector_sz){
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
