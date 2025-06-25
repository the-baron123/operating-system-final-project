/*in this file we are going to use structs (AVL tree) to get all of the macros and save their code!*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/*creating an array of all the operation in the language*/
char *oparations[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

/*from here we will print all of ours errors and if there are none we will */
typedef struct errors
{
    char* name_err;
    struct errors* next;
} errors;

errors* all_err;
/*the struct build for the AVL tree to save the macro names and their code*/
typedef struct data
{
    char* name;
    char* code;
    struct data* low;
    struct data* high;
    int height;
} data;

/*this function adds an errors to the errors list! and if the linked list dose not exist it create it*/
void add_error(const char* err_msg)
{
    errors* new_err = (errors*)malloc(sizeof(errors));
    if (!new_err)
    {
        fprintf(stderr, "Error: failed to allocate memory for error struct\n");
        exit(1);
    }

    new_err->name_err = (char*)malloc(strlen(err_msg) + 1);
    if (!new_err->name_err)
    {
        fprintf(stderr, "Error: failed to allocate memory for error message\n");
        free(new_err);
        exit(1);
    }

    strcpy(new_err->name_err, err_msg);
    new_err->next = NULL;

    if (all_err == NULL)
    {
        all_err = new_err;
    }
    else
    {
        errors* temp = all_err;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = new_err;
    }
}

/*returns 1 if 'name' is a valid name of operation and returns -1 if 'name' is not a valid name of oparation*/
int check_if_operation_in_language(const char* name)
{
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

/*creating a new tree*/
data* create_node(const char* name, const char* code)
{
    data* n = (data*)malloc(sizeof(data));
    n->code = (char*)malloc(sizeof(char)*strlen(code)+1);
    if(!n->code)
    {
        printf("Error: failed to allocate the memory\n");
        free(n);
        exit(1);
    }
    n->name = (char*)malloc(sizeof(char)*strlen(name)+1);
    if(!n->name)
    {
        printf("Error: failed to allocate the memory\n");
        free(n);
        exit(1);
    }
    strcpy(n->code ,code);
    n->height = 1;/*tree height starting from 1 and not a 0*/
    n->high = NULL;
    n->low = NULL;
    strcpy(n->name, name);
    return n;
}

/*returns the hight of specific node in the tree*/
int get_height(data* n)
{
    if (n == NULL) return 0;
    return n->height;
}

/*getting the balance of specific node in the tree*/
int get_balance(data* n)
{
    if (n == NULL) return 0;
    return get_height(n->low) - get_height(n->high);
}

/*rotating right the tree*/
data* rotate_right(data* prev_root)
{
    data* new_root = prev_root->low;
    data* temp = new_root->high;
    /*making the rotate*/
    new_root->high = prev_root;
    prev_root->low = temp;

    /*updating the height to max of sons+1*/
    prev_root->height = MAX(get_height(prev_root->low), get_height(prev_root->high)) + 1;
    new_root->height = MAX(get_height(new_root->low), get_height(new_root->high)) + 1;

    return new_root;
}

/*rotating left the tree*/
data* rotate_left(data* prev_root)
{
    data* new_root = prev_root->high;
    data* temp = new_root->low;

    /*making the rotate*/
    new_root->low = prev_root;
    prev_root->high = temp;

    /*updating the height to max of sons+1*/
    prev_root->height = MAX(get_height(prev_root->low), get_height(prev_root->high)) + 1;
    new_root->height = MAX(get_height(new_root->low), get_height(new_root->high)) + 1;

    return new_root;
}

/*inserting a node to the AVL tree*/
data* insert_avl(data* root, const char* name, const char* code)
{
    int cmp = 0, balance = 0;
    /*normal inserting to tree*/
    if (root == NULL)
    {
        return create_node(name, code);
    }
    cmp = strcmp(name, root->name);
    if (cmp < 0)
    {
        root->low = insert_avl(root->low, name, code);
    }
    else if (cmp > 0)
    {
        root->high = insert_avl(root->high, name, code);
    }

    else
    {
        add_error("ERR: macro appeared twice");
        return NULL;/*if already appeared in the tree we will report the err*/
    }

    /*updating the height*/
    root->height = 1 + MAX(get_height(root->low), get_height(root->high));

    /*checking if the tree is balanced*/
    balance = get_balance(root);

    /*using our 4 cases*/

    /*1) LL rotate to the left twice*/
    if (balance > 1 && strcmp(name, root->low->name) < 0)
    {
        return rotate_right(root);
    }
    /*2) RR rotate to the right twice*/
    if (balance < -1 && strcmp(name, root->high->name) > 0)
    {
        return rotate_left(root);
    }
    /*3) LR rotate to the left and then to the right*/
    if (balance > 1 && strcmp(name, root->low->name) > 0)
    {
        root->low = rotate_left(root->low);
        return rotate_right(root);
    }
    /*4) RL rotate to the right and then to the left*/
    if (balance < -1 && strcmp(name, root->high->name) < 0)
    {
        root->high = rotate_right(root->high);
        return rotate_left(root);
    }

    return root;
}
/*this function gets a position in an assembly file and gets its name and code and put it in the AVL tree*/
char* get_macro_code(FILE* asm_file, data* root)
{
    char *macro_code, buffer[83], end[] = "mcroend";
    unsigned int position = ftell(asm_file);
    int count_letters = 0;

    buffer[0] = '\0';
    while(strcmp(buffer, end) != 0)
    {
        if(fgets(buffer, 83, asm_file) == NULL)
        {
            add_error("ERR: macro did not end or fgets() failed");
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
            add_error("ERR: macro did not end or fgets() failed");
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

