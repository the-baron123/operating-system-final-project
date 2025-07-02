/*in this file we are going to use structs (linked list) to get all of the macros and save their code!*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MAX_IN_LINE 82

/*from here we will print all of ours errors and if there are none we will */
typedef struct errors
{
    char* name_err;
    int line;
    struct errors* next;
} errors;

errors* all_err;
/*the struct build for the linked list to save the macro names and their code*/
typedef struct data
{
    char* name;
    char* code;
    struct data* next;
} data;

data* root = NULL;

/*creating a node*/
data* create_node(const char* name, const char* code) {
    data* new_node = (data*)malloc(sizeof(data));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed for node\n");
        exit(1);
    }

    new_node->name = (char*)malloc(strlen(name) + 1);
    if (new_node->name == NULL) {
        fprintf(stderr, "Memory allocation failed for name\n");
        free(new_node);
        exit(1);
    }
    strcpy(new_node->name, name);

    new_node->code = (char*)malloc(strlen(code) + 1);
    if (new_node->code == NULL) {
        fprintf(stderr, "Memory allocation failed for code\n");
        free(new_node->name);
        free(new_node);
        exit(1);
    }
    strcpy(new_node->code, code);

    new_node->next = NULL;
    return new_node;
}

/*appending to the linked list*/
void add_node(data** head, const char* name, const char* code) {
    data* new_node = create_node(name, code), *temp;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    temp = *head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = new_node;
}

/*searching method by name*/
data* find_node(data* head, const char* name) {
    data* head1 = head;
    while (head1 != NULL) {
        if (strcmp(head1->name, name) == 0)
            return head1;
        head1 = head1->next;
    }
    return NULL; /*did not found a match*/
}


/*this function adds an errors to the errors list! and if the linked list dose not exist it create it*/
void add_error(const char* err_msg, int line) {
    errors* new_err;

    /* Allocate memory for new error node */
    new_err = (errors*)malloc(sizeof(errors));
    if (new_err == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for error struct\n");
        exit(1);
    }

    /* Allocate memory for the error message */
    new_err->name_err = (char*)malloc(strlen(err_msg) + 1); /*+1 for '\0'*/
    if (new_err->name_err == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for error message\n");
        free(new_err);
        exit(1);
    }

    /* Copy the message */
    strcpy(new_err->name_err, err_msg);

    /* Store line number */
    new_err->line = line;

    /* Initialize next pointer */
    new_err->next = NULL;

    /* Insert into the linked list */
    if (all_err == NULL) {
        all_err = new_err;
    } else {
        errors* temp = all_err;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_err;
    }
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
    return -1;
}

/*this function gets a position in an assembly file and gets its name and code and put it in the AVL tree*/
char* get_macro_code(FILE* asm_file)
{
    char *macro_code, buffer[MAX_IN_LINE], end[] = "mcroend";
    unsigned int position = ftell(asm_file);
    int count_letters = 0;

    buffer[0] = '\0';
    while(strcmp(buffer, end) != 0)
    {
        if(fgets(buffer, MAX_IN_LINE, asm_file) == NULL)
        {
            fprintf(stderr, "ERR: macro did not end or fgets() failed");
            return NULL;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        if(buffer[0] != ';' && strcmp(buffer, end) != 0)
        {
            count_letters += strlen(buffer);
        }
    }
    fseek(asm_file, position, SEEK_SET);

    macro_code = (char*)malloc(count_letters*sizeof(char)+1);
    if (!macro_code)
    {
        fprintf(stderr, "Error: failed to allocate the memory\n");
        exit(1);
    }
    macro_code[0] = '\0';

    buffer[0] = '\0';
    while(strcmp(buffer, end) != 0)
    {
        if(fgets(buffer, 83, asm_file) == NULL)
        {
            fprintf(stderr, "ERR: macro did not end or fgets() failed");
            return NULL;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        if(buffer[0] != ';' && strcmp(buffer, end) != 0)
        {
            strcat(macro_code, buffer);
        }
    }
    return macro_code;
}

