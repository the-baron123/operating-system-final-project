/*in this file we are gonna read the file line by line and make for it binary code for our known words. in phase 2 we will complete the process and convert this binary into "abcd"*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_asembler.h"
#include "inputs_1.h"


/*convert line into a machine code and adding it into the right structure*/
void read_file1(FILE* file_asm, char* name)
{
    /*start creating vars for the function*/
    ic_code* operation_code_table;
    dc_code* data_code_table;
    labels* sy_table;
    mat *mats_tabel, *node;
    int line = 0, len = -1, num = 0, row = 0, colom = 0, operation_value = -1, kosher = 1;/*assuming that the file is good until proven wrong*/
    int *str_ascii, **mat_nums;
    int IC = 100, DC = 0, i = 0, j = 0;
    binary* by_of_word = NULL, *temp_by_code;
    char buffer[MAX_IN_LINE+1], *word, *sy_name;

    /*creating an array of all the operation in the language*/
    char *operation_list[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

    /*allocating memory to all of my lists*/
    operation_code_table = (ic_code*)malloc(sizeof(ic_code));
    if (!operation_code_table)
    {
        fprintf(stdout, "Memory allocation failed for operation_code_table\n");
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
    /*start reading from the file*/
    while(fgets(buffer, MAX_IN_LINE, file_asm) != NULL)
    {
        line++;
        word = buffer;
        while(isspace(*word)) word++;
        if((len = strcspn(word, ":")) > MAX_LABEL_LEN && word[len] != '\0')/*found ":" and it was on index bigger than 30*/
        {
            kosher = 0;
            fprintf(stdout, "label is too long (line %d) in file %s\n", line, name);
            continue;
        }
        if((len = strcspn(word, ":")) <= MAX_LABEL_LEN && word[len] != '\0')/*found a label*/
        {
            if(check_label_validation(word, line, name, sy_table))
            {
                sy_name = word;
                sy_name = strtok(sy_name, ":");

                while(*word != ':') word++;

                word++;
                if(!isspace(*word))
                {
                    kosher = 0;
                    fprintf(stdout, "missing space after \":\" (line %d) in file %s\n", line, name);
                    continue;
                }
                while(isspace(*word)) word++;/*skipping all other white spaces(if there are some)*/
                word = strtok(word, " ");
                if(strcmp(word, ".data") == 0)
                {
                    if(add_label(&sy_table, DC, sy_name, ".data") == 0) continue;/*cheeking if label added correctly*/
                    while(word != NULL)
                    {
                        word = strtok(NULL, ",");/*.data ,  3 33, =>(illegal text before number\illegal text in number)*/
                        num = get_num(word, line, name);
                        if(num == 1 << 11) continue;
                        /*add it into "dc" structure*/
                        add_dc(&data_code_table, num, DC);
                        DC++;
                    }
                }
                else if(strcmp(word, ".string") == 0)
                {
                    if(add_label(&sy_table, DC, sy_name, ".data") == 0) continue;
                    str_ascii = get_str(word, line, name);
                    if(str_ascii == NULL) continue;
                    while(*str_ascii != 0)
                    {
                        /*add it into "dc" structure*/
                        add_dc(&data_code_table, *str_ascii, DC);
                        DC++;
                        str_ascii++;
                    }
                    /*add 0 to "dc" to end the string*/
                    add_dc(&data_code_table, 0, DC);
                }
                else if(strcmp(word, ".mat") == 0)/*need to get the size of the mat and after that its simple*/
                {
                    if(add_label(&sy_table, line, sy_name, ".data") == 0) continue;
                    if((node = get_mat(word, line, name)) == NULL) continue;
                    if(add_mat(&mats_tabel, node, line, name) == 0) continue;
                    /*here I need to add it into "dc" structure*/
                    node = mats_tabel;
                    while(node->next != NULL)
                    {
                        node = node->next;
                    }
                    mat_nums = node->nums;
                    /*adding the nums I collect to the "dc" structure*/
                    for(i = 0; i< node->row;i++)
                    {
                        for(j = 0; j< node->colom;j++)
                        {
                            add_dc(&data_code_table, node->nums[i][j], DC);
                            DC++;
                        }
                    }
                }
                else if(strcmp(word, ".extern") == 0)
                {
                    if(add_label(&sy_table, 0, sy_name, ".extern") == 0) continue;
                }
                else
                {
                    while(isspace(*word))/*ignor the white chars*/
                    {
                        word++;
                        if(*word == '\0')/*label is empty*/
                        {
                            fprintf(stdout, "label is empty (line %d) in file %s.\n",line, name);
                            continue;
                        }
                    }
                    if(isdigit(*word))/*first letter is an integer*/
                    {
                        fprintf(stdout, "command name cannot start with a number (line %d) in file %s.\n",line, name);
                        continue;
                    }
                    /*our first word after label without spaces before it*/
                    if((operation_value = check_if_operation_in_language(word, line, name, operation_list)) != -1)/*checking if the word is a saved word in the language*/
                    {
                        if(add_label(&sy_table, IC, sy_name, ".code") == 0) continue;
                        /*need to add it to "ic" structure*/
                        switch(operation_value)
                        {
                        case 0:
                        case 2:
                        case 3:
                            by_of_word = code_for_zero(word,  line, name, operation_value);
                            break;
                        case 1:
                            by_of_word = code_for_one(word,  line, name, operation_value);
                            break;
                        case 4:
                            by_of_word = code_for_four(word,  line, name, operation_value);
                            break;
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                            by_of_word = code_for_five(word,  line, name, operation_value);
                            break;
                        case 13:
                            by_of_word = code_for_thirteen(word, line, name, operation_value, mats_tabel);
                            break;
                        case 14:
                        case 15:
                            /*only on this I can make a quick check that the line is valid I only need to check if there are more chars on the line*/
                            if(strtok(NULL, " ") != NULL)
                            {
                                fprintf(stdout, "illegal text after command (line %d) in file %s.\n",line, name);
                                continue;
                            }
                            by_of_word = code_for_fourteen(word, line, name, operation_value);
                            break;
                        }
                        temp_by_code = by_of_word;
                        while(temp_by_code != NULL)
                        {
                            add_ic(&operation_code_table, temp_by_code->word, IC);
                            IC++;
                            temp_by_code = temp_by_code->next;
                        }
                    }
                    else
                    {
                        fprintf(stdout, "%s does not exist in language (line %d) in file %s.\n", word, line, name);
                        continue;
                    }
                }
            }
            else
            {
                kosher = 0;
                continue;
            }
        }
        else
        {

            if(strcmp(word, ".data") == 0)
            {
                while(word != NULL)
                {
                    word = strtok(NULL, ",");/*.data ,  3 33, =>(illegal text before number\illegal text in number)*/
                    num = get_num(word, line, name);
                    if(num == 1 << 11) continue;
                    /*add it into "dc" structure*/
                    add_dc(&data_code_table, num, DC);
                    DC++;
                }
            }
            else if(strcmp(word, ".string") == 0)
            {
                str_ascii = get_str(word, line, name);
                if(str_ascii == NULL) continue;
                while(*str_ascii != 0)
                {
                    /*add it into "dc" structure*/
                    add_dc(&data_code_table, *str_ascii, DC);
                    DC++;
                    str_ascii++;
                }
                /*add 0 to "dc" to end the string*/
                add_dc(&data_code_table, 0, DC);
            }
            else if(strcmp(word, ".mat") == 0)/*need to get the size of the mat and after that its simple*/
            {
                if((node = get_mat(word, line, name)) == NULL) continue;
                if(add_mat(&mats_tabel, node, line, name) == 0) continue;
                /*here I need to add it into "dc" structure*/
                node = mats_tabel;
                while(node->next != NULL)
                {
                    node = node->next;
                }
                mat_nums = node->nums;
                /*adding the nums I collect to the "dc" structure*/
                for(i = 0; i< node->row;i++)
                {
                    for(j = 0; j< node->colom;j++)
                    {
                        add_dc(&data_code_table, node->nums[i][j], DC);
                        DC++;
                    }
                }
            }
            else
            {
                while(isspace(*word))/*ignor the white chars*/
                {
                    word++;
                    if(*word == '\0')/*label is empty*/
                    {
                        fprintf(stdout, "label is empty (line %d) in file %s.\n",line, name);
                        continue;
                    }
                }
                if(isdigit(*word))/*first letter is an integer*/
                {
                    fprintf(stdout, "command name cannot start with a number (line %d) in file %s.\n",line, name);
                    continue;
                }
                /*our first word after label without spaces before it*/
                if((operation_value = check_if_operation_in_language(word, line, name, operation_list)) != -1)/*checking if the word is a saved word in the language*/
                {
                    /*need to add it to "ic" structure*/
                    switch(operation_value)
                    {
                        case 0:
                        case 2:
                        case 3:
                            by_of_word = code_for_zero(word,  line, name, operation_value);
                            break;
                        case 1:
                            by_of_word = code_for_one(word,  line, name, operation_value);
                            break;
                        case 4:
                            by_of_word = code_for_four(word,  line, name, operation_value);
                            break;
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                            by_of_word = code_for_five(word,  line, name, operation_value);
                            break;
                        case 13:
                            by_of_word = code_for_thirteen(word, line, name, operation_value, mats_tabel);
                            break;
                        case 14:
                        case 15:
                            /*only on this I can make a quick check that the line is valid I only need to check if there are more chars on the line*/
                            if(strtok(NULL, " ") != NULL)
                            {
                                fprintf(stdout, "illegal text after command (line %d) in file %s.\n",line, name);
                                continue;
                            }
                            by_of_word = code_for_fourteen(word, line, name, operation_value);
                            break;
                    }
                    temp_by_code = by_of_word;
                    while(temp_by_code != NULL)
                    {
                        add_ic(&operation_code_table, temp_by_code->word, IC);
                        IC++;
                        temp_by_code = temp_by_code->next;
                    }
                }
                else
                {
                    fprintf(stdout, "%s does not exist in language (line %d) in file %s.\n", word, line, name);
                    continue;
                }
            }
        }
    }
}
