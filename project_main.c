#include "pre_asembler.h"
#include "phase_1.h"
#include "inputs_1.h"
#include "phase_2.h"
#include "code_to_binary.h"
#include "create_obj_file.h"

void print_err(void);

int main(int argc, char* argv[])
{
    /*start pre asembler phase*/
    state* phase_1_data = NULL;
    data_collect_phase_2* info = NULL;
    FILE *file_asm, *temp;
    char* save, file_name[MAX_IN_LINE+1], buffer[MAX_IN_LINE+1];
    if (argc < 2) {
        fprintf(stderr, "did not get any assembly file");
        return 1;
    }

    strcpy(file_name, argv[1]);
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
    /*end of pre asembler*/
    strcat(file_name, ".am");
    file_asm = fopen(file_name, "r");
    if(!file_asm)
    {
        fprintf(stdout, "err opening %s file", argv[1]);
        exit(1);
    }
    /*start phase 1*/
    fprintf(stdout, "starting phase 1\n");
    phase_1_data = read_file1(file_asm, file_name);
    fclose(file_asm);
    /*starting phase 2*/
    fprintf(stdout, "starting phase 2\n");
    strcpy(file_name, argv[1]);
    save = file_name;
    while(*save != '\0' && *save != '.') save++;
    *save = '\0';
    strcat(file_name, ".am");
    file_asm = fopen(file_name, "r");
    if(!file_asm)
    {
        fprintf(stdout, "err opening %s file", argv[1]);
        exit(1);
    }
    info = phase_2(file_asm, file_name, phase_1_data);
    fclose(file_asm);
    /*erasing ext and ent if empty*/
    strcpy(file_name, argv[1]);
    save = file_name;
    while(*save != '\0' && *save != '.') save++;
    *save = '\0';
    strcat(file_name, ".ext");
    temp = fopen(file_name, "r");

    if(fgets(buffer, MAX_IN_LINE, temp) == NULL)
    {
        remove(file_name);
    }
    else
    {
        save = buffer;
        while(*save != '\0' && isspace(*save)) save++;
        if(*save == '\0')
        {
            remove(file_name);
        }
    }
    /*checks entry now*/
    save = file_name;
    while(*save != '\0' && *save != '.') save++;
    *save = '\0';
    strcat(file_name, ".ent");
    temp = fopen(file_name, "r");
    if(!temp)
    {
        /*do nothing*/
    }
    if(fgets(buffer, MAX_IN_LINE, temp) == NULL)
    {
        remove(file_name);
    }
    else
    {
        save = buffer;
        while(*save != '\0' && isspace(*save)) save++;
        if(*save == '\0')
        {
            remove(file_name);
            save = file_name;
            while(*save != '\0' && *save != '.') save++;
            *save = '\0';
            strcat(file_name, ".am");
            add_error("entry label defined and not used ", 0, file_name);
        }
    }
    if(all_err != NULL)
    {
        print_err();
        save = file_name;
        while(*save != '\0' && *save != '.') save++;
        *save = '\0';
        strcat(file_name, ".ent");
        if (remove(file_name) == 0) /*remove write file that opened before*/
        {
            printf("entry file deleted successfully\n");
        }
        else
        {
            perror("Error deleting file");
        }
        save = file_name;
        while(*save != '\0' && *save != '.') save++;
        *save = '\0';
        strcat(file_name, ".ext");
         if (remove(file_name) == 0) /*remove write file that opened before*/
        {
            printf("extern file deleted successfully\n");
        }
        else
        {
            perror("Error deleting file");
        }
        fprintf(stdout, "stop at phase 2\n exit program...\n");
        free_dc_list(&info->dc_values);
        free_ic_list(&info->ic_value);
        return 1;
    }
    /*start making obj file*/

    save = file_name;
    while(*save != '.') save++;
    *save = '\0';
    strcat(file_name, ".obj");
    create_obj(info, file_name);
    free_labels_list(&phase_1_data->label);
    free_dc_list(&info->dc_values);
    free_ic_list(&info->ic_value);
    free(info);
    free(phase_1_data);
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

