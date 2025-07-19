











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
