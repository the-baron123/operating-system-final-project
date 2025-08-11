#include "pre_asembler.h"
#include "phase_1.h"

void print_err(void);

int main(int argc, char* argv[])
{
    /*start pre asembler phase*/
    FILE *file_asm;
    if (argc < 2) {
        fprintf(stderr, "did not get any assembly file");
        return 1;
    }

    file_asm = get_all_macros(NULL, NULL, argc, argv);
    if (remove("temp.am") == 0) /*remove write file that opened before*/
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

    /*end of pre asembler and the start of phase 1*/
    strcat(argv[1], ".am");
    file_asm = fopen(argv[1], "r");
    /*start reading the file*/

    return 0;
}

/*A function that print all of the errs*/
void print_err()
{
    while(all_err->next != NULL)
    {
        fprintf(stdout, "%s \033[1;31m in line\033[0m %d\n", all_err->name_err, all_err->line);
        all_err = all_err->next;
    }
    fprintf(stderr, "%s \033[1;31m in line\033[0m %d\n", all_err->name_err, all_err->line);
}

