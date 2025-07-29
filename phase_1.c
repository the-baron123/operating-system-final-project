/*in this file we are gonna read the file line by line and make for it binary code for our known words. in phase 2 we will complete the process and convert this binary into "abcd"*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_asembler.h"
#include "inputs_1.h"

#define MAX_LABEL_LEN 29
#define WORD_LEN 10
sy_name
typedef enum { IC_TYPE, DC_TYPE } CodeType;

/*this struct gonna save all of the labels that we encounter until now(phase 1)*/
typedef struct labels
{
    unsigned short address;
    char* label_name;
    char* type;/*data\code*/
    struct labels* next;
}labels;

/*this struct used to save all of the IC binary code(that we know in phase 1) and count it's lines*/
typedef struct ic
{
    char* code_value;
    int addres;
    struct ic* next;
}ic_code;
/*this struct used to save all of the DC binary code(that we know in phase 1) and count it's lines*/
typedef struct dc
{
    char* code_value;
    int addres;
    struct dc* next;
}dc_code;

/*this function makes a binary code from an integer*/
char* num_to_by_code(int num)
{
    int i = 0, index = 0;
    char* binary = (char*)malloc(11* sizeof(char));/*10 bits+'\0'*/
    if (!binary) return NULL;

    if (num < 0)
    {
        num = ((1 << 10) + num);
    }

    for(int i = 9; i >= 0; i--)
    {
        binary[index] = ((num >> i) & 1)? '1' : '0';
        index++;
    }
    binary[10] = '\0';

    return binary;
}

int add_dc_or_ic(void** head, short dc_ic, int address, CodeType type)
{
    dc_code* new_node1, *temp1;
    ic_code* new_node2, *temp2;

    if(type == IC_TYPE)
    {
        new_node2 = (ic_code*)malloc(sizeof(ic_code));
        if (!new_node2)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        new_node2->code_value = num_to_by_code(dc_ic);
        new_node2->addres = address;
        new_node2->next = NULL;

        ic_code* temp2 = *(ic_code**)head;

        if (temp2 == NULL)/*empty list*/
        {
            *(ic_code**)head = new_node2;
        }
        else
        {
            while (temp2->next != NULL)
            {
                temp2 = temp2->next;
            }
            temp2->next = new_node2;
        }
    }
    else if(type == DC_TYPE)
    {
        new_node1 = (dc_code*)malloc(sizeof(dc_code));
        if (!new_node1)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        new_node1->code_value = num_to_by_code(dc_ic);
        new_node1->addres = address;
        new_node1->next = NULL;

        dc_code* temp1 = *(dc_code**)head;

        if (temp1 == NULL)/*empty list*/
        {
            *(dc_code**)head = new_node1;
        }
        else
        {
            while (temp1->next != NULL)
            {
                temp1 = temp1->next;
            }
            temp1->next = new_node1;
        }
    }
    else
    {
        fprintf(stdout, "illegal memory type entered!. exit program...\n");
        exit(1);
    }
    return 1;
}

int add_label(labels** head, unsigned short address, const char* name, const char* guiding_name)
{
    labels* current = *head;
    labels* new_label;
    /* checking if the label already existing*/
    while (current != NULL)
    {
        if (strcmp(current->label_name, name) == 0)
        {
            fprintf(stdout, "Error: Label \"%s\" already exists.\n", name);
            return 0;
        }
        current = current->next;
    }

    /* creating new label*/
    new_label = (labels*)malloc(sizeof(labels));
    if (!new_label)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }

    new_label->address = address;

    new_label->label_name = (char*)malloc((strlen(name) + 1)* sizeof(char));
    if (!new_label->label_name)
    {
        free(new_label);
        fprintf(stderr, "Error: Memory allocation for label name failed.\n");
        return 0;
    }
    strcpy(new_label->label_name, name);

    new_label->type = (char*)malloc((strlen(guiding_name) + 1)* sizeof(char));
    if (!new_label->type)
    {
        free(new_label->label_name);
        free(new_label);
        fprintf(stderr, "Error: Memory allocation for guiding name failed.\n");
        return 0;
    }
    strcpy(new_label->type, guiding_name);

    new_label->next = *head;
    *head = new_label;

    return 1; /*return "true"*/
}
/*returns 1 if 'name' is a valid name of operation and returns -1 if 'name' is not a valid name of oparation*/
int check_if_operation_in_language(const char* name, int line, char* file)
{
    /*creating an array of all the operation in the language*/
    char *oparations[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int i = 0;

    for(i = 0; i<16; i++)
    {
        if(strcmp(name, oparations[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}
/*checks label name validation according to the task*/
int check_label_validation(char* name, int line, char* file, labels* sy_tabel)
{
    char* save;
    while(isspace(*name)) name++;
    name = strtok(name, ":");
    if(!isalpha(*name))
    {
        fprintf(stdout, "invalid label name(first letter is not an alphabet char) (line %d) in file %s\n", line, file);
        return 0;
    }
    if(strlen(name) > MAX_LABEL_LEN)
    {
        fprintf(stdout, "label name is too long. (line %d) in file %s\n", line, file);
        return 0;
    }
    save = name;
    while (*save != '\0')
    {
        if (isspace(*save))
        {
            fprintf(stdout, "label name has illegal spaces in it. (line %d) in file %s\n", line, file);
            return 0;
        }
        save++;
    }/*found spaces if there are some and report it*/
    if(find_label(name) != NULL)
    {
        fprintf(stdout, "label is already exist. (line %d) in file %s\n", line, file);
        return 0;
    }
    if(find_node(root, name) != NULL)
    {
        fprintf(stdout, "label name is mcro name. (line %d) in file %s\n", line, file);
        return 0;
    }
    if(check_if_operation_in_language(name, line, file))
    {
        fprintf(stdout, "label name is operation name. (line %d) in file %s\n", line, file);
        return 0;
    }
    return 1;/*true*/
}

/*convert line into a machine code and adding it into the right structure*/
void read_file1(FILE* file_asm, char* name)
{
    /*start creating vars for the function*/
    ic_code* operation_code_table;
    dc_code* data_code_table;
    labels* sy_table;
    mat* mats_tabel, *node;
    int line = 0, len = -1, num = 0, *str_ascii, row = 0, colom = 0, **mat_nums, kosher = 1;/*assuming that the file is good until proven wrong*/
    int IC = 100, DC = 0;
    char buffer[MAX_IN_LINE+1], *word, *sy_name;
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
    sy_table = (sy_table*)malloc(sizeof(sy_table));
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
                        /*add it into "dc" structure*/
                        add_dc_or_ic((void**)&data_code_table, num, DC, DC_TYPE);
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
                        add_dc_or_ic((void**)&data_code_table, *str_ascii, DC, DC_TYPE);
                        DC++;
                        str_ascii++;
                    }
                    /*add 0 to "dc" to end the string*/
                    add_dc_or_ic((void**)&data_code_table, 0, DC, DC_TYPE);
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
                            add_dc_or_ic((void**)&data_code_table, node->nums[i][j], DC, DC_TYPE);
                            DC++;
                        }
                    }
                }
                else
                {
                    if(check_if_operation_in_language(word, line, name))
                    {
                        if(add_label(&sy_table, IC, sy_name, ".code") == 0) continue;
                        /*need to add it to "ic" structure*/
                    }
                    else
                    {
                        fprintf(stdout, "the command does not exist in language (line %d) in file %s.\n",line, file);
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
    }
}
