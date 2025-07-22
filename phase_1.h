#ifndef PHASE_1_H
#define PHASE_1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* This struct saves all of the labels that we encounter until now (phase 1) */
typedef struct labels {
    unsigned short address;
    struct labels* next;
} labels;

/* A linked list that is used to save the binary code of one line */
typedef struct binary {
    unsigned short b_word;
    struct binary* next;
} binary;

/* This struct is used to save all of the IC binary code (that we know in phase 1) and count its lines */
typedef struct ic {
    int line;
    binary* code;
} ic_code;

/* This struct is used to save all of the DC binary code (that we know in phase 1) and count its lines */
typedef struct dc {
    int line;
    binary* code;
} dc_code;

/* Returns 1 if 'name' is a valid name of operation and returns -1 if 'name' is not a valid operation name */
int check_if_operation_in_language(const char* name, int line);

#endif /* PHASE_1_H */
