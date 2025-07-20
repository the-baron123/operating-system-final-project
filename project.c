/*in this file we are going to get an ".am" file(assembly file) and we are going to wrote the ".am" file as it is bate replace all macros in their code lines*/
#include "read_macro.h"

void print_err(void);

int main(int argc, char* argv[])
{
    FILE *file_asm;
    if (argc < 2) {
        fprintf(stderr, "did not get any assembly file");
        return 1;
    }

    file_asm = get_all_macros(NULL, NULL, argc, argv);
    if (remove("temp.asm") == 0) /*remove write file that opened before*/
    {
        printf("File deleted successfully\n");
    }
    else
    {
        perror("Error deleting file");
    }

    fclose(file_asm);
    if(all_err != NULL)
    {
        print_err();
        fprintf(stdout, "***** the output file that the \"pre asembler\" make is only on the files that has no macro error in them! *****");
        return 1;
    }

    return 0;
}

/*A function that print all of the errs*/
void print_err()
{
    while(all_err->next != NULL)
    {
        fprintf(stderr, "%s \033[1;31m in line\033[0m %d\n", all_err->name_err, all_err->line);
        all_err = all_err->next;
    }
    fprintf(stderr, "%s \033[1;31m in line\033[0m %d\n", all_err->name_err, all_err->line);
}

