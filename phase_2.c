#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_asembler.h"
#include "inputs_1.h"

#define PHASE_2 2
#define MAX_MEM 256
#define START_COUNT 100

typedef enum {mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rts, stop}opcode;

typedef struct data_collect_phase_2
{
    int DC;
    int IC;
    dc_code* dc_values;
    ic_code* ic_value;
}data_collect_phase_2;

data_collect_phase_2* phase_2(FILE* file_asm, char* name, state* data_collector)
{
    char buffer[MAX_IN_LINE+1], *word, *save, *sy_name = NULL, *p_sy_name = NULL, *ent_name, *ext_name, catch_word_array[MAX_IN_LINE+1], *catch_word, *start, *end, *temp;
    int line = 0, len, operation_value, all_count = START_COUNT;
    int num = 0, i = 0, j = 0;/*assuming that the file is good until proven wrong*/
    int *str_ascii, *save_str, empty_mat = 0;
    mat *node;
    FILE* ent = NULL, *ext = NULL;
    ic_code* curr = NULL;
    binary* by_of_word = NULL, *temp_by_code;
    /*creating an array of all the operation in the language*/
    char *operation_list[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};
    data_collect_phase_2* result;

    result = (data_collect_phase_2*)malloc(sizeof(data_collect_phase_2));
    if(!result)
    {
        fprintf(stdout, "memory allocation failed.\n");
        exit(1);
    }

    if((data_collector->ic_count+data_collector->dc_count) > 156)
    {
        printf("DC:%d  IC:%d", data_collector->dc_count, data_collector->ic_count);
        add_error("exceeded max memory(256) ",line, name);
    }
    /*opening the file to read*/
    file_asm = fopen(name,"r");
    while(fgets(buffer, MAX_IN_LINE, file_asm) != NULL)
    {
        empty_mat = 0;
        if(p_sy_name)/*freeing the sy_name if I allocate it and restart it*/
        {
            free(p_sy_name);
            p_sy_name = NULL;
            sy_name = NULL;
        }
        line++;
        printf("I got into line %d\n",line);
        end = strchr(buffer, '\n');/*erasing \n at end line*/
        if(end)
        {
            *end = '\0';
        }
        word = buffer;

        while(*word != '\0' && isspace(*word)) word++;/*skipping all other white spaces(if there are some)*/
        if(*word == '\0')
        {
            add_error("missing parameters ",line, name);
            continue;
        }
        sy_name = (char*)malloc(sizeof(char)*(MAX_IN_LINE+1));/*allocate memory*/
        if(!sy_name)
        {
            fprintf(stdout, "Memory allocation failed for data_to_return\n");
            exit(1);
        }
        p_sy_name = sy_name;
        strcpy(sy_name, buffer);/*copping buffer into sy_name*/
        while(*sy_name != '\0' && isspace(*sy_name)) sy_name++;/*skipping all other white spaces(if there are some)*/

        if((len = strcspn(word, ":")) <= MAX_LABEL_LEN && word[len] != '\0')/*found a label*/
        {
            /*getting label name*/
            start = sy_name;
            while(*start != ':') start++;
            end = start;
            *end = '\0';
            /*checking the label*/
            if(check_label_validation(sy_name, end, line, name, data_collector->label, PHASE_2))
            {
                check_extern(sy_name, data_collector->label, name, all_count);/*if its an extern or entry label insert it into the ent\ext file*/
                word = strchr(word, ':');/*getting over the label*/
                word++;
                if(*word == '\0' || !isspace(*word))/*check if missing space*/
                {
                    add_error("missing space after \":\" ", line, name);
                    continue;
                }
                while(*word != '\0' && isspace(*word)) word++;/*skipping all other white spaces(if there are some)*/
                if(*word == '\0')
                {
                    add_error("missing parameters ",line, name);
                    continue;
                }
                i = 0;
                save = word;/*copping word into catch word aarray*/
                while(save[i] != '\0')
                {
                    catch_word_array[i] = save[i];
                    i++;
                }
                catch_word_array[i] = '\0';
                catch_word = catch_word_array;
                catch_word = strtok(catch_word, " ");/*getting first word after label*/
                if(!catch_word)/*if no word after label*/
                {
                    add_error("missing space ",line, name);
                    continue;
                }

                if(strcmp(catch_word, ".data") == 0)/*if its data line*/
                {
                    /*getting over the instruction word*/
                    while(isspace(*word)) word++;/*"    .data" ->-> ".data"*/
                    word = strchr(word, 'a');
                    word++;
                    word = strchr(word, 'a');
                    word++;
                    if(*word == '\0' || !isspace(*word))/*after .data there is no space or end line*/
                    {
                        add_error("missing space ",line ,name);
                        continue;
                    }
                    /*avoiding spaces*/
                    while(*word != '\0' && isspace(*word)) word++;
                    if(*word == '\0')
                    {
                        add_error("missing parameters ",line ,name);
                        continue;
                    }
                    /*start copping the arguments*/
                    start = word;
                    end = strchr(word, ',');
                    /*only one integer*/
                    if(end == NULL)
                    {
                        num = get_last_num(start,line,name);/*in here we insert the right errors if there are some*/
                        if(num == 1 << 11)
                        {
                            continue;
                        }
                        all_count++;
                        continue;
                    }
                    /*getting all of the integers*/
                    while(end != NULL)
                    {
                        num = get_num(start, end, line, name);/*in here we insert the right errors if there are some*/
                        if(num == 1 << 11)
                        {
                            break;
                        }
                        /*update all word counter*/
                        all_count++;
                        start = end+1;
                        end = strchr(start, ',');
                    }
                    /*getting last*/
                    num = get_last_num(start,line,name);/*in here we insert the right errors if there are some*/
                    if(num == 1 << 11)
                    {
                        continue;
                    }
                    /*update all word counter*/
                    all_count++;
                }
                else if(strcmp(catch_word, ".string") == 0)/*if its string line*/
                {
                    /*getting over the instruction word*/
                    word = strchr(word, 'g');
                    word++;
                    if(*word == '\0' || !isspace(*word))/*after .data there is no space or end line*/
                    {
                        add_error("missing space ",line ,name);
                        continue;
                    }
                    /*avoiding spaces*/
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
                    /*if there are some '"' in the word I get the last of the line*/
                    start++;
                    end = start;
                    while(strchr(end+1, '"') != NULL)
                    {
                        end = strchr(end+1, '"');
                    }
                    word = (end+1);
                    /*checking word after string*/
                    while(*word != '\0' && isspace(*word)) word++;
                    if(*word != '\0')
                    {
                        add_error("illegal text after end of string ",line, name);
                        continue;
                    }
                    str_ascii = get_str(start, end, line, name);/*in here we insert the right errors if there are some*/
                    save_str = str_ascii;
                    if(str_ascii == NULL)
                    {
                        continue;
                    }
                    while(*str_ascii != 0)
                    {
                        /*add it into "dc" structure*/
                        all_count++;
                        str_ascii++;
                    }
                    /*add 0 to "dc" to end the string*/
                    all_count++;
                    free(save_str);/*freeing memory*/
                }
                else if(strcmp(catch_word, ".mat") == 0)/*need to get the size of the mat and after that its simple*/
                {
                    /*getting over the instruction word*/
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
                    end = strchr(word, ']');/*searching for ']'*/
                    if(!end)
                    {
                        add_error("missing ']' ",line ,name);
                        continue;
                    }
                    end = strchr((end+1),']');/*searching for second ']'*/
                    if(!end)
                    {
                        add_error("missing ']' ",line ,name);
                        continue;
                    }
                    end++;
                    while(*end != '\0' && isspace(*end)) end++;/*skipping all white chars*/
                    if(*end == '\0')
                    {
                        empty_mat = 1;
                    }
                    if((node = get_mat(word, line, name, empty_mat)) == NULL)/*in here we insert the right errors if there are some*/
                    {
                        continue;
                    }
                    /*adding the nums I collect to the "dc" structure*/
                    for(i = 0; i< node->row;i++)
                    {
                        for(j = 0; j< node->colom;j++)
                        {
                            all_count++;
                        }
                    }
                    free_mat_list(&node);/*freeing memory*/
                    node = NULL;
                }
                else if(strcmp(catch_word, ".entry") == 0)
                {
                    /*getting over the instruction word*/
                    while(*word != '\0' && !isspace(*word)) word++;
                    if(*word == '\0')
                    {
                        add_error("missing space or arguments ",line ,name);
                        continue;
                    }
                    while(*word != '\0' && isspace(*word)) word++;/*skipping all white chars*/
                    if(*word == '\0')
                    {
                        add_error("missing arguments ",line, name);
                        continue;
                    }
                    /*creating entry file*/
                    if(ent == NULL)
                    {
                        ent_name = (char*)malloc(sizeof(char)*strlen(name)+2);/*name.am ->-> name.en*t*  thats why +2*/
                        strcpy(ent_name, name);
                        temp = strchr(ent_name, '.');
                        *temp  = '\0';
                        strcat(ent_name, ".ent");
                        ent = fopen(ent_name, "w");
                        if(!ent)
                        {
                            fprintf(stdout,"failed open file");
                            exit(1);
                        }
                        free(ent_name);
                    }
                    continue;
                }
                else if(strcmp(catch_word, ".extern") == 0)
                {
                    /*getting over the instruction word*/
                    while(*word != '\0' && !isspace(*word)) word++;
                    if(*word == '\0')
                    {
                        add_error("missing space or arguments ",line ,name);
                        continue;
                    }
                    while(*word != '\0' && isspace(*word)) word++;/*skipping all white chars*/
                    if(*word == '\0')
                    {
                        add_error("missing arguments ",line, name);
                        continue;
                    }
                    /*creating extern file*/
                    if(ext == NULL)
                    {
                        ext_name = (char*)malloc(sizeof(char)*strlen(name)+2);/*name.am ->-> name.ext  thats why +2*/
                        strcpy(ext_name, name);
                        temp = strchr(ext_name, '.');
                        *temp  = '\0';
                        strcat(ext_name, ".ext");
                        ext = fopen(ext_name, "w");
                        if(!ent)
                        {
                            fprintf(stdout,"failed open file");
                            exit(1);
                        }
                        free(ext_name);
                    }
                    continue;
                }
                else
                {
                    if((operation_value = check_if_operation_in_language(catch_word, operation_list)) != -1)/*checking if the word is a saved word in the language*/
                    {
                        if(operation_value < 14)
                        {
                            while(*word != '\0' && !isspace(*word)) word++;/*skipping the command*/
                            if(*word == '\0')
                            {
                                add_error("missing parameters ",line, name);
                                continue;
                            }
                            while(*word != '\0' && isspace(*word)) word++;/*skipping white chars after command*/
                            if(*word == '\0')
                            {
                                add_error("missing parameters ",line, name);
                                continue;
                            }
                        }
                        else
                        {
                            while(*word != '\0' && !isspace(*word)) word++;/*skipping the command*/
                            if(*word != '\0')
                            {
                                while(*word != '\0' && isspace(*word)) word++;/*skipping white chars after command*/
                                if(*word != '\0')
                                {
                                    add_error("illegal chars after command ",line, name);
                                    continue;
                                }
                            }
                        }
                        /*add it to "ic" structure*/
                        switch((opcode)operation_value)
                        {
                        case mov:
                        case add:
                        case sub:
                            by_of_word = code_for_zero(word, line, name, operation_value, data_collector->label, all_count);
                            break;
                        case cmp:
                            by_of_word = code_for_one(word, line, name, operation_value, data_collector->label, all_count);
                            break;
                        case lea:
                            by_of_word = code_for_four(word, line, name, operation_value, data_collector->label, all_count);
                            break;
                        case clr:
                        case not:
                        case inc:
                        case dec:
                        case jmp:
                        case bne:
                        case jsr:
                        case red:
                            by_of_word = code_for_five(word, line, name, operation_value, data_collector->label, all_count);
                            break;
                        case prn:
                            by_of_word = code_for_thirteen(word, line, name, operation_value, data_collector->label, all_count);
                            break;
                        case rts:
                        case stop:
                            by_of_word = code_for_fourteen(operation_value);
                            break;
                        }
                        if(!by_of_word)/*if word is NULL*/
                        {
                            free_binary_list(&by_of_word);/*free memory*/
                            by_of_word = NULL;
                            continue;
                        }
                        temp_by_code = by_of_word;
                        while(temp_by_code != NULL)
                        {
                            add_ic(&curr, temp_by_code->word, all_count);/*add into ic struct*/
                            temp_by_code = temp_by_code->next;
                            all_count++;
                        }
                        free_binary_list(&by_of_word);/*free memory*/
                        by_of_word = NULL;
                        continue;
                    }
                }
            }
        }
        else
        {
            /*getting first word*/
            strcpy(catch_word_array, word);
            catch_word = catch_word_array;
            catch_word = strtok(catch_word, " ");
            if(!catch_word)
            {
                add_error("missing space ",line, name);
                continue;
            }

            if(strcmp(catch_word, ".data") == 0)/*if its .data*/
            {
                /*getting over the instruction word*/
                while(isspace(*word)) word++;/*"    .data" ->-> ".data"*/
                word = strchr(word, 'a');
                word++;
                word = strchr(word, 'a');
                word++;
                /*checking first char after word*/
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
                /*start getting the numbers*/
                start = word;
                end = strchr(word, ',');
                if(end == NULL)/*only one number or none*/
                {
                    num = get_last_num(start,line,name);/*all errors of last num will be added if needed*/
                    if(num == 1 << 11)
                    {
                        continue;
                    }
                    all_count++;
                    continue;
                }
                while(end != NULL)/*at least two nums(should be)*/
                {
                    num = get_num(start, end, line, name);
                    if(num == 1 << 11)
                    {
                        break;
                    }
                    /*updating all_count var*/
                    all_count++;
                    start = end+1;
                    end = strchr(start, ',');
                }
                num = get_last_num(start,line,name);/*getting last number*/
                if(num == 1 << 11)
                {
                    continue;
                }
                /*add it into "dc" structure*/
                all_count++;
            }
            else if(strcmp(catch_word, ".string") == 0)/*if its .string*/
            {
                /*getting over the word*/
                word = strchr(word, 'g');
                word++;
                /*checking first char after word*/
                if(*word == '\0' || !isspace(*word))
                {
                    add_error("missing space ",line ,name);
                    continue;
                }
                while(*word != '\0' && isspace(*word)) word++;/*skipping spaces*/
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
                while(strchr(end+1, '"') != NULL)/*getting the last '"'*/
                {
                    end = strchr(end+1, '"');
                }
                word = (end+1);
                while(*word != '\0' && isspace(*word)) word++;/*checking end of word*/
                if(*word != '\0')
                {
                    add_error("illegal text after end of string ",line, name);
                    continue;
                }
                str_ascii = get_str(start, end, line, name);/*get the string*/
                save_str = str_ascii;
                if(str_ascii == NULL)
                {
                    continue;
                }
                while(*str_ascii != 0)
                {
                    /*update all_count var*/
                    all_count++;
                    str_ascii++;
                }
                /*count also the '\0'*/
                all_count++;
                free(save_str);/*free memory*/
            }
            else if(strcmp(catch_word, ".mat") == 0)/*if its .mat*/
            {
                /*skipping first word*/
                while(isspace(*word)) word++;/*example: "    .data" ->-> ".data"*/
                word = strchr(word, 't');
                word++;
                /*checking first char after wword*/
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
                /*checking basic validation of matrix*/
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
                while(*end != '\0' && isspace(*end)) end++;/*skipping spaces*/
                if(*end == '\0')
                {
                    empty_mat = 1;
                }
                if((node = get_mat(word, line, name, empty_mat)) == NULL)/*getting the mat if NULL than continue to next line*/
                {
                    continue;
                }
                /*all_count++ for each variable*/
                for(i = 0; i< node->row;i++)
                {
                    for(j = 0; j< node->colom;j++)
                    {
                        all_count++;
                    }
                }
                free_mat_list(&node);/*free memory*/
                node = NULL;
            }
            else if(strcmp(catch_word, ".entry") == 0)/*if its .entry*/
            {
                /*skipping first word*/
                while(*word != '\0' && !isspace(*word)) word++;
                if(*word == '\0')
                {
                    add_error("missing space or arguments ",line ,name);
                    continue;
                }
                while(*word != '\0' && isspace(*word)) word++;/*skipping spaces*/
                if(*word == '\0')
                {
                    add_error("missing arguments ",line, name);
                    continue;
                }
                /*if its first ent create new file*/
                if(ent == NULL)
                {
                    ent_name = (char*)malloc(sizeof(char)*(MAX_IN_LINE+1));/*name.am ->-> name.ent  thats why +2*/
                    if(!ent_name)
                    {
                        fprintf(stdout, "memory allocation fail\n");
                        exit(1);
                    }
                    /*getting file name*/
                    strcpy(ent_name, name);
                    temp = strchr(ent_name, '.');
                    if(temp)
                    {
                        *temp = '\0';
                    }
                    strcat(ent_name, ".ent");
                    ent = fopen(ent_name, "w");
                    if(!ent)
                    {
                        fprintf(stdout,"failed open file");
                        exit(1);
                    }
                    free(ent_name);
                }
                continue;
            }
            else if(strcmp(catch_word, ".extern") == 0)/*if its .extern*/
            {
                /*skipping first word*/
                while(*word != '\0' && !isspace(*word)) word++;
                if(*word == '\0')
                {
                    add_error("missing space or arguments ",line ,name);
                    continue;
                }
                while(*word != '\0' && isspace(*word)) word++;
                if(*word == '\0')
                {
                    add_error("missing arguments ",line, name);
                    continue;
                }
                /*if its first extern create new file*/
                if(ext == NULL)
                {
                    ext_name = (char*)malloc(sizeof(char)*strlen(name)+3);/*name.am ->-> name.ext  thats why +2*/
                    strcpy(ext_name, name);
                    temp = strchr(ext_name, '.');
                    *temp  = '\0';
                    strcat(ext_name, ".ext");
                    ext = fopen(ext_name, "w");
                    if(!ent)
                    {
                        fprintf(stdout,"failed open file");
                        exit(1);
                    }
                    free(ext_name);
                }
                continue;
            }
            else
            {
                /*getting instruction word*/
                strcpy(catch_word_array, word);
                catch_word = catch_word_array;
                strtok(catch_word, " ");

                if((operation_value = check_if_operation_in_language(catch_word, operation_list)) != -1)/*checking if the word is a saved word in the language*/
                {
                    if(operation_value < 14)
                    {
                        while(*word != '\0' && !isspace(*word)) word++;/*skipping the command*/
                        if(*word == '\0')
                        {
                            add_error("missing parameters ",line, name);
                            continue;
                        }
                        while(*word != '\0' && isspace(*word)) word++;/*skipping white chars after command*/
                        if(*word == '\0')
                        {
                            add_error("missing parameters ",line, name);
                            continue;
                        }
                    }
                    else
                    {
                        while(*word != '\0' && !isspace(*word)) word++;/*skipping the command*/
                        if(*word != '\0')
                        {
                            while(*word != '\0' && isspace(*word)) word++;/*skipping white chars after command*/
                            if(*word != '\0')
                            {
                                add_error("illegal chars after command ",line, name);
                                continue;
                            }
                        }
                    }
                    /*add it to "ic" structure*/
                    switch((opcode)operation_value)
                    {
                    case mov:
                    case add:
                    case sub:
                        by_of_word = code_for_zero(word, line, name, operation_value, data_collector->label, all_count);
                        break;
                    case cmp:
                        by_of_word = code_for_one(word, line, name, operation_value, data_collector->label, all_count);
                        break;
                    case lea:
                        by_of_word = code_for_four(word, line, name, operation_value, data_collector->label, all_count);
                        break;
                    case clr:
                    case not:
                    case inc:
                    case dec:
                    case jmp:
                    case bne:
                    case jsr:
                    case red:
                        by_of_word = code_for_five(word, line, name, operation_value, data_collector->label, all_count);
                        break;
                    case prn:
                        by_of_word = code_for_thirteen(word, line, name, operation_value, data_collector->label, all_count);
                        break;
                    case rts:
                    case stop:
                        by_of_word = code_for_fourteen(operation_value);
                        break;
                    }
                    if(!by_of_word)/*if word is NULL go to next line*/
                    {
                        by_of_word = NULL;
                        continue;
                    }
                    /*adding by_code to ic structure*/
                    temp_by_code = by_of_word;
                    while(temp_by_code != NULL)
                    {
                        add_ic(&curr, temp_by_code->word, all_count);
                        temp_by_code = temp_by_code->next;
                        all_count++;
                    }
                    free_binary_list(&by_of_word);/*free memory*/
                    by_of_word = NULL;
                    continue;
                }
            }
        }
    }
    if(sy_name)
    {
        free(p_sy_name);/*free memory*/
    }
    /*initialized result*/
    result->dc_values = data_collector->data_code;
    result->ic_value = curr;
    result->DC = data_collector->dc_count;
    result->IC = data_collector->ic_count;
    return result;
}


