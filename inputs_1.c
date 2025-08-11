#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_LABEL_LEN 29
#define WORD_LEN 10
#define START_OPCODE 6
#define END_OPCODE 9
#define START_DEST_REGISTER 2
#define END_DEST_REGISTER 5
#define START_SOURCE_REGISTER 6
#define END_SOURCE_REGISTER 9

typedef struct mat
{
    int** nums;
    int row;
    int colom;
    struct mat* next;
}mat;

typedef struct binary
{
    short word;
    struct binary* next;
}binary;

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
    short code_value;
    int addres;
    struct ic* next;
}ic_code;
/*this struct used to save all of the DC binary code(that we know in phase 1) and count it's lines*/
typedef struct dc
{
    short code_value;
    int addres;
    struct dc* next;
}dc_code;

/*searching mat function*/
labels* find_label(labels* head, const char* target_name)
{
    while (head != NULL)
    {
        if (strcmp(head->label_name, target_name) == 0)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

/*this function enters bits to limited area*/
void insert_bits(short* word, int start_bit, int end_bit, int num_to_insert)
{
    int count = end_bit - start_bit + 1;/*number of bits to insert*/
    short mask = ((1 << count) - 1) << start_bit;

    *word &= ~mask; /*initialized the word(00...0)*/
    *word |= ((num_to_insert & ((1 << count) - 1)) << start_bit);/*entering the value*/
}
/*insert node into binary list*/
binary* add_binary_node(binary** head, short word_value)
{
    binary* new_node = (binary*)malloc(sizeof(binary));
    if (!new_node)
    {
        fprintf(stderr, "Error: memory allocation failed.\n");
        exit(1);
    }

    new_node->word = word_value;
    new_node->next = NULL;

    if (*head == NULL)/*empty list*/
    {
        *head = new_node;
    }
    else
    {
        binary* current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
    }

    return *head;
}
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
/*this function insert new node into dc structure*/
int add_dc(dc_code** head, short dc, int address)
{
    dc_code* new_node1, *temp1;

    new_node1 = (dc_code*)malloc(sizeof(dc_code));
    if (!new_node1)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    new_node1->code_value = num_to_by_code(dc);
    new_node1->addres = address;
    new_node1->next = NULL;
    temp1 = *head;

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

    return 1;
}
/*this function insert new node into ic structure*/
int add_ic(ic_code** head, short ic, int address)
{
    ic_code* new_node2, *temp2;

    new_node2 = (ic_code*)malloc(sizeof(ic_code));
    if (!new_node2)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    new_node2->code_value = num_to_by_code(ic);
    new_node2->addres = address;
    new_node2->next = NULL;
    temp2 = *head;
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

    return 1;
}

/*this function insert a new label into the linked list*/
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
        exit(1);
    }

    new_label->address = address;

    new_label->label_name = (char*)malloc((strlen(name) + 1)* sizeof(char));
    if (!new_label->label_name)
    {
        free(new_label);
        fprintf(stderr, "Error: Memory allocation for label name failed.\n");
        exit(1);
    }
    strcpy(new_label->label_name, name);

    new_label->type = (char*)malloc((strlen(guiding_name) + 1)* sizeof(char));
    if (!new_label->type)
    {
        free(new_label->label_name);
        free(new_label);
        fprintf(stderr, "Error: Memory allocation for guiding name failed.\n");
        exit(1);
    }
    strcpy(new_label->type, guiding_name);

    new_label->next = *head;
    *head = new_label;

    return 1; /*return "true"*/
}
/*
this function checks if some operation is in the language and return it's binary value
if the word does not exist return 0
*/
int check_if_operation_in_language(const char* name, int line, char* file, char** operation_list)
{
    int i = 0;

    for(i = 0; i<16; i++)
    {
        if(strcmp(name, operation_list[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}
/*checks label name validation according to the task*/
int check_label_validation(char* name, int line, char* file, labels* sy_tabel)
{
    /*creating an array of all the operation in the language*/
    char *operation_list[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
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
        if(!isdigit(*save) && !isalpha(*save))
        {
            fprintf(stdout, "label name has illegal char in it. (line %d) in file %s\n", line, file);
            return 0;
        }
        save++;
    }/*found spaces if there are some and report it*/
    if(find_label(name) != NULL)
    {
        fprintf(stdout, "label is already exist. (line %d) in file %s\n", line, file);
        return 0;
    }
    if(find_node(sy_tabel, name) != NULL)
    {
        fprintf(stdout, "label name is mcro name. (line %d) in file %s\n", line, file);
        return 0;
    }
    if(check_if_operation_in_language(name, line, file, operation_list))
    {
        fprintf(stdout, "label name is operation name. (line %d) in file %s\n", line, file);
        return 0;
    }
    return 1;/*true*/
}
/*function to add a node into the mat linked list*/
int add_mat(mat** head, mat* node_add, int line, char* file)
{
    mat* current;

    if (head == NULL || node_add == NULL)/*if the node to add or the pointer to the list is empty*/
    {
        return 0;
    }

    /*if the list is empty*/
    if (*head == NULL) {
        *head = node_add;
        node_add->next = NULL;
        return 0;
    }

    current = *head;

    /*going over the list*/
    while (current->next != NULL) {
        current = current->next;
    }
    /*linking the node to the list*/
    current->next = node_add;
    node_add->next = NULL;

    return 1;/*returning head of the list*/
}
/*gets a string that should be a number(or just regular string and convert it back into a number*/
int get_num(char* num, int line, char* name)
{
    char* save;
    int number = 0;

    while(isspace(*num)) num++;/*removing white chars*/
    num = strtok(num, " ");/*getting the number*/
    save = num;
    while(!isspace(*save) && *save != ',' && *save != '\n')/*searching for illegal chars in number*/
    {
        if(!isdigit(*save) && *save != '-' && *save != '+')
        {
            fprintf(stdout, "illegal text in number (line %d) in file %s.\n",line, name);
            return 1 << 11;
        }
        save++;
    }
    if(*save != ',' && strtok(NULL, ",") == NULL)/*searching for illegal chars after num*/
    {
        while(*save != '\0')
        {
            if(!isspace(*save) && *save != '\n')
            {
                fprintf(stdout, "missing comma\illegal char after number (line %d) in file %s.\n",line, name);
                return 1 << 11;
            }
            save++;
        }
    }
    number = atoi(num);/*getting the number with atoi*/
    if(number > 1 << 8)
    {
        fprintf(stdout, "number is too big (line %d) in file %s.\n",line, name);
        return 1 << 11;
    }
    return number;
}
/*
this function gets a string and convert it into an array of integers
that each index in it is the binary value of the char in the index space on the string
*/
int* get_str(char* str, int line, char* name)
{
    int *res, *p_res;

    while(isspace(*str)) str++;/*skipping all white chars from the start just to be sure*/
    if(*str != '"')
    {
        fprintf(stdout, "string missing \" (line %d) in file %s\n", line, name);
        return NULL;
    }
    str = strtok(str, "\"");/*getting the string*/
    if(str == NULL)
    {
        fprintf(stdout, "string missing \" (line %d) in file %s\n", line, name);
        return NULL;
    }
    res = (int*)malloc((strlen(str)+1)* sizeof(int));/*allocating memory for string length + '\0'*/
    p_res = res;/*setting a pointer that I can move without worrying to lose my position */
    while(*str != '\0')
    {
        if(!isprint(*str))/*checking if the char is printable*/
        {
            free(res);
            fprintf(stdout, "string contains a non printable char (line %d) in file %s\n", line, name);
            return NULL;
        }
        /*setting the array*/
        *p_res = (int)*str;
        p_res++;
        str++;
    }
    *p_res = 0;
    if(strtok(str, " \n") != NULL)
    {
        free(res);
        fprintf(stdout, "illegal char after string(should be no chars at all or only white char) (line %d) in file %s\n", line, name);
        return NULL;
    }
    return res;
}
/*this function gets the first integer from the start of the word*/
int get_integer_mat(char* mat, int line, char* name)
{
    int res = -1, count_num = 0;
    char* integer, *save;

    while(isspace(*mat) && *mat != '\0') mat++;/*ignoring white chars*/
    if(*mat == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        return res;
    }
    if(*mat == ']')
    {
        fprintf(stdout, "missing integer (line %d), in file %s\n", line, name);
        return res;
    }
    if(!isdigit(*mat))
    {
        fprintf(stdout, "illegal char(should be an integer) (line %d), in file %s\n", line, name);
        return res;
    }
    /*getting the number*/
    save = mat;
    while(isdigit(*save))
    {
        count_num++;
        save++;
    }
    if(*save == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        return res;
    }
    if(isalpha(*save))
    {
        fprintf(stdout, "illegal char (line %d), in file %s\n", line, name);
        return res;
    }
    integer = (char*)malloc((count_num+1)* sizeof(char));
    if(integer == NULL)
    {
        fprintf(stdout, "allocating memory error\n", line, name);
        exit(1);
    }
    while(isdigit(*mat))
    {
        *integer = *mat;
        integer++;
        mat++;
    }
    res = atoi(integer);

    while(isspace(*mat)) mat++;
    if(*mat == ']')
    {
        return res;/*returning the number*/
    }
    if(*save == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        return -1;
    }
    if(isalpha(*save))
    {
        fprintf(stdout, "illegal char (line %d), in file %s\n", line, name);
        return -1;
    }
    fprintf(stdout, "missing parameters\illegal char (line %d), in file %s\n", line, name);
    return -1;
}
/*
this function gets a string that should be a define of a matrix
and gets the matrix and it's values
 */
mat* get_mat(char* mat_info, int line, char* name)
{
    mat* new_mat;
    char* save, *mat_name;
    int count_mat = 0, row = 0, colom = 0, **nums, i, j, integer;

    new_mat = (mat*)malloc(sizeof(mat));
    if (new_mat == NULL)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    if(isspace(*mat_info))
    {
        fprintf(stdout, "illegal white char (line %d), in file %s\n", line, name);
        return NULL;
    }
    /*getting the size of the matrix*/
    row = get_integer_mat(mat_info, line, name);
    if(row == -1)
    {
        /*the error were print in the function*/
        free(mat_name);
        return NULL;
    }
    while(*mat_info != ']')/*we checked in the function that that char exist*/
    {
        mat_info++;
    }
    mat_info++;
    if(*mat_info == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        free(mat_name);
        return NULL;
    }
    if(isspace(*save))
    {
        fprintf(stdout, "illegal white char (line %d), in file %s\n", line, name);
        free(mat_name);
        return NULL;
    }
    if(*save != '[')
    {
        fprintf(stdout, "illegal char after mat name(line %d), in file %s\n", line, name);
        free(mat_name);
        return NULL;
    }
    colom = get_integer_mat(mat_info, line, name);
    if(colom == -1)
    {
        /*the error were print in the function*/
        free(mat_name);
        return NULL;
    }
    /*allocating memory for the array and setting it for 0*/
    nums = (int**)malloc(row* sizeof(int*));
    if (nums == NULL) {
        fprintf(stdout, "Memory allocation failed for rows.\n");
        free(mat_name);
        exit(1);
    }
    for(i = 0;i<row;i++)
    {
        nums[i] = (int*)calloc(colom, sizeof(int));
        if (nums[i] == NULL) {
            fprintf(stdout, "Memory allocation failed for rows.\n");
            for(j = 0;j<i;j++)
            {
                free(nums[j]);
            }
            free(mat_name);
            exit(1);
        }
    }
    /*getting the numbers of the mat*/
    for(i = 0;i<row;i++)
    {
        for(j = 0;j<colom;j++)
        {
            if((save = strtok(mat_info, ",")) != NULL)
            {
                integer = get_num(mat_info, line, name);
                if(integer == 1 << 11)
                {
                    for(i = 0;i<row;i++)
                    {
                        free(nums[i]);
                    }
                    free(nums);
                    free(mat_name);
                    return NULL;
                }
                else
                {
                    nums[i][j] = integer;
                }
            }
            else
            {
                integer = get_num(mat_info, line, name);
                if(integer == 1 << 11)
                {
                    for(i = 0;i<row;i++)
                    {
                        free(nums[i]);
                    }
                    free(nums);
                    free(mat_name);
                    return NULL;
                }
                new_mat->nums = nums;
                new_mat->colom = colom;
                new_mat->row = row;
                return new_mat;
            }
        }
    }
    if(save != NULL)
    {
        fprintf(stdout, "to much inputs (line %d) in file %s.\n", line, name);
        for(i = 0;i<row;i++)
        {
            free(nums[i]);
        }
        free(nums);
        free(mat_name);
        return NULL;
    }
    new_mat->nums = nums;
     new_mat->colom = colom;
     new_mat->row = row;
    return new_mat;
}
/*gets a string and checks if its a register and if it is return the number of the register*/
short check_if_register(char* word, int line, char* file_name)
{
    short res;

    while(*word != '\0' && isspace(*word)) word++;
    if(*word != 'r')
    {
        return -1;
    }
    word++;
    if(*word != '\0' && isdigit((*word)))
    {
        if((res = atoi(word)) > 7)
        {
            return -1;
        }
        word++;
        if(*word == '\0')
        {
            return res;
        }
    }
    return -1;
}
/*this function gets a word in the language and convert it to it's binary value and return it*/
/*source: 1,2,3 dest: 0,1,2,3*/
binary* code_for_zero(char* word, int line, char* file_name, int opcode)
{
    char* save_word;
    short by_word = 0, register_num;
    labels* temp_label;
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;

    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);

    /*start getting the source*/
    if((strtok(word," ") != NULL) && temp_label = find_label(labels_table, word))
    {
        by_word = 0;
        insert_bits(&res->word, 4, 5, 1);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
    }/*now checks if its a matrix*/
    else if(strtok(word,"[") != NULL && (temp_label = find_label(labels_table, strtok(word,"["))) != NULL)
    {
        by_word = 0;
        insert_bits(&res->word, 4, 5, 2);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
        by_word = 0;
        strtok(word, "]")
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 6, 9, check_if_register(word, line, file_name));
        strtok(NULL, "[");
        if(isspace(*word))
        {
            fprintf(stdout, "illegal space (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        strtok(NULL,"]");
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 2, 5, check_if_register(word, line, file_name));
    }/*now checks if its a register*/
    else if((register_num = check_if_register(word, line, file_name)) != -1)
    {
        insert_bits(&res->word, 4, 5, 3);
        by_word = 0;
        insert_bits(&by_word, START_DEST_REGISTER, END_DEST_REGISTER, register_num);
        add_binary_node(&res, by_word);
    }
    else
    {
        fprintf(stdout, "undefined text or illegal source (line %d) in file %s.\n",line, file_name);
        return NULL;
    }

    /*start getting the dest*/
    strtok(word, " ");
    if(word == NULL)
    {
        fprintf(stdout, "missing parameters (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    while(isspace(*word)) word++;
    /*check if it is a number*/
    if(*word == '#')
    {
        word++;
        insert_bits(&res->word, 2, 3, 0);
        save_word = word;
        insert_bits(&by_word, 2, 10, get_num(save_word, line, file_name));
        add_binary_node(&res, by_word);
    }/*now checks if its a register*/
    else if((register_num = check_if_register(word, line, file_name)) != -1)
    {
        insert_bits(&res->word, 2, 3, 3);
        by_word = 0;
        insert_bits(&by_word, START_DEST_REGISTER, END_DEST_REGISTER, register_num);
        add_binary_node(&res, by_word);
    }/*now checks if its a matrix*/
    else if(strtok(word,"[") != NULL && (temp_label = find_label(labels_table, strtok(word,"["))) != NULL)
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 2);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
        by_word = 0;
        strtok(word, "]")
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 6, 9, check_if_register(word, line, file_name));
        strtok(NULL, "[");
        if(isspace(*word))
        {
            fprintf(stdout, "illegal space (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        strtok(NULL,"]");
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 2, 5, check_if_register(word, line, file_name));
    }/*checks if it is a label*/
    else if(strtok(word," ") != NULL && temp_label = find_label(labels_table, strtok(word," ")))
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
    }
    else
    {
        fprintf(stdout, "undefined text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    /*searching for a comma*/
    strtok(word, " ");
    if(word == NULL)
    {
        fprintf(stdout, "missing arguments (line %d) in file %s.\n",line, file_name);
        return NULL;
    }

    while(*word != '\0' && isspace(*word)) word++;
    if(*word != ',')
    {
        fprintf(stdout, "missing comma (line %d) in file %s.\n",line, file_name);
        return NULL;
    }


    /*checking if there are illegal chars at end of command*/
    if(strtok(word," ") != NULL)
    {
        fprintf(stdout, "illegal text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    return res;
}
/*source: 0,1,2,3. dest: 0,1,2,3*/
binary* code_for_one(char* word, int line, char* file_name, int opcode)
{
    char* save_word;
    short by_word = 0, register_num;
    labels* temp_label;
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;

    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    /*start getting the source*/
    strtok(word, " ");
    if(word == NULL)
    {
        fprintf(stdout, "missing parameters (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    while(isspace(*word)) word++;
    /*now the input may be four things 1) label, 2) number, 3) register, 4) calling a matrix*/
    /*check if it is a number*/
    if(*word == '#')
    {
        word++;
        insert_bits(&res->word, 4, 5, 0);
        save_word = word;
        insert_bits(&by_word, 2, 10, get_num(save_word, line, file_name));
        add_binary_node(&res, by_word);
    }/*now checks if its a register*/
    else if((register_num = check_if_register(word, line, file_name)) != -1)
    {
        insert_bits(&res->word, 4, 5, 3);
        by_word = 0;
        insert_bits(&by_word, START_DEST_REGISTER, END_DEST_REGISTER, register_num);
        add_binary_node(&res, by_word);
    }/*now checks if its a matrix*/
    else if(strtok(word,"[") != NULL && (temp_label = find_label(labels_table, strtok(word,"["))) != NULL)
    {
        by_word = 0;
        insert_bits(&res->word, 4, 5, 2);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
        by_word = 0;
        strtok(word, "]")
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 6, 9, check_if_register(word, line, file_name));
        strtok(NULL, "[");
        if(isspace(*word))
        {
            fprintf(stdout, "illegal space (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        strtok(NULL,"]");
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 2, 5, check_if_register(word, line, file_name));
    }/*checks if it is a label*/
    else if(strtok(word," ") != NULL && temp_label = find_label(labels_table, strtok(word," ")))
    {
        by_word = 0;
        insert_bits(&res->word, 4, 5, 1);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
    }
    else
    {
        fprintf(stdout, "undefined text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    /*searching for a comma*/
    strtok(word, " ");
    if(word == NULL)
    {
        fprintf(stdout, "missing arguments (line %d) in file %s.\n",line, file_name);
        return NULL;
    }

    while(*word != '\0' && isspace(*word)) word++;
    if(*word != ',')
    {
        fprintf(stdout, "missing comma (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    /*getting the dest*/
    if(*word == '#')
    {
        word++;
        insert_bits(&res->word, 2, 3, 0);
        save_word = word;
        insert_bits(&by_word, 2, 10, get_num(save_word, line, file_name));
        add_binary_node(&res, by_word);
    }/*now checks if its a register*/
    else if((register_num = check_if_register(word, line, file_name)) != -1)
    {
        insert_bits(&res->word, 2, 3, 3);
        by_word = 0;
        insert_bits(&by_word, START_DEST_REGISTER, END_DEST_REGISTER, register_num);
        add_binary_node(&res, by_word);
    }/*now checks if its a matrix*/
    else if(strtok(word,"[") != NULL && (temp_label = find_label(labels_table, strtok(word,"["))) != NULL)
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 2);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
        by_word = 0;
        strtok(word, "]")
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 6, 9, check_if_register(word, line, file_name));
        strtok(NULL, "[");
        if(isspace(*word))
        {
            fprintf(stdout, "illegal space (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        strtok(NULL,"]");
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 2, 5, check_if_register(word, line, file_name));
    }/*checks if it is a label*/
    else if(strtok(word," ") != NULL && temp_label = find_label(labels_table, strtok(word," ")))
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
    }
    else
    {
        fprintf(stdout, "undefined text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    /*checking if there are illegal chars at end of command*/
    if(strtok(word," ") != NULL)
    {
        fprintf(stdout, "illegal text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    return res;
}
/*source: 1,2. dest: 1,2,3*/
binary* code_for_four(char* word, int line, char* file_name, int opcode)
{
    char* save_word;
    short by_word = 0, register_num;
    labels* temp_label;
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;

    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    /*getting the source*/
    /*checks if it is a label*/
    if((strtok(word," ") != NULL) && temp_label = find_label(labels_table, word))
    {
        by_word = 0;
        insert_bits(&res->word, 4, 5, 1);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
    }/*now checks if its a matrix*/
    else if(strtok(word,"[") != NULL && (temp_label = find_label(labels_table, strtok(word,"["))) != NULL)
    {
        by_word = 0;
        insert_bits(&res->word, 4, 5, 2);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
        by_word = 0;
        strtok(word, "]")
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 6, 9, check_if_register(word, line, file_name));
        strtok(NULL, "[");
        if(isspace(*word))
        {
            fprintf(stdout, "illegal space (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        strtok(NULL,"]");
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 2, 5, check_if_register(word, line, file_name));
    }
    else
    {
        fprintf(stdout, "undefined text or illegal source (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    /*searching for a comma*/
    strtok(word, " ");
    if(word == NULL)
    {
        fprintf(stdout, "missing arguments (line %d) in file %s.\n",line, file_name);
        return NULL;
    }

    while(*word != '\0' && isspace(*word)) word++;
    if(*word != ',')
    {
        fprintf(stdout, "missing comma (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    /*start getting the dest*/
    if((strtok(word," ") != NULL) && temp_label = find_label(labels_table, word))
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
    }/*now checks if its a matrix*/
    else if(strtok(word,"[") != NULL && (temp_label = find_label(labels_table, strtok(word,"["))) != NULL)
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 2);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
        by_word = 0;
        strtok(word, "]")
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 6, 9, check_if_register(word, line, file_name));
        strtok(NULL, "[");
        if(isspace(*word))
        {
            fprintf(stdout, "illegal space (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        strtok(NULL,"]");
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 2, 5, check_if_register(word, line, file_name));
    }/*now checks if its a register*/
    else if((register_num = check_if_register(word, line, file_name)) != -1)
    {
        insert_bits(&res->word, 2, 3, 3);
        by_word = 0;
        insert_bits(&by_word, START_DEST_REGISTER, END_DEST_REGISTER, register_num);
        add_binary_node(&res, by_word);
    }
    else
    {
        fprintf(stdout, "undefined text or illegal source (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    /*checks for illegal text at end of command*/
    if(strtok(word," ") != NULL)
    {
        fprintf(stdout, "illegal text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    return res;
}
/*source: none. dest: 1,2,3*/
binary* code_for_five(char* word, int line, char* file_name, int opcode)
{
    char* save_word;
    short by_word = 0, register_num;
    labels* temp_label;
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;

    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);

    strtok(word, " ");
    if(word == NULL)
    {
        fprintf(stdout, "missing parameters (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    while(isspace(*word)) word++;
    /*now the input may be four things 1) label, 2) number, 3) register, 4) calling a matrix*/
    /*checks if its a register*/
    if((register_num = check_if_register(word, line, file_name)) != -1)
    {
        insert_bits(&res->word, 2, 3, 3);
        by_word = 0;
        insert_bits(&by_word, START_DEST_REGISTER, END_DEST_REGISTER, register_num);
        add_binary_node(&res, by_word);
    }/*now checks if its a matrix*/
    else if(strtok(word,"[") != NULL && (temp_label = find_label(labels_table, strtok(word,"["))) != NULL)
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 2);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
        by_word = 0;
        strtok(word, "]")
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 6, 9, check_if_register(word, line, file_name));
        strtok(NULL, "[");
        if(isspace(*word))
        {
            fprintf(stdout, "illegal space (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        strtok(NULL,"]");
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 2, 5, check_if_register(word, line, file_name));
    }/*checks if it is a label*/
    else if(strtok(word," ") != NULL && temp_label = find_label(labels_table, strtok(word," ")))
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
    }
    else
    {
        fprintf(stdout, "undefined text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    if(strtok(word," ") != NULL)
    {
        fprintf(stdout, "illegal text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    return res;
}
/*source: none. dest: 0,1,2,3*/
binary* code_for_thirteen(char* word, int line, char* file_name, int opcode, label* labels_table)
{
    char* save_word;
    short by_word = 0, register_num;
    labels* temp_label;
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;

    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);

    strtok(word, " ");
    if(word == NULL)
    {
        fprintf(stdout, "missing parameters (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    while(isspace(*word)) word++;
    /*now the input may be four things 1) label, 2) number, 3) register, 4) calling a matrix*/
    /*check if it is a number*/
    if(*word == '#')
    {
        word++;
        insert_bits(&res->word, 2, 3, 0);
        save_word = word;
        insert_bits(&by_word, 2, 10, get_num(save_word, line, file_name));
        add_binary_node(&res, by_word);
    }/*now checks if its a register*/
    else if((register_num = check_if_register(word, line, file_name)) != -1)
    {
        insert_bits(&res->word, 2, 3, 3);
        by_word = 0;
        insert_bits(&by_word, START_DEST_REGISTER, END_DEST_REGISTER, register_num);
        add_binary_node(&res, by_word);
    }/*now checks if its a matrix*/
    else if(strtok(word,"[") != NULL && (temp_label = find_label(labels_table, strtok(word,"["))) != NULL)
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 2);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
        by_word = 0;
        strtok(word, "]")
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 6, 9, check_if_register(word, line, file_name));
        strtok(NULL, "[");
        if(isspace(*word))
        {
            fprintf(stdout, "illegal space (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        strtok(NULL,"]");
        if(check_if_register(word, line, file_name) == -1)
        {
            fprintf(stdout, "undefined variable (line %d) in file %s.\n",line, file_name);
            return NULL;
        }
        insert_bits(&by_word, 2, 5, check_if_register(word, line, file_name));
    }/*checks if it is a label*/
    else if(strtok(word," ") != NULL && temp_label = find_label(labels_table, strtok(word," ")))
    {
        by_word = 0;
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, temp_label->address);
        insert_bits(&by_word, 0, 1, 2);
        add_binary_node(&res, by_word);
    }
    else
    {
        fprintf(stdout, "undefined text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    if(strtok(word," ") != NULL)
    {
        fprintf(stdout, "illegal text (line %d) in file %s.\n",line, file_name);
        return NULL;
    }
    return res;
}
/*source: none. dest: none*/
binary* code_for_fourteen(char* word, int line, int file_name, int opcode)
{
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    res->next = NULL;
    return res;
}

