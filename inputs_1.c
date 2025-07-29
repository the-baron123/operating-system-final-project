#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct mat
{
    char* name;
    int** nums;
    int row;
    int colom;
    struct mat* next;
}mat;
/*function to add a node into the mat linked list*/
int add_mat(mat** head, mat* node_add, int line, char* file)
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
        /*checks for a matrix with that name*/
        if (strcmp(current->name, node_add->name) == 0) {
            fprintf(stdout, "Matrix with name \"%s\" already exists (line %d) in file %s.\n", node_add->name, line, file);
            return 0;
        }
        current = current->next;
    }

    /*checking the last node*/
    if (strcmp(current->name, node_add->name) == 0) {
        fprintf(stderr, "Matrix with name \"%s\" already exists (line %d) in file %s.\n", node_add->name, line, file);
        return 0;
    }

    /*linking the node to the list*/
    current->next = node_add;
    node_add->next = NULL;

    return 1;/*returning head of the list*/
}
/*gets a string that should be a number(or just regular string and convert it back into a number*/
int get_num(char* num, int line, char* name)
{
    char* save;
    int number = 0;

    while(isspace(*num)) num++;/*removing white chars*/
    num = strtok(num, " ");/*getting the number*/
    save = num;
    while(!isspace(*save) && *save != ',' && *save != '\n')/*searching for illegal chars in number*/
    {
        if(!isdigit(*save) && *save != '-' && *save != '+')
        {
            fprintf(stdout, "illegal text in number (line %d) in file %s.\n",line, name);
            return 1 << 11;
        }
        save++;
    }
    if(*save != ',' && strtok(NULL, ",") == NULL)/*searching for illegal chars after num*/
    {
        while(*save != '\0')
        {
            if(!isspace(*save) && *save != '\n')
            {
                fprintf(stdout, "missing comma\illegal char after number (line %d) in file %s.\n",line, name);
                return 1 << 11;
            }
            save++;
        }
    }
    number = atoi(num);/*getting the number with atoi*/
    return number;
}

int* get_str(char* str, int line, char* name)
{
    int *res, *p_res;

    while(isspace(*str)) str++;/*skipping all white chars from the start just to be sure*/
    if(*str != '"')
    {
        fprintf(stdout, "string missing \" (line %d) in file %s\n", line, name);
        return NULL;
    }
    str = strtok(str, "\"");/*getting the string*/
    if(str == NULL)
    {
        fprintf(stdout, "string missing \" (line %d) in file %s\n", line, name);
        return NULL;
    }
    res = (int*)malloc((strlen(str)+1)* sizeof(int));/*allocating memory for string length + '\0'*/
    p_res = res;/*setting a pointer that I can move without worrying to lose my position */
    while(*str != '\0')
    {
        if(!isprint(*str))/*checking if the char is printable*/
        {
            free(res);
            fprintf(stdout, "string contains a non printable char (line %d) in file %s\n", line, name);
            return NULL;
        }
        /*setting the array*/
        *p_res = (int)*str;
        p_res++;
        str++;
    }
    *p_res = 0;
    if(strtok(str, " \n") != NULL)
    {
        free(res);
        fprintf(stdout, "illegal char after string(should be no chars at all or only white char) (line %d) in file %s\n", line, name);
        return NULL;
    }
    return res;
}

int get_integer_mat(char* mat, int line, char* name)
{
    int res = -1, count_num = 0;
    char* integer, *save;

    while(isspace(*mat) && *mat != '\0') mat++;/*ignoring white chars*/
    if(*mat == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        return res;
    }
    if(*mat == ']')
    {
        fprintf(stdout, "missing integer (line %d), in file %s\n", line, name);
        return res;
    }
    if(!isdigit(*mat))
    {
        fprintf(stdout, "illegal char(should be an integer) (line %d), in file %s\n", line, name);
        return res;
    }
    /*getting the number*/
    save = mat;
    while(isdigit(*save))
    {
        count_num++;
        save++;
    }
    if(*save == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        return res;
    }
    if(isalpha(*save))
    {
        fprintf(stdout, "illegal char (line %d), in file %s\n", line, name);
        return res;
    }
    integer = (char*)malloc((count_num+1)* sizeof(char));
    if(integer == NULL)
    {
        fprintf(stdout, "allocating memory error\n", line, name);
        return res;
    }
    while(isdigit(*mat))
    {
        *integer = *mat;
        integer++;
        mat++;
    }
    res = atoi(integer);

    while(isspace(*mat)) mat++;
    if(*mat == ']')
    {
        return res;/*returning the number*/
    }
    if(*save == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        return -1;
    }
    if(isalpha(*save))
    {
        fprintf(stdout, "illegal char (line %d), in file %s\n", line, name);
        return -1;
    }
    fprintf(stdout, "missing parameters\illegal char (line %d), in file %s\n", line, name);
    return -1;
}

mat* get_mat(char* mat_info, int line, char* name)
{
    mat* new_mat;
    char* save, *mat_name;
    int count_mat = 0, row = 0, colom = 0, **nums, i, j, integer;

    new_mat = (mat*)malloc(sizeof(mat));
    if (new_mat == NULL)
    {
        fprintf(stdout, "memory allocation error.\n");
        return NULL;
    }

    while(isspace(*mat_info) && *mat_info != '\0') mat_info++;
    if(*mat_info == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        return NULL;
    }
    save = mat_info;
    /*getting the name of the matrix*/
    while(*save != '\0' && !isspace(*save) && isalpha(*save))
    {
        count_mat++;
        save++;
    }
    if(*save == '\0' || count_mat == 0)
    {
        fprintf(stdout, "missing name\parameters (line %d), in file %s\n", line, name);
        return NULL;
    }
    if(isspace(*save))
    {
        fprintf(stdout, "illegal white char (line %d), in file %s\n", line, name);
        return NULL;
    }
    if(*save != '[')
    {
        fprintf(stdout, "illegal char after mat name(line %d), in file %s\n", line, name);
        return NULL;
    }
    mat_name = (char*)malloc((count_mat+1) * sizeof(char));
    if(mat_name == NULL)
    {
        fprintf(stdout, "allocating memory error\n");
        return NULL;
    }
    /*getting the name using save as pointer*/
    save = mat_name;
    while(*mat_info != '[')
    {
        *save = *mat_info;
        save++;
        mat_info++;
    }
    *save = '\0';
    new_mat->name = mat_name;

    /*getting the size of the matrix*/
    row = get_integer_mat(mat_info, line, name);
    if(row == -1)
    {
        /*the error were print in the function*/
        free(mat_name);
        return NULL;
    }
    while(*mat_info != ']')/*we checked in the function that that char exist*/
    {
        mat_info++;
    }
    mat_info++;
    if(*mat_info == '\0')
    {
        fprintf(stdout, "missing parameters (line %d), in file %s\n", line, name);
        free(mat_name);
        return NULL;
    }
    if(isspace(*save))
    {
        fprintf(stdout, "illegal white char (line %d), in file %s\n", line, name);
        free(mat_name);
        return NULL;
    }
    if(*save != '[')
    {
        fprintf(stdout, "illegal char after mat name(line %d), in file %s\n", line, name);
        free(mat_name);
        return NULL;
    }
    colom = get_integer_mat(mat_info, line, name);
    if(colom == -1)
    {
        /*the error were print in the function*/
        free(mat_name);
        return NULL;
    }
    /*allocating memory for the array and setting it for 0*/
    nums = (int**)malloc(row* sizeof(int*));
    if (nums == NULL) {
        fprintf(stdout, "Memory allocation failed for rows.\n");
        free(mat_name);
        return NULL;
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
            free(mat_name);
            return NULL;
        }
    }
    /*getting the numbers of the mat*/
    for(i = 0;i<row;i++)
    {
        for(j = 0;j<colom;j++)
        {
            if((save = strtok(mat_info, ",")) != NULL)
            {
                integer = get_num(mat_info, line, name);
                if(integer == 1 << 11)
                {
                    for(i = 0;i<row;i++)
                    {
                        free(nums[i]);
                    }
                    free(nums);
                    free(mat_name);
                    return NULL;
                }
                else
                {
                    nums[i][j] = integer;
                }
            }
            else
            {
                integer = get_num(mat_info, line, name);
                if(integer == 1 << 11)
                {
                    for(i = 0;i<row;i++)
                    {
                        free(nums[i]);
                    }
                    free(nums);
                    free(mat_name);
                    return NULL;
                }
                new_mat->nums = nums;
                new_mat->colom = colom;
                new_mat->row = row;
                return new_mat;
            }
        }
    }
    if(save != NULL)
    {
        fprintf(stdout, "to much inputs (line %d) in file %s.\n", line, name);
        for(i = 0;i<row;i++)
        {
            free(nums[i]);
        }
        free(nums);
        free(mat_name);
        return NULL;
    }
    new_mat->nums = nums;
     new_mat->colom = colom;
     new_mat->row = row;
    return new_mat;
}
