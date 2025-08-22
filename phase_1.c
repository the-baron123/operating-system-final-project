/*in this file we are gonna read the file line by line and make for it binary code for our known words. in phase 2 we will complete the process and convert this binary into "abcd"*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_asembler.h"
#include "inputs_1.h"

#define PHASE_1 1
#define PHASE_2 2
#define START_COUNT 100

typedef enum {mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rts, stop}opcode;


/*convert line into a machine code and adding it into the right structure*/
state* read_file1(FILE* file_asm, char* name)
{
    /*start creating vars for the function*/
    state* data_to_return = NULL;
    dc_code* data_code_table = NULL, *temp;
    labels* sy_table = NULL;
    mat *node;
    int line = 0, len = -1, num = 0, operation_value, all_count = START_COUNT;/*assuming that the file is good until proven wrong*/
    int *str_ascii, *save_str, empty_mat = 0;
    int IC = 0,DC = 0, i = 0, j = 0, count_ic = 0;
    char buffer[MAX_IN_LINE+1], catch_word_array[MAX_IN_LINE+1], *catch_word, *word, *sy_name, *start, *end, *save;

    /*creating an array of all the operation in the language*/
    char *operation_list[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};

    /*allocating memory to all of my lists and structs and strings*/
    sy_name = (char*)malloc(sizeof(char)*MAX_LABEL_LEN);
    if(!sy_name)
    {
        fprintf(stdout, "Memory allocation failed for data_to_return\n");
        exit(1);
    }
    data_to_return = (state*)malloc(sizeof(state));
    if (!data_to_return)
    {
        fprintf(stdout, "Memory allocation failed for data_to_return\n");
        exit(1);
    }
    data_code_table = (dc_code*)malloc(sizeof(dc_code));
    if (!data_code_table)
    {
        fprintf(stdout, "Memory allocation failed for data_code_table\n");
        exit(1);
    }
    sy_table = (labels*)malloc(sizeof(labels));
    if (!sy_table)
    {
        fprintf(stdout, "Memory allocation failed for sy_table\n");
        exit(1);
    }
    /*end allocating memory*/
    /*start setting the memory*/
    data_to_return->data_code = NULL;
    data_to_return->label = NULL;

    data_code_table->next = NULL;

    sy_table->label_name = NULL;
    sy_table->next = NULL;
    sy_table->type = NULL;
    /*end setting the memory*/
    /*opening the file to read*/
    file_asm = fopen(name,"r");
    /*start reading from the file*/
    while(fgets(buffer, MAX_IN_LINE, file_asm) != NULL)
    {
        printf("%d", DC);
        empty_mat = 0;
        line++;
        printf("line %d\n",line);
        word = buffer;
        while(isspace(*word)) word++;
        if((len = strcspn(word, ":")) > MAX_LABEL_LEN && word[len] != '\0')/*found ":" and it was on index bigger than 30*/
        {
            add_error("label is too long ", line, name);
            continue;
        }
        strcpy(sy_name, word);
        if((len = strcspn(word, ":")) <= MAX_LABEL_LEN && word[len] != '\0')/*found a label*/
        {
            save = strchr(word, ':');
            save++;
            while(*save != '\0' && isspace(*save)) save++;
            if(*save == '\0')
            {
                add_error("label is empty ",line, name);
                continue;
            }
            start = sy_name;
            while(*start != ':') start++;
            end = start;
            *end = '\0';
            if(check_label_validation(sy_name, end, line, name, sy_table, PHASE_1))
            {
                word = strchr(word, ':');
                word++;
                if(*word == '\0' || !isspace(*word))/*check if its a space*/
                {
                    add_error("missing space after \":\" ", line, name);
                    continue;
                }
                while(isspace(*word)) word++;/*skipping all other white spaces(if there are some)*/
                strcpy(catch_word_array, word);
                catch_word = catch_word_array;
                catch_word = strtok(catch_word, " ");
                if(!catch_word)
                {
                    add_error("missing space ",line, name);
                    continue;
                }

                if(strcmp(catch_word, ".data") == 0)
                {
                    if(add_label(&sy_table, all_count, sy_name, ".data", line, name) == 0)/*cheeking if label added correctly*/
                    {
                        continue;
                    }
                    /*skipping .data*/
                    while(isspace(*word)) word++;/*"    .data" ->-> ".data"*/
                    word = strchr(word, 'a');
                    word++;
                    word = strchr(word, 'a');
                    word++;
                    /*checking char after word*/
                    if(*word == '\0' || !isspace(*word))
                    {
                        add_error("missing space ",line ,name);
                        continue;
                    }
                    while(*word != '\0' && isspace(*word)) word++;
                    if(*word == '\0')
                    {
                        add_error("missing parameters ",line ,name);
                        continue;
                    }
                    /*getting the numbers*/
                    start = word;
                    end = strchr(word, ',');
                    if(end == NULL)/*only one number or none(err)*/
                    {
                        num = get_last_num(start,line,name);
                        if(num == 1 << 11)
                        {
                            continue;
                        }
                        add_dc(&data_code_table, num, all_count);
                        all_count++;
                        DC++;
                        continue;
                    }
                    while(end != NULL)/*getting the numbers*/
                    {
                        num = get_num(start, end, line, name);
                        if(num == 1 << 11)
                        {
                            break;
                        }
                        /*add it into "dc" structure*/
                        add_dc(&data_code_table, num, all_count);
                        all_count++;
                        DC++;
                        start = end+1;
                        end = strchr(start, ',');
                    }
                    num = get_last_num(start,line,name);/*getting last number*/
                    if(num == 1 << 11)
                    {
                        continue;
                    }
                    /*add it into "dc" structure*/
                    add_dc(&data_code_table, num, all_count);
                    all_count++;
                    DC++;
                }
                else if(strcmp(catch_word, ".string") == 0)
                {
                    /*add label*/
                    if(add_label(&sy_table, all_count, sy_name, ".data", line, name) == 0)/*cheeking if label added correctly*/
                    {
                        continue;
                    }
                    /*skipping .string*/
                    word = strchr(word, 'g');
                    word++;
                    /*checking char after .string*/
                    if(*word == '\0' || !isspace(*word))
                    {
                        add_error("missing space ",line ,name);
                        continue;
                    }
                    while(*word != '\0' && isspace(*word)) word++;/*skipping space*/
                    if(*word == '\0')
                    {
                        add_error("missing parameters ",line ,name);
                        continue;
                    }

                    start = strchr(word, '"');/*go to first \" */
                    if(start == NULL)
                    {
                        add_error("missing \" ", line, name);
                        continue;
                    }
                    start++;
                    end = start;
                    while(strchr(end+1, '"') != NULL)/*go to last '"'*/
                    {
                        end = strchr(end+1, '"');
                    }
                    save = (end+1);
                    while(*save != '\0' && isspace(*save)) save++;/*skipping space*/
                    if(*save != '\0')
                    {
                        add_error("illegal text after end of string ",line, name);
                        continue;
                    }
                    str_ascii = get_str(start, end, line, name);/*getting str*/
                    save_str = str_ascii;
                    if(str_ascii == NULL)
                    {
                        continue;
                    }
                    while(*str_ascii != 0)
                    {
                        /*add it into "dc" structure*/
                        add_dc(&data_code_table, *str_ascii, all_count);
                        all_count++;
                        DC++;
                        str_ascii++;
                    }
                    /*add 0 to "dc" to end the string*/
                    add_dc(&data_code_table, 0, all_count);
                    all_count++;
                    DC++;
                    free(save_str);/*free memory*/
                }
                else if(strcmp(catch_word, ".mat") == 0)/*need to get the size of the mat and after that its simple*/
                {
                    /*add label*/
                    if(add_label(&sy_table, all_count, sy_name, ".data", line, name) == 0)/*cheeking if label added correctly*/
                    {
                        continue;
                    }
                    /*skipping word .mat*/
                    while(isspace(*word)) word++;/*example: "    .data" ->-> ".data"*/
                    word = strchr(word, 't');
                    word++;
                    /*checking char after it*/
                    if(*word == '\0' || !isspace(*word))
                    {
                        add_error("missing space ",line ,name);
                        continue;
                    }
                    while(*word != '\0' && isspace(*word)) word++;/*skipping all white chars*/
                    if(*word == '\0')
                    {
                        add_error("missing args ",line ,name);
                        continue;
                    }
                    /*basic validation checks on matrix*/
                    end = strchr(word, ']');
                    if(!end)
                    {
                        add_error("missing ']' ",line ,name);
                        continue;
                    }
                    end = strchr((end+1),']');
                    if(!end)
                    {
                        add_error("missing ']' ",line ,name);
                        continue;
                    }
                    end++;
                    while(*end != '\0' && isspace(*end)) end++;/*skipping space*/
                    if(*end == '\0')
                    {
                        empty_mat = 1;
                    }
                    if((node = get_mat(word, line, name, empty_mat)) == NULL)/*getting the mat*/
                    {
                        continue;
                    }
                    /*adding the nums I collect to the "dc" structure*/
                    for(i = 0; i< node->row;i++)
                    {
                        for(j = 0; j< node->colom;j++)
                        {
                            add_dc(&data_code_table, node->nums[i][j], all_count);
                            all_count++;
                            DC++;
                        }
                    }
                    free_mat_list(&node);/*free mat*/
                    node = NULL;
                }
                else if(strcmp(catch_word, ".entry") == 0 || strcmp(catch_word, ".extern") == 0)/*add it into sy_table*/
                {
                    while(*word != '\0' && !isspace(*word)) word++;
                    if(*word == '\0')
                    {
                        add_error("missing space ",line, name);
                        continue;
                    }
                    while(*word != '\0' && isspace(*word)) word++;
                    if(*word == '\0')
                    {
                        add_error("missing parameters ",line, name);
                        continue;
                    }
                    strcpy(sy_name, word);
                    start = sy_name;
                    while(!isspace(*start)) start++;
                    end = start;
                    *end = '\0';
                    if(check_label_validation(sy_name, end, line, name, sy_table, PHASE_1) == 0)/*label already exist or problem with it's name*/
                    {
                        continue;
                    }
                    /*adding the label*/
                    printf("label:%s:, type:%s:",sy_name, catch_word);
                    (strcmp(catch_word, ".entry") == 0)? add_label(&sy_table, all_count++, sy_name, catch_word, line, name):add_label(&sy_table, 0, sy_name, catch_word, line, name);/*adding extern label or entry*/
                    continue;
                }
                else
                {
                    /*skipping space*/
                    while(*word != '\0' && isspace(*word)) word++;
                    if(*word == '\0')
                    {
                        add_error("missing parameters ",line, name);
                        continue;
                    }
                    /*copying word*/
                    strcpy(catch_word_array, word);
                    catch_word = catch_word_array;
                    strtok(catch_word, " ");
                    while(*word != '\0' && !isspace(*word)) word++;
                    if(*word == '\0')
                    {
                        add_error("missing parameters ",line, name);
                        continue;
                    }
                    if((operation_value = check_if_operation_in_language(catch_word, operation_list)) != -1)/*checking if the word is a saved word in the language*/
                    {
                    /*add it to "ic" structure*/
                    switch((opcode)operation_value)
                    {
                    case mov:   /* opcode 0: mov */
                    case add:   /* opcode 2: add */
                    case sub:   /* opcode 3: sub */
                        count_ic = count_ic_on_word_zero(word, line, name);
                        break;
                    case cmp:   /* opcode 1: cmp */
                        count_ic = count_ic_on_word_one(word, line, name);
                        break;
                    case lea:   /* opcode 4: lea */
                        count_ic = count_ic_on_word_four(word, line, name);
                        break;
                    case clr:   /* opcode 5: clr */
                    case not:   /* opcode 6: not */
                    case inc:   /* opcode 7: inc */
                    case dec:   /* opcode 8: dec */
                    case jmp:   /* opcode 9: jmp */
                    case bne:   /* opcode 10: bne */
                    case jsr:   /* opcode 11: jsr */
                    case red:   /* opcode 12: red */
                        count_ic = count_ic_on_word_five(word, line, name);
                        break;
                    case prn:   /* opcode 13: prn */
                        count_ic = count_ic_on_word_thirteen(word, line, name);
                        break;
                    case rts:   /* opcode 14: rts */
                    case stop:  /* opcode 15: stop */
                        count_ic = count_ic_on_word_fourteen(word, line, name);
                        break;
                    }
                    /*add label to the symbol table*/
                    add_label(&sy_table, all_count, sy_name, ".code", line, name);
                    /*update counters*/
                    all_count += count_ic;
                    IC += count_ic;
                    continue;
                    }
                    else
                    {
                        add_error("command does not exist in language ",line ,name);
                        continue;
                    }
                }
            }
        }
        else
        {
            /*getting */
            strtok(sy_name, " ");/*no need to check if word is NULL because we erase in the pre_asembler all empty lines*/

            if(strcmp(sy_name, ".data") == 0)
            {
                while(isspace(*word)) word++;/*"    .data" ->-> ".data"*/
                word = strchr(word, 'a');
                word++;
                word = strchr(word, 'a');
                word++;
                if(*word == '\0' || !isspace(*word))
                {
                    add_error("missing space ",line ,name);
                    continue;
                }
                while(*word != '\0' && isspace(*word)) word++;
                if(*word == '\0')
                {
                    add_error("missing parameters ",line ,name);
                    continue;
                }

                start = word;
                end = strchr(word, ',');
                if(end == NULL)
                {
                    num = get_last_num(start,line,name);
                    if(num == 1 << 11)
                    {
                        continue;
                    }
                    add_dc(&data_code_table, num, all_count);
                    all_count++;
                    DC++;
                    continue;
                }
                while(end != NULL)
                {
                    num = get_num(start, end, line, name);
                    if(num == 1 << 11)
                    {
                        break;
                    }
                    /*add it into "dc" structure*/
                    add_dc(&data_code_table, num, all_count);
                    all_count++;
                    DC++;
                    start = end+1;
                    end = strchr(start, ',');
                }
                num = get_last_num(start,line,name);
                if(num == 1 << 11)
                {
                    continue;
                }
                /*add it into "dc" structure*/
                add_dc(&data_code_table, num, all_count);
                all_count++;
                DC++;
            }
            else if(strcmp(sy_name, ".string") == 0)
            {

                word = strchr(word, 'g');
                word++;
                if(*word == '\0' || !isspace(*word))
                {
                    add_error("missing space ",line ,name);
                    continue;
                }
                while(*word != '\0' && isspace(*word)) word++;
                if(*word == '\0')
                {
                    add_error("missing parameters ",line ,name);
                    continue;
                }

                start = strchr(word, '"');/*go to first \" */
                if(start == NULL)
                {
                    add_error("missing \" ", line, name);
                    continue;
                }
                start++;
                end = start;
                while(strchr(end+1, '"') != NULL)
                {
                    end = strchr(end+1, '"');
                }
                word = (end+1);
                while(*word != '\0' && isspace(*word)) word++;
                if(*word != '\0')
                {
                    add_error("illegal text after end of string ",line, name);
                    continue;
                }
                str_ascii = get_str(start, end, line, name);
                save_str = str_ascii;
                if(str_ascii == NULL)
                {
                    continue;
                }
                while(*str_ascii != 0)
                {
                    /*add it into "dc" structure*/
                    add_dc(&data_code_table, *str_ascii, all_count);
                    all_count++;
                    DC++;
                    str_ascii++;
                }
                /*add 0 to "dc" to end the string*/
                add_dc(&data_code_table, 0, all_count);
                all_count++;
                DC++;
                free(save_str);
            }
            else if(strcmp(sy_name, ".mat") == 0)/*need to get the size of the mat and after that its simple*/
            {
                while(isspace(*word)) word++;/*example: "    .data" ->-> ".data"*/
                word = strchr(word, 't');
                word++;
                if(*word == '\0' || !isspace(*word))
                {
                    add_error("missing space ",line ,name);
                    continue;
                }
                while(*word != '\0' && isspace(*word)) word++;/*skipping all white chars*/
                if(*word == '\0')
                {
                    add_error("missing args ",line ,name);
                    continue;
                }
                end = strchr(word, ']');
                if(!end)
                {
                    add_error("missing ']' ",line ,name);
                    continue;
                }
                end = strchr((end+1),']');
                if(!end)
                {
                    add_error("missing ']' ",line ,name);
                    continue;
                }
                end++;
                while(*end != '\0' && isspace(*end)) end++;
                if(*end == '\0')
                {
                    empty_mat = 1;
                }
                if((node = get_mat(word, line, name, empty_mat)) == NULL)
                {
                    continue;
                }

                /*adding the nums I collect to the "dc" structure*/
                for(i = 0; i< node->row;i++)
                {
                    for(j = 0; j< node->colom;j++)
                    {
                        add_dc(&data_code_table, node->nums[i][j], all_count);
                        all_count++;
                        DC++;
                    }
                }
                free_mat_list(&node);
                node = NULL;
            }
            else if(strcmp(sy_name, ".entry") == 0 || strcmp(sy_name, ".extern") == 0)/*ignoring it completely until phase 2*/
            {
                while(*word != '\0' && !isspace(*word)) word++;/*skipping ".entry"\".extern"*/
                while(*word != '\0' && isspace(*word)) word++;
                if(*word == '\0')
                {
                    add_error("missing parameters ", line, name);
                    continue;
                }
                strcpy(catch_word_array, word);
                catch_word = catch_word_array;

                if(!catch_word)
                {
                    add_error("missing space or parameters ",line, name);
                    continue;
                }
                end = catch_word;
                while(*word != '\0' && !isspace(*end)) end++;
                *end = '\0';
                if(check_label_validation(catch_word, end, line, name, sy_table, PHASE_1) == 0)/*label already exist or problem with it's name*/
                {
                    continue;
                }
                (strcmp(catch_word, ".entry") == 0)? add_label(&sy_table, all_count++, catch_word, sy_name, line, name):add_label(&sy_table, 0, catch_word, sy_name, line, name);/*adding extern label or entry*/
                continue;
            }
            else
            {
                strcpy(catch_word_array, word);
                catch_word = catch_word_array;
                strtok(catch_word, " ");
                if((operation_value = check_if_operation_in_language(catch_word, operation_list)) != -1)/*checking if the word is a saved word in the language*/
                {
                    if(operation_value < 14)
                    {
                        while(*word != '\0' && !isspace(*word)) word++;
                        if(*word == '\0')
                        {
                            add_error("missing parameters ",line, name);
                            continue;
                        }
                        while(*word != '\0' && isspace(*word)) word++;
                        if(*word == '\0')
                        {
                            add_error("missing parameters ",line, name);
                            continue;
                        }
                    }
                    /*add it to "ic" structure*/
                    switch((opcode)operation_value)
                    {
                    case mov:   /* opcode 0: mov */
                    case add:   /* opcode 2: add */
                    case sub:   /* opcode 3: sub */
                        count_ic = count_ic_on_word_zero(word, line, name);
                        break;
                    case cmp:   /* opcode 1: cmp */
                        count_ic = count_ic_on_word_one(word, line, name);
                        break;
                    case lea:   /* opcode 4: lea */
                        count_ic = count_ic_on_word_four(word, line, name);
                        break;
                    case clr:   /* opcode 5: clr */
                    case not:   /* opcode 6: not */
                    case inc:   /* opcode 7: inc */
                    case dec:   /* opcode 8: dec */
                    case jmp:   /* opcode 9: jmp */
                    case bne:   /* opcode 10: bne */
                    case jsr:   /* opcode 11: jsr */
                    case red:   /* opcode 12: red */
                        count_ic = count_ic_on_word_five(word, line, name);
                        break;
                    case prn:   /* opcode 13: prn */
                        count_ic = count_ic_on_word_thirteen(word, line, name);
                        break;
                    case rts:   /* opcode 14: rts */
                    case stop:  /* opcode 15: stop */
                        count_ic = count_ic_on_word_fourteen(word, line, name);
                        break;
                    }
                    /*update counters*/
                    all_count += count_ic;
                    IC += count_ic;
                    continue;
                }
                else
                {
                    add_error("command does not exist in language ",line ,name);
                    continue;
                }
            }
        }
    }
    temp = data_code_table;
    data_code_table = data_code_table->next;
    free(temp);
    free(sy_name);

    /*setting data_to_return*/
    data_to_return->label = sy_table;
    data_to_return->data_code = data_code_table;
    data_to_return->dc_count = DC;
    data_to_return->ic_count = IC;

    return data_to_return;
}

