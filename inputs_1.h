#ifndef INPUTS_1_H
#define INPUTS_1_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_LABEL_LEN 29
#define WORD_LEN 10

typedef enum { IC_TYPE, DC_TYPE } CodeType;

typedef struct mat
{
    char* name;
    int** nums;
    int row;
    int colom;
    struct mat* next;
}mat;

typedef struct binary
{
    short word;
    struct binary* next;
}binary;

typedef struct labels
{
    unsigned short address;
    char* label_name;
    char* type;/*data\code*/
    struct labels* next;
}labels;

typedef struct ic
{
    char* code_value;
    int addres;
    struct ic* next;
}ic_code;

typedef struct dc
{
    char* code_value;
    int addres;
    struct dc* next;
}dc_code;

char* num_to_by_code(int num);

int add_dc_or_ic(void** head, short dc_ic, int address, CodeType type);

int add_label(labels** head, unsigned short address, const char* name, const char* guiding_name);

int check_if_operation_in_language(const char* name, int line, char* file, char** operation_list);

int check_label_validation(char* name, int line, char* file, labels* sy_tabel);

int add_mat(mat** head, mat* node_add, int line, char* file);

int get_num(char* num, int line, char* name);

int* get_str(char* str, int line, char* name);

int get_integer_mat(char* mat, int line, char* name);

mat* get_mat(char* mat_info, int line, char* name);

int* code_to_binary(char* word, int line, char* file_name, int* IC);

#endif /* INPUTS_1_H */
