/*in this file we are going to get an ".am" file(assembly file) and we are going to wrote the ".am" file as it is bate replace all macros in their code lines*/
#include "read_macro.h"


int main(int argc, char* argv[])
{
    FILE *pre;
    FILE *file_asm;

    if (argc < 2) {
        fprintf(stderr, "did not get any assembly file");
        return 1;
    }

    pre = fopen("separation.asm", "w");
    if (pre == NULL) {
        fprintf(stderr, "Error opening output file");
        return 1;
    }

    file_asm = fopen(argv[1], "r");
    if (file_asm == NULL) {
        perror("Error opening input asm file");
        fclose(pre);
        return 1;
    }

    fclose(file_asm);
    fclose(pre);
    return 0;
}


