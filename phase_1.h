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

/* This struct is used to save all of the IC binary code (that we know in phase 1) and count its lines */
typedef struct ic {
    int line;
    short code;
    struct ic* next;
} ic_code;

/* This struct is used to save all of the DC binary code (that we know in phase 1) and count its lines */
typedef struct dc {
    int line;
    short code;
    struct dc* next;
} dc_code;

/* Returns 1 if 'name' is a valid name of operation and returns -1 if 'name' is not a valid operation name */
int check_if_operation_in_language(const char* name, int line);

#endif /* PHASE_1_H */
