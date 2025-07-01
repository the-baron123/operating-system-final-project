/*in this file we are going to get an ".am" file(assembly file) and we are going to wrote the ".am" file as it is bate replace all macros in their code lines*/
#include "read_macro.h"

#define START_MCRO "mcro"

int macro_name_valid(char*, int);

void print_err(void);
/*getting all macros from all of the file*/
FILE* get_all_macros(FILE*, FILE*, int, char**);

void copy_file(FILE* source, FILE* dest);

int main(int argc, char* argv[])
{
    FILE *file_asm;
    if (argc < 2) {
        fprintf(stderr, "did not get any assembly file");
        return 1;
    }

    file_asm = get_all_macros(NULL, NULL, argc, argv);

    fclose(file_asm);
    return 0;
}

/*copping source file to the destination file*/
void copy_file(FILE* source, FILE* dest)
{
    int len = 0;
    char buffer[MAX_IN_LINE], *word;
    char line_copy[MAX_IN_LINE];
    data* mcro;
    while(fgets(buffer, MAX_IN_LINE, source) != NULL)
    {
        strcpy(line_copy, buffer);
        word = strtok(line_copy, " \t\n");
        if (word == NULL)/*empty line*/
        {
            fputs(buffer, dest);
            continue;
        }
        if(strtok(NULL, " \t\n") != NULL)/*not a mcro*/
        {
            fputs(buffer, dest);
        }
        else/*might be a mcro*/
        {
            len = strlen(buffer);
            if (len > 0 && buffer[len-1] != '\n') {
                buffer[len] = '\n';
                buffer[len+1] = '\0';
            }
            mcro = find_node(root, strtok(buffer, " "));
            if(mcro == NULL)
            {
                fputs(buffer, dest);
            }
            else
            {
                fputs(mcro->code, dest);
            }
        }
    }
}



/*checks if the name is valid*/
int macro_name_valid(char* word, int line)
{
    /*creating an array of all the operation in the language*/
    char *oparations[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int i = 0;

    for(i = 0; i < 16;i++)
    {
        if(strcmp(word, oparations[i]) == 0)
        {
            add_error("ERR: mcro name is operation name(invalid name)", line);
            return 0;
        }
    }
    return 1;
}
/*A function that print all of the errs*/
void print_err()
{
    while(all_err->next != NULL)
    {
        fprintf(stderr, all_err->name_err);
        all_err = all_err->next;
    }
    fprintf(stderr, all_err->name_err);
}

FILE* get_all_macros(FILE* curr_read, FILE* curr_write, int argc, char** argv)
{
    char buffer[MAX_IN_LINE], *word, *macro_name, *macro_code;
    int check_name_validation = 0, count_line = 0, num_file = 1;
    FILE* res = fopen("pre.asm", "w");
    if (res == NULL)
    {
        fprintf(stderr, "Failed to open pre.asm for writing");
        return NULL;
    }

    for(num_file = 1; num_file < argc; num_file++)/*start reading the files and writing the files into in to file*/
    {
        count_line = 0;
        curr_write = fopen("temp.asm", "w");/*opening the writing file(for the current file)*/
        if (curr_write == NULL) {
            fprintf(stderr, "Error opening output file");
            return NULL;
        }
        curr_read = fopen(argv[num_file], "r");/*opening the source file*/
        if (curr_read == NULL) {
            fprintf(stderr, "Error opening output file");
            return NULL;
        }
        buffer[0] = '\0';/*setting the buffer*/
        while(fgets(buffer, MAX_IN_LINE, curr_read) != NULL)/*start getting all of the macros from the current files*/
        {
            count_line++;
            if(buffer[strlen(buffer)-1] != '\n')/*checking if the line in the file is too long*/
            {
                add_error("ERR: too much chars in line", count_line);
            }
            if(buffer[0] != ';')/*if the line is not a comment*/
            {
                word = strtok(buffer, " ");/*getting the first word from the buffer*/
                if(strcmp(word, START_MCRO) == 0)/*if the first word is start of macro collect it*/
                {
                    word = strtok(NULL, " ");/*getting the name of macro*/
                    check_name_validation = macro_name_valid(word, count_line);/*check if name is valid*/
                    if(check_name_validation == 1)
                    {
                        macro_name = (char*)malloc(strlen(word)*sizeof(char)+1);/*allocate memory*/
                        if(macro_name == NULL)
                        {
                            fprintf(stderr, "ERR: memory allocate failed");
                            return NULL;
                        }
                        strcpy(macro_name, word);/*copying the name of macro to macro_name*/
                        word = strtok(NULL, " ");/*getting the next word*/
                        if(word != NULL)/*if there is another word*/
                        {
                            free(macro_name);
                            add_error("ERR: illegal text after macro name", count_line);
                            continue;
                        }
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
                    else
                    {
                        add_error("ERR: mcro name is operation name(invalid name)", count_line);
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
        curr_write = fopen("temp.asm", "r");/*open it as read file*/
        if (!curr_write)
        {
            fprintf(stderr, "ERR: failed to reopen temp.asm for reading\n");
            return NULL;
        }
        copy_file(curr_write, res);/*copping the file*/
        remove("temp.asm");/*close write file*/
        fclose(curr_read);/*close read file*/
    }
    return res;
}

