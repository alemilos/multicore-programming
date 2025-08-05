#include "utils_serial.h"

int main(){
    int *vector;
    size_t vector_sz;
    Get_Input(&vector, &vector_sz);
    Print_Array(vector, vector_sz);
    Bubble_Sort(vector, vector_sz);
    Print_Array(vector, vector_sz);
}