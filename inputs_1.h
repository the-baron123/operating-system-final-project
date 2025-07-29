#ifndef INPUTS_1_H
#define INPUTS_1_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct mat
{
    char* name;
    int** nums;
}mat;

mat* add_mat(mat** head, mat* node_add, int line, char* file);

int get_num(char* num, int line, char* name);

int* get_str(char* str, int line, char* name);

int get_integer_mat(char* mat, int line, char* name);

mat* get_mat(char* mat, int line, char* name);


#endif /* INPUTS_1_H */
