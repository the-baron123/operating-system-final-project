#ifndef INPUTS_1_H
#define INPUTS_1_H

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

/*the output structure of the first phase*/
typedef struct state
{
    int dc_count;
    int ic_count;
    labels* label;
    dc_code* data_code;
}state;

/*converting a number into a 4 digit special*/
char* base_four_no_bit(int number);
char* base_four(int number, int bits);
/*freeing dc memory */
void free_labels_list(labels** head_ref);
/*freeing dc memory */
void free_dc_list(dc_code** head_ref);
/*free mat memory*/
void free_mat_list(mat** head_ref);
/*free binary memory*/
void free_binary_list(binary** head_ref);
/*free ic memory*/
void free_ic_list(ic_code** head_ref);
/*searching mat function*/
labels* find_label(labels* head, char* target_name);
/*this function enters bits to limited area*/
void insert_bits(short* word, int start_bit, int end_bit, int num_to_insert);
/*insert node into binary list*/
binary* add_binary_node(binary** head, short word_value);
/*insert node into binary list*/
binary* add_binary_node_with_word(binary** head, binary* word);
/*this function insert new node into dc structure*/
int add_dc(dc_code** head, short dc, int address);
/*this function insert new node into ic structure*/
int add_ic(ic_code** head, short ic, int address);
/*this function insert a new label into the linked list*/
int add_label(labels** head, unsigned short address, const char* name, const char* guiding_name, int line, char* file_name);
/*gets a label and add it into the extern or entry files*/
void check_extern(char* sy_name, labels* temp_label, char* file_name, int line);
/*
this function checks if some operation is in the language and return it's binary value
if the word does not exist return 0
*/
int check_if_operation_in_language(char* name, char** operation_list);
/*checks label name validation according to the task*/
int check_label_validation(char* start, char* end_str, int line, char* file, labels* sy_tabel, int phase);
/*function to add a node into the mat linked list*/
int add_mat(mat** head, mat* node_add);
/*gets a string that should be a number(or just regular string and convert it back into a number*/
int get_last_num(char* num, int line, char* name);
/*gets a string that should be a number(or just regular string and convert it back into a number*/
int get_num(char* start, char* end_num, int line, char* name);
/*
this function gets a string and convert it into an array of integers
that each index in it is the binary value of the char in the index space on the string
*/
int* get_str(char* start, char* end_str, int line, char* name);
/*this function gets the first integer from the start of the word*/
int get_integer_mat(char* mat, int line, char* name);
/*
this function gets a string that should be a define of a matrix
and gets the matrix and it's values
 */
mat* get_mat(char* mat_info, int line, char* name, int empty_mat);
/*gets a string and checks if its a register and if it is return the number of the register*/
short check_if_register(char* word, int line, char* file_name);
/*this function gets a word in the language and convert it to it's binary value and return it*/
/*source: 1,2,3 dest: 0,1,2,3*/
binary* code_for_zero(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);
/*source: 0,1,2,3. dest: 0,1,2,3*/
binary* code_for_one(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);
/*source: 1,2. dest: 1,2,3*/
binary* code_for_four(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);
/*source: none. dest: 1,2,3*/
binary* code_for_five(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);
/*source: none. dest: 0,1,2,3*/
binary* code_for_thirteen(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);
/*source: none. dest: none*/
binary* code_for_fourteen(int opcode);

/*those function counts the IC word that are needed to the assembly word*/
int count_ic_on_word_zero(char* word, int line, char* file_name);
int count_ic_on_word_one(char* word, int line, char* file_name);
int count_ic_on_word_four(char* word, int line, char* file_name);
int count_ic_on_word_five(char* word, int line, char* file_name);
int count_ic_on_word_thirteen(char* word, int line, char* file_name);
int count_ic_on_word_fourteen(char* word, int line, char* file_name);
#endif /* INPUTS_1_H */
