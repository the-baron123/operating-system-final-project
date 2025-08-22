#include "inputs_1.h"
#include "pre_asembler.h"

#define PHASE_2 2
#define START_COUNT 100

binary* get_mat_code(char* arg, int line, char* file_name, labels* labels_table, int* IC)
{
    short by_word = 0, reg_val;
    char *start, *end, *label_name, *copy, *reg;
    labels* search_label = NULL;
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    res->next = NULL;

    start = arg;
    while(isspace(*start)) start++;
    end = strchr(start, '[');
    label_name = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation error");
        exit(1);
    }
    copy = label_name;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';
    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        check_extern(label_name, labels_table, file_name, *IC++);/*if it's an extern label add a comment on the ext file*/
        /*set new word*/
        insert_bits(&res->word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&res->word, 0, 1, 2):insert_bits(&res->word, 0, 1, 1);
        by_word = 0;

        start = strchr(arg, '[');
        start++;
        end = strchr(start, ']');
        if(!end)
        {
            free(label_name);
            free_binary_list(&res);
            add_error("missing ']' ",line, file_name);
            return NULL;
        }
        if(*(end+1) != '[')
        {
            free(label_name);
            free_binary_list(&res);
            add_error("missing '[' or illegal space ",line, file_name);
            return NULL;
        }
        end--;
        reg = (char*)malloc(sizeof(char)*((end-start)+2));/*set register*/
        if(!reg)
        {
            fprintf(stdout, "memory allocation error");
            exit(1);
        }
        /*copping register*/
        copy = reg;
        while(end >= start)
        {
            *copy = *start;
            start++;
            copy++;
        }
        *copy = '\0';
        reg_val = check_if_register(reg, line, file_name);
        if(reg_val == -1)
        {
            free(label_name);
            free(reg);
            free_binary_list(&res);
            return NULL;
        }
        /*set bits 6-9 for first register*/
        insert_bits(&by_word, 6, 9, reg_val);
        start = strchr(start, '[');
        start++;
        end = strchr(start, ']');
        if(!end)
        {
            free(label_name);
            free(reg);
            free_binary_list(&res);
            add_error("missing ']' ",line, file_name);
            return NULL;
        }
        end--;
        free(reg);
        reg = (char*)malloc(sizeof(char)*((end-start)+2));/*set register*/
        if(!reg)
        {
            fprintf(stdout, "memory allocation error");
            exit(1);
        }
        /*copping register*/
        copy = reg;
        while(end >= start)
        {
            *copy = *start;
            start++;
            copy++;
        }
        *copy = '\0';
        reg_val = check_if_register(reg, line, file_name);
        if(reg_val == -1)
        {
            free(label_name);
            free(reg);
            free_binary_list(&res);
            return NULL;
        }
        /*set bits 2-5 for second register*/
        insert_bits(&by_word, 2, 5, reg_val);
        add_binary_node(&res, by_word);
        free(reg);
    }
    else
    {
        free_binary_list(&res);
        free(label_name);
        add_error("matrix name dose not exist ", line, file_name);
        return NULL;
    }
    free(label_name);
    end = strchr(arg, ']');
    end++;
    end = strchr(end, ']');
    end++;
    while(*end != '\0' && isspace(*end)) end++;
    if(*end != '\0')
    {
        add_error("illegal text after argument ", line, file_name);
        return NULL;
    }
    return res;
}

binary* get_reg_code(char* arg,int start_bit, int end_bit, int line, char* file_name)
{
    char *start, *end, *reg, *copy;
    short reg_val;
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    res->next = NULL;

    start = arg;
    end = start;
    while(*end != '\0') end++;
    reg = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!reg)
    {
        fprintf(stdout, "memory allocation fail ");
        exit(1);
    }
    copy = reg;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';
    reg_val = check_if_register(reg, line, file_name);
    if(reg_val == -1)
    {
        free(reg);
        free_binary_list(&res);
        free(arg);
        return NULL;
    }

    insert_bits(&res->word, start_bit, end_bit, reg_val);
    free(reg);
    return res;
}
/*this function gets an arguments for a word in the language and convert it to it's binary value and return it*/
/*source: 1,2,3 dest: 0,1,2,3*/
binary* code_for_zero(char* word, int line, char* file_name, int opcode, labels* labels_table, int IC)
{
    short by_word = 0, num, duble_reg = 0;
    char *start, *end, *label_name, *copy, *arg;
    labels* search_label = NULL;
    binary* res = (binary*)malloc(sizeof(binary)), *add;
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    res->next = NULL;
    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    IC++;

    end = strchr(word, ',');
    if(!end)
    {
        free_binary_list(&res);
        add_error("missing comma ",line, file_name);
        return NULL;
    }
    start = word;

    arg = (char*)malloc(sizeof(char)*(end-start+1));
    if(!arg)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    copy = arg;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';

    start = word;
    while(*start != '\0' && !isspace(*start) && *start != ',') start++;
    end = start;
    start = word;
    label_name = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation fail ");
        exit(1);
    }
    copy = label_name;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';

    /*start getting the source*/
    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        while(*end != '\0' && isspace(*end)) end++;
        if(*end != ',')
        {
            free(arg);
            free(label_name);
            free_binary_list(&res);
            add_error("illegal text after argument ", line, file_name);
            return NULL;
        }
        check_extern(label_name, labels_table, file_name, IC++);/*if it's an extern label add a comment on the ext file*/
        by_word = 0;
        /*set the bits*/
        insert_bits(&res->word, 4, 5, 1);
        insert_bits(&by_word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&by_word, 0, 1, 2):insert_bits(&by_word, 0, 1, 1);
        add_binary_node(&res, by_word);
    }
    free(label_name);

    if(strchr(arg, '[') != NULL && search_label == NULL)/*found a matrix*/
    {
        insert_bits(&res->word, 4, 5, 2);/*set source bits*/
        add = get_mat_code(arg, line, file_name, labels_table, &IC);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    else if(strchr(arg, '#') != NULL)
    {
        start = strchr(arg, '#');
        start++;
        if(isspace(*start))
        {
            free_binary_list(&res);
            free(arg);
            add_error("illegal space ", line, file_name);
            return NULL;
        }
        num = get_last_num(start, line, file_name);
        if(num == (1 << 11))
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        insert_bits(&by_word, 2, 10, num);
        add_binary_node(&res, by_word);
    }
    else if(search_label == NULL)/*should be a register*/
    {
        duble_reg++;
        insert_bits(&res->word, 4, 5, 3);/*set source bits*/
        add = get_reg_code(arg, START_SOURCE_REGISTER, END_SOURCE_REGISTER, line, file_name);
        res = add_binary_node_with_word(&res, add);
    }
    free(arg);
    /*start getting the destination*/
    start = strchr(word, ',');
    if(!start)
    {
        free_binary_list(&res);
        add_error("missing comma ",line ,file_name);
        return NULL;
    }
    start++;

    while(*start != '\0' && isspace(*start)) start++;
    while(*start != '\0' && !isspace(*start)) start++;
    end = start;
    start = strchr(word,',');
    start++;

    label_name = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation failed");
        exit(1);
    }
    copy = label_name;
    while(start < end)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';

    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        while(*end != '\0' && isspace(*end)) end++;
        if(!isspace(*end) && *end != '\0')
        {
            free(label_name);
            free_binary_list(&res);
            add_error("illegal text after argument ", line, file_name);
            return NULL;
        }
        check_extern(label_name, labels_table, file_name, IC++);/*if it's an extern label add a comment on the ext file*/
        by_word = 0;
        /*set the bits*/
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&by_word, 0, 1, 2):insert_bits(&by_word, 0, 1, 1);
        add_binary_node(&res, by_word);
        free(label_name);
        return res;
    }
    free(label_name);

    start = strchr(word,',');
    start++;
    arg = (char*)malloc(sizeof(char)*(strlen(start)+1));
    if(!arg)
    {
        fprintf(stdout, "memory allocation failed");
        exit(1);
    }
    copy = arg;
    while(*start != '\0')
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';
    /*found a matrix*/
    if(strchr(arg, '[') != NULL && search_label == NULL)/*found a matrix*/
    {
        insert_bits(&res->word, 2, 3, 2);/*set source bits*/
        add = get_mat_code(arg, line, file_name, labels_table, &IC);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    else if(search_label == NULL)/*should be a register*/
    {
        insert_bits(&res->word, 2, 3, 3);/*set source bits*/
        if(duble_reg)
        {
            insert_bits(&res->next->word, START_DEST_REGISTER, END_DEST_REGISTER, check_if_register(arg, line, file_name));
            free(arg);
            return res;
        }
        add = get_reg_code(arg, START_DEST_REGISTER, END_DEST_REGISTER, line, file_name);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    free(arg);
    return res;
}
/*source: 0,1,2,3. dest: 0,1,2,3*/
binary* code_for_one(char* word, int line, char* file_name, int opcode, labels* labels_table, int IC)
{
    short by_word = 0, num, duble_reg = 0;
    char *start, *end, *label_name, *copy, *arg, *free_arg;
    labels* search_label = NULL;
    binary* res = (binary*)malloc(sizeof(binary)), *add;
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    res->next = NULL;
    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    IC++;

    end = strchr(word, ',');
    if(!end)
    {
        free_binary_list(&res);
        add_error("missing comma ",line, file_name);
        return NULL;
    }
    start = word;

    arg = (char*)malloc(sizeof(char)*(end-start+1));
    if(!arg)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    free_arg = arg;
    copy = arg;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';

    start = arg;
    while(*start != '\0' && isspace(*start)) start++;
    while(*start != '\0' && !isspace(*start) && *start != ',') start++;
    end = start;
    start = arg;
    while(*start != '\0' && isspace(*start)) start++;
    label_name = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation fail");
        exit(1);
    }
    copy = label_name;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';
    /*start getting the source*/
    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        while(*end != '\0' && isspace(*end)) end++;
        if(*end != '\0')
        {
            free(label_name);
            free(free_arg);
            free_binary_list(&res);
            add_error("illegal text after argument ", line, file_name);
            return NULL;
        }
        check_extern(label_name, labels_table, file_name, IC++);/*if it's an extern label add a comment on the ext file*/
        by_word = 0;
        /*set the bits*/
        insert_bits(&res->word, 4, 5, 1);
        insert_bits(&by_word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&by_word, 0, 1, 2):insert_bits(&by_word, 0, 1, 1);
        add_binary_node(&res, by_word);
    }
    free(label_name);

    if(strchr(arg, '[') != NULL && search_label == NULL)/*found a matrix*/
    {
        by_word = 0;
        insert_bits(&res->word, 4, 5, 2);/*set source bits*/
        add = get_mat_code(arg, line, file_name, labels_table, &IC);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    else if(strchr(arg, '#') != NULL)
    {
        by_word = 0;
        start = strchr(arg, '#');
        start++;
        if(isspace(*start))
        {
            free_binary_list(&res);
            free(free_arg);
            add_error("illegal space ", line, file_name);
            return NULL;
        }
        num = get_last_num(start, line, file_name);
        if(num == (1 << 11))
        {
            free_binary_list(&res);
            free(free_arg);
            return NULL;
        }
        insert_bits(&by_word, 2, 10, num);
        add_binary_node(&res, by_word);
    }
    else if(search_label == NULL)/*should be a register*/
    {
        duble_reg++;
        insert_bits(&res->word, 4, 5, 3);/*set source bits*/
        add = get_reg_code(arg, START_SOURCE_REGISTER, END_SOURCE_REGISTER, line, file_name);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    free(free_arg);

    /*start getting the destination*/
    start = strchr(word, ',');
    if(!start)
    {
        free_binary_list(&res);
        add_error("missing comma ",line ,file_name);
        return NULL;
    }
    start++;

    while(*start != '\0' && isspace(*start)) start++;
    if(*start == '\0')
    {
        free_binary_list(&res);
        add_error("missing parameters ",line, file_name);
        return NULL;
    }
    end = start;
    while(*end != '\0' && !isspace(*end) && *end != ',') end++;
    label_name = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation failed");
        exit(1);
    }
    copy = label_name;
    while(start < end)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';
    by_word = 0;
    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        while(*end != '\0' && isspace(*end)) end++;
        if(*end != ',')
        {
            free(label_name);
            free_binary_list(&res);
            add_error("illegal text after argument ", line, file_name);
            return NULL;
        }
        check_extern(label_name, labels_table, file_name, IC++);/*if it's an extern label add a comment on the ext file*/
        by_word = 0;
        /*set the bits*/
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&by_word, 0, 1, 2):insert_bits(&by_word, 0, 1, 1);
        add_binary_node(&res, by_word);
        free(label_name);
        return res;
    }
    free(label_name);
    start = strchr(word,',');
    start++;

    arg = (char*)malloc(sizeof(char)*(strlen(start)+1));
    if(!arg)
    {
        fprintf(stdout, "memory allocation failed");
        exit(1);
    }
    free_arg = arg;
    copy = arg;
    while(*start != '\0')
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';
    /*found a matrix*/
    if(strchr(arg, '[') != NULL && search_label == NULL)/*found a matrix*/
    {
        insert_bits(&res->word, 2, 3, 2);/*set source bits*/
        add = get_mat_code(arg, line, file_name, labels_table, &IC);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    else if(strchr(arg, '#') != NULL)
    {
        start = strchr(arg, '#');
        start++;
        if(isspace(*start))
        {
            free_binary_list(&res);
            free(free_arg);
            add_error("illegal space ", line, file_name);
            return NULL;
        }
        num = get_last_num(start, line, file_name);
        if(num == (1 << 11))
        {
            free_binary_list(&res);
            free(free_arg);
            return NULL;
        }
        insert_bits(&by_word, 2, 10, num);
        add_binary_node(&res, by_word);
    }
    else if(search_label == NULL)/*should be a register*/
    {
        insert_bits(&res->word, 2, 3, 3);/*set source bits*/
        if(duble_reg)
        {
            insert_bits(&res->next->word, START_DEST_REGISTER, END_DEST_REGISTER, check_if_register(arg, line, file_name));
            free(free_arg);
            return res;
        }
        add = get_reg_code(arg, START_DEST_REGISTER, END_DEST_REGISTER, line, file_name);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    free(free_arg);
    return res;
}
/*source: 1,2. dest: 1,2,3*/
binary* code_for_four(char* word, int line, char* file_name, int opcode, labels* labels_table, int IC)
{
    short by_word = 0;
    char *start, *end, *label_name, *copy, *arg;
    labels* search_label = NULL;
    binary* res = (binary*)malloc(sizeof(binary)), *add;
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    res->next = NULL;
    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    IC++;

    end = strchr(word, ',');
    if(!end)
    {
        free_binary_list(&res);
        add_error("missing comma ",line, file_name);
        return NULL;
    }
    start = word;

    arg = (char*)malloc(sizeof(char)*(end-start+1));
    if(!arg)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    copy = arg;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';

    start = word;
    while(*start != '\0' && !isspace(*start) && *start != ',') start++;
    end = start;
    start = word;
    label_name = (char*)malloc((sizeof(char)*(end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation fail");
        exit(1);
    }
    copy = label_name;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';
    /*start getting the source*/
    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        while(*end != '\0' && isspace(*end) && *end != ',') end++;
        if(*end != ',')
        {
            free(label_name);
            free(arg);
            free_binary_list(&res);
            add_error("illegal text after argument ", line, file_name);
            return NULL;
        }
        check_extern(label_name, labels_table, file_name, IC++);/*if it's an extern label add a comment on the ext file*/
        by_word = 0;
        /*set the bits*/
        insert_bits(&res->word, 4, 5, 1);
        insert_bits(&by_word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&by_word, 0, 1, 2):insert_bits(&by_word, 0, 1, 1);
        add_binary_node(&res, by_word);
    }
    free(label_name);

    if(strchr(word, '[') != NULL && search_label == NULL)/*found a matrix*/
    {
        insert_bits(&res->word, 4, 5, 2);/*set source bits*/
        add = get_mat_code(arg, line, file_name, labels_table, &IC);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    else if(search_label == NULL)
    {
        free(arg);
        free_binary_list(&res);
        add_error("undefined label name ",line, file_name);
        return NULL;
    }
    free(arg);
    /*start getting the destination*/
    start = strchr(word, ',');
    if(!start)
    {
        free_binary_list(&res);
        add_error("missing comma ",line ,file_name);
        return NULL;
    }
    start++;

    while(*start != '\0' && isspace(*start)) start++;
    if(*start == '\0')
    {
        free_binary_list(&res);
        add_error("missing parameters ",line, file_name);
        return NULL;
    }
    end = start;
    while(*end != '\0' && !isspace(*end)) end++;
    label_name = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation faild");
        exit(1);
    }
    copy = label_name;
    while(start < end)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';

    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        while(*end != '\0' && isspace(*end)) end++;
        if(!isspace(*end) && *end != '\0')
        {
            free(label_name);
            free_binary_list(&res);
            add_error("illegal text after argument ", line, file_name);
            return NULL;
        }
        check_extern(label_name, labels_table, file_name, IC++);/*if it's an extern label add a comment on the ext file*/
        by_word = 0;
        /*set the bits*/
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&by_word, 0, 1, 2):insert_bits(&by_word, 0, 1, 1);
        add_binary_node(&res, by_word);
        free(label_name);
        return res;
    }
    free(label_name);
    start = strchr(word, ',');
    if(!start)
    {
        free_binary_list(&res);
        add_error("missing comma ",line, file_name);
        return NULL;
    }
    start++;

    arg = (char*)malloc(sizeof(char)*(strlen(start)+1));
    if(!arg)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    copy = arg;
    while(*start != '\0')
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';
    if(strchr(word, '[') != NULL && search_label == NULL)/*found a matrix*/
    {
        insert_bits(&res->word, 2, 3, 2);/*set source bits*/
        add = get_mat_code(arg, line, file_name, labels_table, &IC);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    else if(search_label == NULL)/*should be a register*/
    {
        insert_bits(&res->word, 2, 3, 3);/*set source bits*//*set source bits*/
        add = get_reg_code(arg, START_DEST_REGISTER, END_DEST_REGISTER, line, file_name);
        if(!add)
        {
            free_binary_list(&res);
            free(arg);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    free(arg);
    return res;

}
/*source: none. dest: 1,2,3*/
binary* code_for_five(char* word, int line, char* file_name, int opcode, labels* labels_table, int IC)
{
    short by_word = 0;
    char *start, *end, *label_name, *copy;
    labels* search_label = NULL;
    binary* res = (binary*)malloc(sizeof(binary)), *add;
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    res->next = NULL;
    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    IC++;

    start = word;
    while(*start != '\0' && !isspace(*start)) start++;
    end = start;
    while(*start != '\0' && isspace(*start)) start++;
    if(*start != '\0')
    {
        free_binary_list(&res);
        add_error("illegal text after argument ", line, file_name);
        return NULL;
    }
    start = word;
    label_name = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation fail");
        exit(1);
    }
    copy = label_name;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';

    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        check_extern(label_name, labels_table, file_name, IC++);/*if it's an extern label add a comment on the ext file*/
        by_word = 0;
        /*set the bits*/
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&by_word, 0, 1, 2):insert_bits(&by_word, 0, 1, 1);
        add_binary_node(&res, by_word);
        free(label_name);
        return res;
    }
    free(label_name);

    if(strchr(word, '[') != NULL)/*found a matrix*/
    {
        insert_bits(&res->word, 2, 3, 2);/*set source bits*/
        add = get_mat_code(word, line, file_name, labels_table, &IC);
        if(!add)
        {
            free_binary_list(&res);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    else/*should be a register*/
    {
        insert_bits(&res->word, 2, 3, 3);/*set source bits*/
        add = get_reg_code(word, START_DEST_REGISTER, END_DEST_REGISTER, line, file_name);
        if(!add)
        {
            free_binary_list(&res);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    return res;
}
/*source: none. dest: 0,1,2,3*/
binary* code_for_thirteen(char* word, int line, char* file_name, int opcode, labels* labels_table, int IC)
{
    short by_word = 0, num;
    char *start, *end, *label_name, *copy;
    labels* search_label = NULL;
    binary* res = (binary*)malloc(sizeof(binary)), *add;
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    res->next = NULL;
    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    IC++;

    start = word;
    while(*start != '\0' && !isspace(*start)) start++;
    end = start;
    while(*start != '\0' && isspace(*start)) start++;
    if(*start != '\0')
    {
        free_binary_list(&res);
        add_error("illegal text after argument ", line, file_name);
        return NULL;
    }
    start = word;
    label_name = (char*)malloc(sizeof(char)*((end-start)+1));
    if(!label_name)
    {
        fprintf(stdout, "memory allocation fail");
        exit(1);
    }
    copy = label_name;
    while(end > start)
    {
        *copy = *start;
        start++;
        copy++;
    }
    *copy = '\0';

    if((search_label = find_label(labels_table, label_name)) != NULL)
    {
        check_extern(label_name, labels_table, file_name, IC++);/*if it's an extern label add a comment on the ext file*/
        by_word = 0;
        /*set the bits*/
        insert_bits(&res->word, 2, 3, 1);
        insert_bits(&by_word, 2, 10, search_label->address);
        (search_label->address != 0)? insert_bits(&by_word, 0, 1, 2):insert_bits(&by_word, 0, 1, 1);
        add_binary_node(&res, by_word);
        free(label_name);
        return res;
    }
    free(label_name);

    if(strchr(word, '[') != NULL)/*found a matrix*/
    {
        insert_bits(&res->word, 2, 3, 2);/*set source bits*/
        add = get_mat_code(word, line, file_name, labels_table, &IC);
        if(!add)
        {
            free_binary_list(&res);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    else if(strchr(word, '#') != NULL)
    {
        start = strchr(word, '#');
        start++;
        if(isspace(*start))
        {
            free_binary_list(&res);
            add_error("illegal space ", line, file_name);
            return NULL;
        }
        num = get_last_num(start, line, file_name);
        if(num == (1 << 11))
        {
            free_binary_list(&res);
            return NULL;
        }
        insert_bits(&by_word, 2, 10, num);
        add_binary_node(&res, by_word);
    }
    else/*should be a register*/
    {
        insert_bits(&res->word, 2, 3, 3);/*set source bits*/
        add = get_reg_code(word, START_DEST_REGISTER, END_DEST_REGISTER, line, file_name);
        if(!add)
        {
            free_binary_list(&res);
            return NULL;
        }
        res = add_binary_node_with_word(&res, add);
    }
    return res;
}
/*source: none. dest: none*/
binary* code_for_fourteen(int opcode)
{
    binary* res = (binary*)malloc(sizeof(binary));
    if(!res)
    {
        fprintf(stdout, "memory allocation error.\n");
        exit(1);
    }
    res->word = 0;
    insert_bits(&res->word, START_OPCODE, END_OPCODE, opcode);
    res->next = NULL;
    return res;
}

