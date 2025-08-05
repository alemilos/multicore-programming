#include "utils.h"

/* Get Input from an input.txt file */
void Get_input(int **vector, int *vector_sz){
    if (Read_vector(vector, vector_sz) == 1){
        exit(EXIT_FAILURE);
    }
}

int Read_vector(int **vector, int *vector_sz){
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Failed to open input file");
        fclose(file);
        exit(EXIT_FAILURE);
    }



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

    fclose(file);
    return 0;
}