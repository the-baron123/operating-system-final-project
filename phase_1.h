#ifndef PHASE_1_H
#define PHASE_1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_asembler.h"
#include "inputs_1.h"

#define PHASE_2 2

/*reading the file and returning the important data*/
state* read_file1(FILE* file_asm, char* name);

#endif /* PHASE_1_H */
