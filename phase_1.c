/*in this file we are gonna read the file line by line and make for it binary code for our known words. in phase 2 we will complete the process and convert this binary into "abcd"*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_asembler.h"
#define MAX_LABEL_LEN 29

/*this struct gonna save all of the labels that we encounter until now(phase 1)*/
typedef struct labels
{
    unsigned short adress;
    char* label_name;
    char* guiding_name;/*data\string\mat\entery\extern*/
    struct labels* next;
}labels;

/*a linked list that are used to save the binary code of one line*/
typedef struct binary
{
    unsigned short b_word;
    struct binary* next;
}binary;
/*this struct used to save all of the IC binary code(that we know in phase 1) and count it's lines*/
typedef struct ic
{
    int line;
    binary* code;
    struct ic* next;
}ic_code;
/*this struct used to save all of the DC binary code(that we know in phase 1) and count it's lines*/
typedef struct dc
{
    int dc_line;
    binary* code;
    struct dc* next;
}dc_code;

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

    new_label->adress = address;

    new_label->label_name = (char*)malloc(strlen(name) + 1);
    if (!new_label->label_name)
    {
        free(new_label);
        fprintf(stderr, "Error: Memory allocation for label name failed.\n");
        return 0;
    }
    strcpy(new_label->label_name, name);

    new_label->guiding_name = (char*)malloc(strlen(guiding_name) + 1);
    if (!new_label->guiding_name)
    {
        free(new_label->label_name);
        free(new_label);
        fprintf(stderr, "Error: Memory allocation for guiding name failed.\n");
        return 0;
    }
    strcpy(new_label->guiding_name, guiding_name);

    new_label->next = *head;
    *head = new_label;

    return 1; /*return "true"*/
}

/*returns 1 if 'name' is a valid name of operation and returns -1 if 'name' is not a valid name of oparation*/
int check_if_operation_in_language(const char* name)
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
int check_label_validation(char* name, int line, char* name, labels* sy_tabel)
{
    char* save;
    while(isspace(*name)) name++;
    name = strtok(name, ":");
    if(!isalpha(*name))
    {
        fprintf(stdout, "invalid label name(first letter is not an alphabet char) (line %d) in file %s", line, name);
        return 0;
    }
    if(strlen(name) > MAX_LABEL_LEN)
    {
        fprintf(stdout, "label name is too long. (line %d) in file %s", line, name);
        return 0;
    }
    save = name;
    while(*save == '\0' && !isspace(*save)) save++;/*found spaces if there are some*/
    if(*save != '\0')
    {
        fprintf(stdout, "label name has illegal spaces in it. (line %d) in file %s", line, name);
        return 0;
    }
    if(find_label(name) != NULL)
    {
        fprintf(stdout, "label is already exist. (line %d) in file %s", line, name);
        return 0;
    }
    if(find_node(root, name) != NULL)
    {
        fprintf(stdout, "label name is mcro name. (line %d) in file %s", line, name);
        return 0;
    }
    if(check_if_operation_in_language(name))
    {
        fprintf(stdout, "label name is operation name. (line %d) in file %s", line, name);
        return 0;
    }
    return 1;/*true*/
}
/*gets a string that should be a number(or just regular string and convert it back into a number*/
int get_num(char* num, int line, char* name)
{
    char* save;
    int sign = 1, number = 0;
    while(isspace(*num)) num++;
    num = strtok(num, " ");
    if(*num == '-' && strlen(num) >= 2)
    {
        sign = -1;
        num++;
    }
    else if(*num == '+' && strlen(num) >= 2)
    {
        num++;
    }
    else if(strlen(num) < 2 && (*num == '+' || *num == '-'))
    {
        fprintf(stdout, "not enough chars to complete a number (line %d) in file %s", line, name);
        return MAX;
    }
    else if(!isdigit(*num) && !(*num == '+' || *num == '-'))
    {
        fprintf(stdout, "illegal text before number (line %d) in file %s", line, name);
        return 0;
    }
    save = num;
    while(*save != '\0' && isdigit(*save)) save++;
    if(*save != '\0')
    {
        fprintf(stdout, "illegal text in the number (line %d) in file %s", line, name);
        return 0;
    }
    number = atoi(num);
    if(*num != '"')
        {
            fpritf(stdout, "")
        }
    return number*sign;
}


/*convert line into a machine code and adding it into the right structure*/
void read_file1(FILE* asembly, char* name)
{
    labels* sy_tabel;
    int line = 0, len = -1, kosher = 1, num = 0;/*assuming that the file is good until proven wrong*/
    char buffer[MAX_IN_LINE+1], *word, *sy_name;

    while(fgets(buffer, MAX_IN_LINE, file_asm) != NULL)
    {
        line++;
        word = buffer;
        while(isspace(*word)) word++;
        if((len = strcspn(word, ":")) > MAX_LABEL_LEN && word[len] != '\0')/*found ":" and it was on index bigger than 30*/
        {
            kosher = 0;
            fprintf(stdout, "label is too long (line %d) in file %s", line, name);
            continue;
        }
        if((len = strcspn(word, ":")) <= MAX_LABEL_LEN && word[len] != '\0')/*found a label*/
        {
            if(check_label_validation(word, line, sy_table))
            {
                sy_name = word;
                sy_name = strtok(sy_name, ":");
                sy_name--;

                while(*word != ':') word++;

                word++;
                if(!isspace(*word))
                {
                    kosher = 0;
                    fprintf(stdout, "missing space after \":\" (line %d) in file %s", line, name)
                    continue;
                }
                while(isspace(*word)) word++;/*skipping all other white spaces(if there are some)*/
                word = strtok(word, " ");
                if(strcmp(word, ".data") == 0)
                {
                    while(word != NULL)
                    {
                        word = strtok(NULL, ",");/*.data ,  3 33, =>(illegal text before number\illegal text in number)*/
                        num = get_num(word, line, name, 1);
                        /*here I need to add it into "dc" structure*/

                    }
                    add_label(&sy_tabel, line, sy_name, ".data");
                }
                else if(strcmp(word, ".string") == 0)
                {
                    while(word != NULL)
                    {
                        word = strtok(NULL, ",");/*.data ,  3 33, =>(illegal text before number\illegal text in number)*/
                        num = get_num(word, line, name, 0);
                        /*here I need to add it into "dc" structure*/
                    }
                    add_label(&sy_tabel, line, sy_name, ".string");
                }
                else if(strcmp(word, ".mat") == 0)
                {
                    while(word != NULL)
                    {
                        word = strtok(NULL, ",");/*.data ,  3 33, =>(illegal text before number\illegal text in number)*/
                        num = get_num(word, line, name, 1);
                        /*here I need to add it into "dc" structure*/
                    }
                    add_label(&sy_tabel, line, sy_name, ".mat");
                }
                else
                {

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
