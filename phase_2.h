#ifndef PHASE_2_H
#define PHASE_2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_asembler.h"
#include "inputs_1.h"

#define PHASE_2 2
#define MAX_MEM 256
#define START_COUNT 100

typedef struct data_collect_phase_2
{
    int DC;
    int IC;
    dc_code* dc_values;
    ic_code* ic_value;
}data_collect_phase_2;

data_collect_phase_2* phase_2(FILE* file_asm, char* name, state* data_collector);

#endif /* PHASE_2_H */
