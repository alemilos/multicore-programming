#ifndef UTILS_SERIAL_H
#define UTILS_SERIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

int Get_Input(int **, size_t *);
void Full_Trim(char *, size_t *);
void Print_Array(int *, size_t);
void Bubble_Sort(int *, size_t);

#endif