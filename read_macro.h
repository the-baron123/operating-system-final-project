#ifndef READ_MACRO_H
#define READ_MACRO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Maximum macro buffer size */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
/*Maximum chars in a line*/
#define MAX_IN_LINE 82

/* Struct for error handling (linked list) */
typedef struct errors {
    char* name_err;
    int line;
    struct errors* next;
} errors;

/* Global variable to the first error node */
extern errors* all_err;

/* Struct for AVL tree node that stores macro info */
typedef struct data {
    char* name;
    char* code;
    struct data* low;
    struct data* high;
    int height;
} data;

/* Error handling */
void add_error(const char* err_msg, int line);

/* Macro operation name validation */
int check_if_operation_in_language(const char* name);

/* AVL tree operations */
data* create_node(const char* name, const char* code);
int get_height(data* n);
int get_balance(data* n);
data* rotate_right(data* prev_root);
data* rotate_left(data* prev_root);
data* insert_avl(data* root, const char* name, const char* code, int line);
data* find_macro(data* root, const char* name);
/* Macro code extraction */
char* get_macro_code(FILE* asm_file);

#endif /* READ_MACRO_H */
