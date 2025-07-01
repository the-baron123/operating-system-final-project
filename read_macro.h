/* read_macro.h - Header file for macro and error linked list handling */

#ifndef READ_MACRO_H
#define READ_MACRO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Maximum number of characters in a line */
#define MAX_IN_LINE 82
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Linked list node for errors */
typedef struct errors {
    char* name_err;
    int line;
    struct errors* next;
} errors;

/* Linked list node for macros */
typedef struct data {
    char* name;
    char* code;
    struct data* next;
} data;

/* Global heads for linked lists */
extern errors* all_err;
extern data* root;

/* Macro linked list functions */
data* create_node(const char* name, const char* code);
void add_node(data** head, const char* name, const char* code);
data* find_node(data* head, const char* name);

/* Error linked list function */
void add_error(const char* err_msg, int line);

/* Check if a string is a valid operation name */
int check_if_operation_in_language(const char* name);

/* Extract macro code from assembly file */
char* get_macro_code(FILE* asm_file);

#endif /* READ_MACRO_H */
