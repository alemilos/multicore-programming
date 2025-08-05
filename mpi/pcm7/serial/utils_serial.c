#include "utils_serial.h"

int Get_Input(int **vector, size_t *vector_sz){
    *vector_sz = 0;
    printf("Type your whitespace separated unsorted array: ");

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
}

void Bubble_Sort(int *vector, size_t vector_sz){
    for (size_t i = 0; i < vector_sz; i++){
        for (size_t j = 0; j < vector_sz-i-1; j++){
            if (vector[j+1] < vector[j]){
                // swap 
                int tmp = vector[j];
                vector[j] = vector[j+1];
                vector[j+1] = tmp;
            }
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

void Print_Array(int *vector, size_t vector_sz){
    printf("[");
    for (size_t i = 0; i < vector_sz; i++){
       if (i < vector_sz-1) printf("%d,", vector[i]);
       else printf("%d]\n", vector[i]);
    }
}