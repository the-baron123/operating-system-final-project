#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "pre_asembler.h"

#define MAX_LABEL_LEN 29
#define WORD_LEN 10
#define START_OPCODE 6
#define END_OPCODE 9
#define START_DEST_REGISTER 2
#define END_DEST_REGISTER 5
#define START_SOURCE_REGISTER 6
#define END_SOURCE_REGISTER 9
#define MAX_BITS 10
#define PHASE_1 1
#define PHASE_2 2
#define START_COUNT 100

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
/*the output structure of the first phase*/
typedef struct state
{
    int dc_count;
    int ic_count;
    labels* label;
    dc_code* data_code;
}state;

/* converting a number into a base-4 string with "bits" bits*/
char* base_four(int number, int bits)
{
    char *base_ch, ch;
    int two_bits, index, shift;

    base_ch = (char*)malloc(sizeof(char) * (bits + 1)); /* +1 for '\0' */
    if (!base_ch)
    {
        fprintf(stdout, "memory allocation failed");
        exit(1);
    }

    for (index = 0; index < bits; index++)
    {
        shift = 2 * (bits - index - 1);
        two_bits = (number >> shift) & 0x3;

        switch (two_bits)
        {
        case 0: ch = 'a'; break;
        case 1: ch = 'b'; break;
        case 2: ch = 'c'; break;
        case 3: ch = 'd'; break;
        }
        base_ch[index] = ch;
    }

    base_ch[bits] = '\0';
    return base_ch;
}

/* converting a number into a base-4 string (a,b,c,d) without leading 'a's */
char* base_four_no_bit(int number)
{
    char *base_ch, ch;
    int two_bits, index, shift, start, len;

    base_ch = (char*)malloc(sizeof(char) * 5);
    if(!base_ch)
    {
        fprintf(stdout, "memory allocation failed");
        exit(1);
    }

    start = -1;

    for(index = 0; index < 4; index++)
    {
        shift = 2 * (3 - index);
        two_bits = (number >> shift) & 0x3;

        switch(two_bits)
        {
        case 0: ch = 'a'; break;
        case 1: ch = 'b'; break;
        case 2: ch = 'c'; break;
        case 3: ch = 'd'; break;
        }

        if(ch != 'a' && start == -1)/*start counting the indexes*/
        {
            start = index;
        }

        base_ch[index] = ch;
    }

    if(start == -1)
    {
        base_ch[0] = 'a';
        base_ch[1] = '\0';
    }
    else
    {
        len = 4 - start;
        memmove(base_ch, base_ch + start, len);
        base_ch[len] = '\0';
    }

    return base_ch;
}

/*freeing labels memory */
void free_labels_list(labels** head_ref)
{
    labels* current = *head_ref;
    labels* temp;

    while (current != NULL)
    {
        temp = current->next;

        free(current->label_name);
        free(current->type);

        free(current);
        current = temp;
    }
    *head_ref = NULL;
}
/*freeing dc memory */
void free_dc_list(dc_code** head_ref)
{
    dc_code* current = *head_ref;
    dc_code* temp;

    while (current != NULL)
    {
        temp = current->next;
        free(current);
        current = temp;
    }
    *head_ref = NULL;
}
/*free mat memory*/
void free_mat_list(mat** head_ref)
{
    int i;
    mat* current = *head_ref;
    mat* temp;

    while (current != NULL)
    {
        temp = current->next;

        if (current->nums)
        {
            for (i = 0; i < current->row; i++)
            {
                free(current->nums[i]);
            }
            free(current->nums);
        }

        free(current);
        current = temp;
    }
    *head_ref = NULL;
}
/*free binary memory*/
void free_binary_list(binary** head_ref)
{
    binary* current = *head_ref;
    binary* temp;

    while (current != NULL)
    {
        temp = current->next;
        free(current);
        current = temp;
    }
    *head_ref = NULL;
}
/*free ic memory*/
void free_ic_list(ic_code** head_ref)
{
    ic_code* current = *head_ref;
    ic_code* temp;

    while (current != NULL)
    {
        temp = current->next;
        free(current);
        current = temp;
    }
    *head_ref = NULL;
}
/*searching mat function*/
labels* find_label(labels* head, char* target_name)
{
    char buffer[MAX_LABEL_LEN+1], *find;

    while(isspace(*target_name)) target_name++;
    strcpy(buffer, target_name);
    buffer[sizeof(buffer)-1] = '\0';
    find = buffer;
    find = strchr(buffer, '\n');
    if(find) *find = '\0';

    while (head != NULL)
    {
        if(head->label_name != NULL)
        {
            if(strcmp(head->label_name, buffer) == 0)
            {
                return head;
            }
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
    binary* current;
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
        current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
    }

    return *head;
}
/*insert node into binary list*/
binary* add_binary_node_with_word(binary** head, binary* word)
{
    binary* current;
    if (word == NULL) return *head;

    if (*head == NULL) /*empty list*/
    {
        *head = word;
    }
    else
    {
        current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = word;
    }

    return *head;
}/*this function insert new node into dc structure*/
int add_dc(dc_code** head, short dc, int address)
{
    dc_code* new_node1, *temp1;

    new_node1 = (dc_code*)malloc(sizeof(dc_code));
    if (!new_node1)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    new_node1->code_value = dc;
    new_node1->addres = address;
    new_node1->next = NULL;
    temp1 = *head;

    if (temp1 == NULL)/*empty list*/
    {
        *head = new_node1;
    }
    else
    {
        while(temp1->next != NULL)
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
    ic_code *new_node2, *temp2;

    new_node2 = (ic_code*)malloc(sizeof(ic_code));
    if (!new_node2)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    new_node2->code_value = ic;
    new_node2->addres = address;
    new_node2->next = NULL;

    if (*head == NULL)
    {
        *head = new_node2;
        return 1;
    }

    temp2 = *head;
    while (temp2->next != NULL)
    {
        temp2 = temp2->next;
    }
    temp2->next = new_node2;

    return 1;
}
/*this function insert a new label into the linked list*/
int add_label(labels** head, unsigned short address, const char* name, const char* guiding_name, int line, char* file_name)
{
    labels* current = *head;
    labels* new_label;

    /*check if name exist*/
    while (current != NULL)
    {
        if (current->label_name != NULL && strcmp(current->label_name, name) == 0)
        {
            if((strcmp(current->type, ".extern") == 0 ||strcmp(current->type, ".entry") == 0))
            {
                return 1;
            }
            add_error("Error: Label already exists. ", line, file_name);
            return 0;
        }
        current = current->next;
    }

    /*create new node*/
    new_label = (labels*)calloc(1, sizeof(labels));
    if (!new_label)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }

    new_label->address = address;

    new_label->label_name = (char*)malloc(strlen(name) + 1);
    if (!new_label->label_name)
    {
        free(new_label);
        fprintf(stderr, "Error: Memory allocation for label name failed.\n");
        exit(1);
    }
    strcpy(new_label->label_name, name);

    new_label->type = (char*)malloc(strlen(guiding_name) + 1);
    if (!new_label->type)
    {
        free(new_label->label_name);
        free(new_label);
        fprintf(stderr, "Error: Memory allocation for guiding name failed.\n");
        exit(1);
    }
    strcpy(new_label->type, guiding_name);

    /*adding to the list*/
    new_label->next = *head;
    *head = new_label;

    return 1; /*return "true"*/
}
/*gets a label and add it into the extern or entry files*/
void check_extern(char* sy_name, labels* label_table, char* file_name, int IC)
{
    FILE* ent = NULL, *ext = NULL;
    char* p_ent_name, *save;
    labels* search_node;
    search_node = find_label(label_table, sy_name);
    if(!search_node)
    {
        return;
    }
    printf("label:%s:, type:%s:\n",sy_name, search_node->type);
    if(strcmp(search_node->type, ".extern") == 0)
    {
        if(ext == NULL)
        {
            p_ent_name = file_name;
            save = p_ent_name;
            while(*save != '.') save++;
            *save = '\0';
            ext = fopen(strcat(p_ent_name, ".ext"), "a");
            if (!ext)
            {
                fprintf(stdout, "Failed to open file");
                exit(1);
            }
        }
        fputs(sy_name, ext);
        fputs("   ", ext);
        save = base_four(IC, 4);
        fputs(save, ext);
        free(save);
        fputs("\n", ext);
        return;
    }
    if(strcmp(search_node->type, "xentry") == 0 || strcmp(search_node->type, ".entry") == 0)
    {
        if(ent == NULL)
        {
            p_ent_name = file_name;
            save = p_ent_name;
            while(*save != '.') save++;
            *save = '\0';
            ent = fopen(strcat(p_ent_name, ".ent"), "a");
            if (!ent)
            {
                fprintf(stdout, "Failed to open file");
                exit(1);
            }
        }
        fputs(sy_name, ent);
        fputs("   ", ent);
        save = base_four(IC, 4);
        fputs(save, ent);
        free(save);
        fputs("\n", ent);
        return;
    }
}
/*
this function checks if some operation is in the language and return it's binary value
if the word does not exist return 0
*/
int check_if_operation_in_language(char* name, char** operation_list)
{
    int i = 0;
    char* endl = strchr(name, '\n');
    if(endl)
    {
        *endl = '\0';
    }
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
int check_label_validation(char* start, char* end, int line, char* file, labels* sy_tabel, int phase)
{
    /*creating an array of all the operation in the language*/
    int i;
    char *operation_list[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    labels* search = NULL;
    char* save, *name = (char*)malloc(sizeof(char)*(end-start+1));
    if(!name)
    {
        fprintf(stdout,"memory allocation error");
        exit(1);
    }
    while(*start != '\0' && isspace(*start)) start++;
    if(!isalpha(*start))
    {
        free(name);
        add_error("invalid label name(first letter is not an alphabet char) ", line, file);
        return 0;
    }
    if(end - start > MAX_LABEL_LEN)
    {
        free(name);
        add_error("label name is too long ", line, file);
        return 0;
    }
    save = start;
    while (*save != '\0')/*found spaces if there are some and report it*/
    {
        if (isspace(*save))
        {
            free(name);
            add_error("label name has illegal spaces in it ", line, file);
            return 0;
        }
        if(!isdigit(*save) && !isalpha(*save))
        {
            free(name);
            add_error("label name has illegal char in it ", line, file);
            return 0;
        }
        save++;
    }
    for(i = 0;i<end- start;i++)
    {
        name[i] = *start;
    }
    name[i] = '\0';
    if((search = find_label(sy_tabel, name)) != NULL && strcmp(search->type, ".entry") != 0)
    {
        save = search->type;
        *save = 'x';
        free(name);
        return 1;
    }
    if((search = find_label(sy_tabel, name)) != NULL && phase == PHASE_1)
    {
        free(name);
        add_error("label is already exist ", line, file);
        return 0;
    }
    if(find_node(root, name) != NULL)/*searching for a macro*/
    {
        free(name);
        add_error("label name is mcro name ", line, file);
        return 0;
    }
    if(check_if_operation_in_language(name,operation_list) != -1)
    {
        free(name);
        add_error("label name is operation name ", line, file);
        return 0;
    }
    free(name);
    return 1;/*true*/
}
/*function to add a node into the mat linked list*/
int add_mat(mat** head, mat* node_add)
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
int get_last_num(char* num, int line, char* name)
{
    char* save;
    int number = 0, sign = 1;

    while((*num != '\0') && isspace(*num)) num++;/*removing white chars*/
    if(*num == '\0')
    {
        add_error("missing number ",line, name);
        return 1 << 11;
    }
    save = num;
    while(!isspace(*save) && *save != '\0')/*searching for illegal chars in number*/
    {
        if(sign == 1 && (*save == '-' || *save == '+'))
        {
            sign = 0;
            save++;
            continue;
        }
        else if(sign == 0 && (*save == '-' || *save == '+'))
        {
            add_error("illegal text in number first ",line, name);
            return 1 << 11;
        }
        if(!isdigit(*save) && *save != '-' && *save != '+')
        {
            add_error("illegal text in number ",line, name);
            return 1 << 11;
        }
        save++;
    }

    while(*save != '\0')/*searching for illegal chars after number*/
    {
        if(!isspace(*save))
        {
            add_error("illegal char after number ",line, name);
            return 1 << 11;
        }
        save++;
    }
    number = atoi(num);/*getting the number with atoi*/
    if(number > 1 << 8)
    {
        add_error("number is too big ",line, name);
        return 1 << 11;
    }
    return number;
}
/*start contains */
int get_num(char* start, char* end, int line, char* name)
{
    char* save;
    int number = 0, sign = 1;

    end--;
    while(start > end && isspace(*start)) start++;/*removing white chars*/
    if(start > end)
    {
        add_error("missing number ",line, name);
        return 1 << 11;
    }
    save = start;
    while(!isspace(*save) && end > save)/*searching for illegal chars in number*/
    {
        if(sign == 1 && (*save == '-' || *save == '+'))
        {
            sign = 0;
            save ++;
            continue;
        }
        else if( sign == 0 && (*save == '-' || *save == '+'))
        {
            add_error("illegal text in number first ",line, name);
            return 1 << 11;
        }
        if(!isdigit(*save) && *save != '-' && *save != '+' && *save != ',')
        {
            add_error("illegal text in number ",line, name);
            return 1 << 11;
        }
        save++;
    }

    while(end > save)/*searching for illegal chars after number*/
    {
        if(!isspace(*save) && *save != '\n')
        {
            add_error("illegal char after number ",line, name);
            return 1 << 11;
        }
        save++;
    }
    number = atoi(start);/*getting the number with atoi*/
    if(number > (1 << 10) || number*-1 > (1 << 10))
    {
        add_error("number is too big ",line, name);
        return 1 << 11;
    }
    return number;
}
/*
this function gets a string and convert it into an array of integers
that each index in it is the binary value of the char in the index space on the string
*/
int* get_str(char* start, char* end_str, int line, char* name)
{
    int *res, *p_res;

    res = (int*)malloc(((end_str-start)+1)* sizeof(int));/*allocating memory for string length + '\0'*/
    p_res = res;/*setting a pointer that I can move without worrying to lose my position */
    while(start != end_str)
    {
        if(!isprint(*start) && !isspace(*start))/*checking if the char is printable*/
        {
            free(res);
            add_error("string contains a non printable char", line, name);
            return NULL;
        }
        /*setting the array*/
        *p_res = (int)*start;
        p_res++;
        start++;
    }
    *p_res = 0;
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
        add_error("missing parameters", line, name);
        return res;
    }
    if(*mat != '[')
    {
        add_error("missing '[' ", line, name);
        return res;
    }
    mat++;
    while(*mat != '\0' && isspace(*mat)) mat++;
    if(*mat == '\0')
    {
        add_error("missing number ", line, name);
        return res;
    }
    if(!isdigit(*mat))
    {
        add_error("illegal char(should be an integer) ", line, name);
        return res;
    }
    /*getting the number*/
    save = mat;
    while(isdigit(*save))/*counting the number digits*/
    {
        count_num++;
        save++;
    }
    if(*save == '\0')
    {
        add_error("missing parameters", line, name);
        return res;
    }
    if(isalpha(*save))
    {
        add_error("illegal char", line, name);
        return res;
    }
    if(isspace(*save))
    {
        while(*save != '\0' && *save != ']')
        {
            save++;
        }
        if(*save == '\0')
        {
            add_error("missing ']' ", line, name);
            return res;
        }
    }
    if(*save != ']')
    {
        add_error("missing ']' ", line, name);
        return res;
    }

    integer = (char*)malloc((count_num+1)* sizeof(char));
    if(integer == NULL)
    {
        add_error("allocating memory error", line, name);
        exit(1);
    }
    save = integer;
    while(isdigit(*mat))/*copping the number*/
    {
        *save = *mat;
        save++;
        mat++;
    }
    *save = '\0';
    res = atoi(integer);

    free(integer);
    return res;
}
/*
this function gets a string that should be a define of a matrix
and gets the matrix and it's values
 */
mat* get_mat(char* mat_info, int line, char* name, int empty_mat)
{
    mat* new_mat;
    char *start, *end;
    int row = 0, colom = 0, **nums, i, j, integer;

    new_mat = (mat*)malloc(sizeof(mat));
    if (new_mat == NULL)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    new_mat->next = NULL;
    new_mat->nums = NULL;
    if(isspace(*mat_info))
    {
        free_mat_list(&new_mat);
        add_error("illegal white char ", line, name);
        return NULL;
    }
    /*getting the size of the matrix*/
    row = get_integer_mat(mat_info, line, name);
    if(row == -1)
    {
        /*the error were print in the function*/
        free_mat_list(&new_mat);
        return NULL;
    }
    mat_info = strchr(mat_info, ']');/*we checked in "get_number_mat" function that it exist*/
    mat_info++;/*if it all good it should be point to '['*/

    if(*mat_info == '\0')
    {
        add_error("missing parameters ", line, name);
        free_mat_list(&new_mat);
        return NULL;
    }
    if(isspace(*mat_info))
    {
        add_error("illegal white char ", line, name);
        free_mat_list(&new_mat);
        return NULL;
    }
    if(*mat_info != '[')
    {
        add_error("illegal char after mat name ", line, name);
        free_mat_list(&new_mat);
        return NULL;
    }

    colom = get_integer_mat(mat_info, line, name);
    if(colom == -1)
    {
        /*the error were print in the function*/
        free_mat_list(&new_mat);
        return NULL;
    }
    /*searching for space after second ']'*/
    mat_info = strchr(mat_info, ']');
    mat_info++;
    if(!isspace(*mat_info))
    {
        add_error("missing space ",line, name);
        free_mat_list(&new_mat);
        return NULL;
    }

    /*allocating memory for the array and setting it for 0*/
    nums = (int**)malloc(row* sizeof(int*));
    if (nums == NULL) {
        fprintf(stdout, "Memory allocation failed for rows.\n");
        free_mat_list(&new_mat);
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
            free_mat_list(&new_mat);
            exit(1);
        }
    }
    start = mat_info;
    end = strchr(mat_info, ',');
    if(empty_mat == 1)
    {
        new_mat->nums = nums;
        new_mat->colom = colom;
        new_mat->row = row;
        return new_mat;
    }
    /*getting the numbers of the mat*/
    for(i = 0;i<row;i++)
    {
        for(j = 0;j<colom;j++)
        {
            if(end != NULL)
            {
                integer = get_num(start, end, line, name);
                if(integer == 1 << 11)
                {
                    for(i = 0;i<row;i++)
                    {
                        free(nums[i]);
                    }
                    free(nums);
                    free_mat_list(&new_mat);
                    return NULL;
                }
                else
                {
                    nums[i][j] = integer;
                }
            }
            else
            {
                integer = get_last_num(start, line, name);
                if(integer == 1 << 11)
                {
                    for(i = 0;i<row;i++)
                    {
                        free(nums[i]);
                    }
                    free(nums);
                    free_mat_list(&new_mat);
                    return NULL;
                }
                nums[i][j] = integer;
                new_mat->nums = nums;
                new_mat->colom = colom;
                new_mat->row = row;
                return new_mat;
            }
            start = (end+1);
            end = strchr(start, ',');
        }
    }
    if(end != NULL)/*if I got too much inputs*/
    {
        add_error("to much inputs ", line, name);
        for(i = 0;i<row;i++)
        {
            free(nums[i]);
        }
        free(nums);
        free_mat_list(&new_mat);
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
    short res = 0;

    while(*word != '\0' && isspace(*word)) word++;
    if(*word != 'r')
    {
        add_error("register don't start with 'r'", line, file_name);
        return -1;
    }
    word++;
    if(*word != '\0' && isdigit((*word)))
    {
        if((res = atoi(word)) > 7)
        {
            add_error("register does not exist", line, file_name);
            return -1;
        }
        word++;
        while(*word != '\0' && *word != ',')
        {
            if(isspace(*word))
            {
                word++;
            }
        }
        if(*word == ',' || *word == '\0')
        {
            return res;
        }
    }
    add_error("illegal char in register ", line, file_name);
    return -1;
}

int count_ic_on_word_zero(char* word, int line, char* file_name)
{
    float count_ic = 1;
    char* start;

    while(*word != '\0' && isspace(*word)) word++;
    if(*word == '\0')
    {
        add_error("missing parameters ", line, file_name);
    }
    start = word;

    if(*start == '#')
    {
        count_ic++;
    }
    else if(strchr(start, '[') != NULL)
    {
        count_ic += 2;
    }
    else if(strchr(start, 'r') != NULL)
    {
        count_ic += 0.5;
    }
    else
    {
        count_ic++;
    }

    start = strchr(start, ',');
    if(!start)
    {
        add_error("missing comma ",line, file_name);
        count_ic += 2;
        return (count_ic > (int)count_ic)? (int)(count_ic+1):(int)count_ic;
    }
    start++;

    while(*start != '\0' && isspace(*start)) start++;
    if(*start == '\0')
    {
        add_error("missing parameters ", line, file_name);
    }

    if(*start == '#')
    {
        count_ic++;
    }
    else if(strchr(start, '[') != NULL)
    {
        count_ic += 2;
    }
    else if(strchr(start, 'r') != NULL)
    {
        count_ic += 0.5;
    }
    else
    {
        count_ic++;
    }
    return (count_ic > (int)count_ic)? (int)(count_ic+1):(int)count_ic;
}
int count_ic_on_word_one(char* word, int line, char* file_name)
{
    int count_ic = 1;
    char* start;

    while(*word != '\0' && isspace(*word)) word++;
    if(*word == '\0')
    {
        add_error("missing parameters ", line, file_name);
    }
    start = word;

    if(*start == '#')
    {
        count_ic++;
    }
    else if(strchr(start, '[') != NULL)
    {
        count_ic += 2;
    }
    else if(strchr(start, 'r') != NULL)
    {
        count_ic += 0.5;
    }
    else
    {
        count_ic++;
    }

    start = strchr(start, ',');
    if(!start)
    {
        add_error("missing comma ",line, file_name);
        count_ic += 2;
        return (count_ic > (int)count_ic)? (int)(count_ic+1):(int)count_ic;
    }
    start++;

    while(*start != '\0' && isspace(*start)) start++;
    if(*start == '\0')
    {
        add_error("missing parameters ", line, file_name);
    }

    if(*start == '#')
    {
        count_ic++;
    }
    else if(strchr(start, '[') != NULL)
    {
        count_ic += 2;
    }
    else if(strchr(start, 'r') != NULL)
    {
        count_ic += 0.5;
    }
    else
    {
        count_ic++;
    }
    return (count_ic > (int)count_ic)? (int)(count_ic+1):(int)count_ic;
}
int count_ic_on_word_four(char* word, int line, char* file_name)
{
    int count_ic = 1;
    char* start;

    while(*word != '\0' && isspace(*word)) word++;
    if(*word == '\0')
    {
        add_error("missing parameters ", line, file_name);
    }
    start = word;

    if(strchr(start, '[') != NULL)
    {
        count_ic += 2;
    }
    else
    {
        count_ic++;
    }

    start = strchr(word, ',');
    if(!start)
    {
        add_error("missing comma ",line, file_name);
        count_ic += 2;
        return (count_ic > (int)count_ic)? (int)(count_ic+1):(int)count_ic;
    }
    start++;

    while(*start != '\0' && isspace(*start)) start++;
    if(*start == '\0')
    {
        add_error("missing parameters ", line, file_name);
    }

    if(strchr(start, '[') != NULL)
    {
        count_ic += 2;
    }
    else if(strchr(start, 'r') != NULL)
    {
        count_ic += 0.5;
    }
    else
    {
        count_ic++;
    }
    return (count_ic > (int)count_ic)? (int)(count_ic+1):(int)count_ic;
}
int count_ic_on_word_five(char* word, int line, char* file_name)
{
    int count_ic = 1;
    char* start;

    while(*word != '\0' && isspace(*word)) word++;
    if(*word == '\0')
    {
        add_error("missing parameters ", line, file_name);
    }
    start = word;

    if(strchr(start, '[') != NULL)
    {
        count_ic += 2;
    }
    else if(strchr(start, 'r') != NULL)
    {
        count_ic += 0.5;
    }
    else
    {
        count_ic++;
    }
    return (count_ic > (int)count_ic)? (int)(count_ic+1):(int)count_ic;
}
int count_ic_on_word_thirteen(char* word, int line, char* file_name)
{
    float count_ic = 1;
    char* start;

    while(*word != '\0' && isspace(*word)) word++;
    if(*word == '\0')
    {
        add_error("missing parameters ", line, file_name);
    }
    start = word;

    if(*start == '#')
    {
        count_ic++;
    }
    else if(strchr(start, '[') != NULL)
    {
        count_ic += 2;
    }
    else if(strchr(start, 'r') != NULL)
    {
        count_ic += 0.5;
    }
    else
    {
        count_ic++;
    }
    return (count_ic > (int)count_ic)? (int)(count_ic+1):(int)count_ic;
}
int count_ic_on_word_fourteen(char* word, int line, char* file_name)
{
    while(*word != '\0' && isspace(*word)) word++;/*"    stop" ->-> "stop"*/
    while(*word != '\0' && isprint(*word)) word++;/*"stop  xxx" ->-> "   xxx"  */
    while(*word != '\0' && isspace(*word)) word++;
    if(*word != '\0')
    {
        add_error("illegal text after command ",line, file_name);
    }
    return 1;
}
