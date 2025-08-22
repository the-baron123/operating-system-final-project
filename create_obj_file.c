#include "inputs_1.h"
#include "phase_2.h"


void create_obj(data_collect_phase_2* all_data, char* obj_name)
{
    int i = START_COUNT;
    char *line;
    FILE* obj;
    ic_code* p_ic, *ic_read = all_data->ic_value;
    dc_code* p_dc, *dc_read = all_data->dc_values;

    /* Initialize counter */
    i = START_COUNT;

    /* Initialize IC and DC read pointers */
    ic_read = all_data->ic_value;
    dc_read = all_data->dc_values;

    /* Open the object file for writing */
    obj = fopen(obj_name, "w");
    if (!obj)
    {
        fprintf(stdout, "err opening %s file", obj_name);
        exit(1);
    }

    /* Write IC and DC counts at the top of the file */
    line = base_four_no_bit(all_data->IC);
    fputs("    ", obj);
    fputs(line, obj);
    free(line);
    fputs("  ", obj);
    line = base_four_no_bit(all_data->DC);
    fputs(line, obj);
    free(line);
    fputs("\n", obj);

    /* Write all instruction code (IC) entries */
    while (ic_read != NULL)
    {
        line = base_four(i, 4); /* convert address to base-4 */
        fputs(line, obj);
        free(line);
        fputs("  ", obj);
        line = base_four(ic_read->code_value, 5); /* convert code value to base-4 */
        printf("address: %d  value: %d\n", i, ic_read->code_value);
        fputs(line, obj);
        free(line);
        fputs("\n", obj);
        p_ic = ic_read->next; /* move to next IC node */
        ic_read = p_ic;
        i++;
    }

    /* Write all data code (DC) entries */
    while (dc_read != NULL)
    {
        line = base_four(i, 4); /* convert address to base-4 */
        fputs(line, obj);
        free(line);
        fputs("  ", obj);
        line = base_four(dc_read->code_value, 5); /* convert code value to base-4 */
        printf("address: %d  value: %d\n", i, dc_read->code_value);
        fputs(line, obj);
        free(line);
        fputs("\n", obj);
        p_dc = dc_read->next; /* move to next DC node */
        dc_read = p_dc;
        i++;
    }
    /* Close the object file */
    fclose(obj);
    return;
}

