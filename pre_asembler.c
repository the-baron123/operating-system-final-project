/*in this file we are going to use structs (linked list) to get all of the macros and save their code!*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define START_MCRO "mcro"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MAX_IN_LINE 81 /*80 letters + '\n' + '\0' = 82*/

#define AM_EXTENSION ".am"
#define AS_EXTENSION ".as"

/*from here we will print all of ours errors and if there are none we will */
typedef struct errors
{
    char* name_err;
    int line;
    struct errors* next;
} errors;
/*collect errors for the pre asembler phase*/
errors* pre_err;
/*the struct build for the linked list to save the macro names and their code*/
typedef struct data
{
    char* name;
    char* code;
    struct data* next;
} data;

data* root = NULL;

/*this function adds an errors to the errors list! and if the linked list dose not exist it create it*/
void add_error(const char* err_msg, int line, char* name_file) {
    errors* new_err;

    /* Allocate memory for new error node */
    new_err = (errors*)malloc(sizeof(errors));
    if (new_err == NULL) {
        fprintf(stdout, "Error: failed to allocate memory for error struct\n");
        exit(1);
    }

    /* Allocate memory for the error message */
    new_err->name_err = (char*)malloc(strlen(err_msg)+strlen(name_file) + 1); /*+1 for '\0'*/
    if (new_err->name_err == NULL) {
        fprintf(stdout, "Error: failed to allocate memory for error message\n");
        free(new_err);
        exit(1);
    }

    /* Copy the message */
    strcpy(new_err->name_err, err_msg);
    /*adding the file name*/
    strcat(new_err->name_err, name_file);

    /* Store line number */
    new_err->line = line;

    /* Initialize next pointer */
    new_err->next = NULL;

    /* Insert into the linked list */
    if (pre_err == NULL)
    {
        pre_err = new_err;
    }
    else
    {
        errors* temp = pre_err;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_err;
    }
}


/*creating a node*/
data* create_node(const char* name, const char* code) {
    data* new_node = (data*)malloc(sizeof(data));
    if (new_node == NULL) {
        fprintf(stdout, "Memory allocation failed for node\n");
        exit(1);
    }

    new_node->name = (char*)malloc(strlen(name) + 1);
    if (new_node->name == NULL) {
        fprintf(stdout, "Memory allocation failed for name\n");
        free(new_node);
        exit(1);
    }
    strcpy(new_node->name, name);

    new_node->code = (char*)malloc(strlen(code) + 1);
    if (new_node->code == NULL) {
        fprintf(stdout, "Memory allocation failed for code\n");
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

/*copping source file to the destination file*/
void copy_file(FILE* source, FILE* dest)
{
    int len = 0;
    char buffer[MAX_IN_LINE], *word;
    char line_copy[MAX_IN_LINE], *save;
    data* mcro;
    while(fgets(buffer, MAX_IN_LINE, source) != NULL)
    {
        len = strlen(buffer);
        if (buffer[len-1] != '\n')/*checks if the line ended with \n*/
        {
            buffer[len] = '\n';
            buffer[len+1] = '\0';
            fprintf(stdout, "%s", buffer);
        }
        strcpy(line_copy, buffer);
        save = line_copy;/*save line_copy place before possible changes*/
        while(*save != '\0' && isspace(*save)) save++;/*ignore all white spaces*/
        word = strtok(save, " ");/*word is the first word in line_copy(buffer)*/
        if (word == NULL)/*empty line*/
        {
            continue;/*do nothing*/
        }
        else if(strtok(NULL, " ") != NULL)/*not a mcro because it have more than 2 words in it*/
        {
            fputs(buffer, dest);
        }
        else/*might be a mcro*/
        {
            save = buffer;
            while(isspace(*save)) save++;/*ignore all white chars*/
            word = save;
            mcro = find_node(root, word);
            if(mcro == NULL)
            {
                fputs(word, dest);
            }
            else
            {
                fputs(mcro->code, dest);
            }
        }
    }
}
/*returns 1 if 'name' is a valid name of operation and returns 0 if 'name' is not a valid name of oparation*/
int macro_name_valid(char* word, int line, int argc, char** argv)
{
    /*creating an array of all the operation in the language*/
    char* oparations[] = {"data\n",".data\n","string\n",".string\n","mat\n",".mat\n","mov\n", "cmp\n", "add\n", "sub\n", "not\n", "clr\n", "lea\n", "inc\n", "dec\n", "jmp\n", "bne\n", "red\n", "prn\n", "jsr\n", "rts\n", "stop\n"};
    char* save_word = word;
    int i = 0;

    if(isdigit(*save_word))/*checking first letter*/
    {
        add_error("ERR: mcro cannot start with a number(invalid name) in file name: ", line, argv[argc]);
        return 0;
    }
    /*going over the string to search for illegal char in name*/
    while(*save_word != '\0' && (isalpha(*save_word) || isdigit(*save_word))) save_word++;
    if(!(isalpha(*save_word) || isdigit(*save_word)))
    {
        add_error("ERR: mcro name have illegal char in it(invalid name) in file name: ", line, argv[argc]);
        return 0;
    }
    /*check if the name is not a saved word*/
    for(i = 0; i < 22;i++)
    {
        if(strcmp(word, oparations[i]) == 0)
        {
            add_error("ERR: mcro name is operation name(invalid name) in file name: ", line, argv[argc]);
            return 0;
        }
    }
    return 1;
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
            fprintf(stdout, "ERR: macro did not end or fgets() failed");
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
        fprintf(stdout, "Error: failed to allocate the memory\n");
        exit(1);
    }
    macro_code[0] = '\0';

    buffer[0] = '\0';
    while(strcmp(buffer, end) != 0)
    {
        if(fgets(buffer, MAX_IN_LINE, asm_file) == NULL)
        {
            fprintf(stdout, "ERR: macro did not end or fgets() failed");
            return NULL;
        }

        if(buffer[0] != ';' && strcmp(buffer, end) != 0)
        {
            strcat(macro_code, buffer);/*copy the line*/
        }
    }
    return macro_code;
}


FILE* get_all_macros(FILE* curr_read, FILE* curr_write, int argc, char** argv)
{
    char buffer[MAX_IN_LINE], *curr_read_name, *word, *macro_name, *macro_code, *save;
    int check_name_validation = 0, count_line = 0, num_file = 1, len = 0;
    char* res_name;
    FILE* res;

    res_name = (char*)malloc(strlen(argv[1])+4);/*name of the file +".am"+'\0'*/
    if(!res_name)
    {
        fprintf(stdout, "ERR: memory allocate failed");
        exit(1);
    }
    strcpy(res_name, argv[1]);
    strcat(res_name, ".am");
    res = fopen(res_name, "w");
    if (res == NULL)
    {
        fprintf(stdout, "Failed to open pre.asm for writing");
        exit(1);
    }
    buffer[0] = '\0';/*setting the buffer*/
    for(num_file = 1; num_file < argc; num_file++)/*start reading the files and writing the files into in to file*/
    {
        count_line = 0;
        curr_write = fopen("temp.am", "w");/*opening the writing file(for the current file)*/
        if (curr_write == NULL) {
            fprintf(stdout, "Error opening output file");
            exit(1);
        }
        strcat(argv[num_file], ".as");
        curr_read = fopen(argv[num_file], "r");/*opening the source file*/
        if (curr_read == NULL) {
            fprintf(stdout, "Error opening output file");
            exit(1);
        }

        while(fgets(buffer, MAX_IN_LINE, curr_read) != NULL)/*start getting all of the macros and code from the current files*/
        {
            len = strlen(buffer);
            if (buffer[len-1] != '\n')/*checks if the line ended with \n if not fix it*/
            {
                buffer[len] = '\n';
                buffer[len+1] = '\0';
            }
            save = buffer;
            count_line++;
            if(strlen(buffer) == MAX_IN_LINE - 1 && (buffer[strlen(buffer)-1] != '\n'))/*checking if the line in the file is too long*/
            {
                add_error("ERR: too much chars in line in file name: ", count_line, argv[num_file]);
            }
            if(buffer[0] != ';')/*if the line is not a comment*/
            {
                word = strtok(save, " ");/*getting the first word from the buffer*/
                if(strcmp(word, START_MCRO) == 0)/*if the first word is start of macro collect it*/
                {
                    word = strtok(NULL, " ");/*getting the name of macro*/
                    if(strtok(NULL, " ") != NULL)/*checking if line has illegal text*/
                    {
                        add_error("ERR: illegal text after macro name in file name: ", count_line, argv[num_file]);
                        continue;
                    }
                    save = word;
                    while(!isspace(*save)) save++;
                    if(*save != '\n')
                    {
                        *save = '\n';
                        save++;
                        *save = '\0';
                    }
                    check_name_validation = macro_name_valid(word, count_line, num_file, argv);/*check if mcro name is valid*/
                    if(check_name_validation == 1)
                    {
                        macro_name = (char*)malloc(strlen(word)*sizeof(char)+1);/*allocate memory*/
                        if(macro_name == NULL)
                        {
                            fprintf(stdout, "ERR: memory allocate failed");
                            exit(1);
                        }
                        strcpy(macro_name, word);/*copying the name of macro to macro_name*/

                        macro_code = get_macro_code(curr_read);/*getting the code of the macro*/
                        if(root == NULL)/*if our tree of macros is empty*/
                        {
                            root = create_node(macro_name, macro_code);
                        }
                        else
                        {
                            add_node(&root, macro_name, macro_code);
                        }
                        free(macro_name);
                    }
                    else/*illegal macro name*/
                    {
                        fclose(curr_write);/*close write file*/
                        fclose(curr_read);/*close read file*/
                        continue;
                    }
                }
                else
                {
                    while(word != NULL)/*if line is not a comment and not a mcro just put it in the curr_write file*/
                    {
                        fputs(word, curr_write);/*puts the word in the curr_write file*/
                        fputc(' ', curr_write);/*adding whit space*/
                        word = strtok(NULL, " ");/*getting next word*/
                    }
                }
            }
        }
        fputc('\n', curr_write);
        fclose(curr_write);/*closing the file*/
        curr_write = fopen("temp.am", "r");/*open it as read file*/
        if (!curr_write)
        {
            fprintf(stdout, "ERR: failed to reopen temp.asm for reading\n");
            exit(1);
        }
        copy_file(curr_write, res);/*copping the file*/
        fclose(curr_read);/*close read file*/
    }
    return res;
}

