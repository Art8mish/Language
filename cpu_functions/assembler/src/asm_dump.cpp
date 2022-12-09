
#include "../include/asm.h"

#define PRINT_ARG()                                                      \
    if (*((int*)code) & MEMORY_CODE)                                     \
        fprintf(dump_file, "RAM : ");                                    \
    if ((*((int*)code) & REGISTER_CODE) &&                               \
        (*((int*)code) & IMMEDIATE_CONST_CODE))                          \
    {                                                                    \
        fprintf(dump_file, "%lf (num) & %d (reg)", *(code+1), *(code+2));\
        two_arg = true;                                                  \
    }                                                                    \
    else if (*((int*)code) & REGISTER_CODE)                              \
        fprintf(dump_file, "%d (reg)", *((int *)(code + 1)));            \
    else if (*((int*)code) & IMMEDIATE_CONST_CODE)                       \
        fprintf(dump_file, "%g (num)", *(code + 1));                     \
    else                                                                 \
        fprintf(dump_file, "ERROR/label : %d", *(code + 1));             \
    fprintf(dump_file, "\n");                                            \
                                                                         \
    if (two_arg)                                                         \
    {                                                                    \
        op_counter++;                                                    \
        code++;                                                          \
    }                                                                    \
    op_counter++;                                                        \
    code++

int DumpCode(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    FILE *dump_file = fopen(ASM_DUMP_FILE, "a");
    ERROR_CHECK(dump_file == NULL, OPENING_FILE_ERROR);

    double *code = field->code_buffer;

    fprintf(dump_file, "\n\nDUMP\n");

    //print Code
    fprintf(dump_file, "\nCode: \n");
    for (int i = 0; i < field->pc; i++)
        fprintf(dump_file, "%g ", code[i]);
    fprintf(dump_file, "\n\n");

    bool two_arg = false;

    //print commands
    for (int op_counter = 0; op_counter < field->pc; op_counter++)
    {
        #define DEF_CMD(name, num, arg, cpu_code)                        \
            else if ((*((int*)code) & 0xFFFF) == name##_CODE)            \
            {                                                            \
                if (arg)                                                 \
                {                                                        \
                    fprintf(dump_file, "%-3d %-4s : ",*((int*)code) & 0xFFFF, #name); \
                    PRINT_ARG();                                         \
                }                                                        \
                else                                                     \
                    fprintf(dump_file, "%-3d %-4s\n", *((int*)code), #name);  \
            }

        if (false) {}

        #include "../../cmd.h"

        else
            fprintf(dump_file, "UNKNOWN COMMAND\n");

        code++;
        two_arg = false;

        #undef DEF_CMD
    }

    //print labels
    fprintf(dump_file, "\nlabels: \n");
    for (unsigned int i = 0; i < LABELS_AMOUNT; i++)
        fprintf(dump_file, "labels[%d].name = %s, labels[%d].value = %d\n", i, field->labels[i].name, i, field->labels[i].value);

    fclose(dump_file);
    
    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int DumpField(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    FILE *dump_file = fopen(ASM_DUMP_FILE, "a");
    ERROR_CHECK(dump_file == NULL, OPENING_FILE_ERROR);

    fprintf(dump_file, "\nStruct Asm : char_buffer[%p], chars_count %d, code_buffer[%p], lines_count %d\n",
                        field->char_buffer, field->chars_count, 
                        field->code_buffer, field->lines_count);

    fclose(dump_file);

    return SUCCESS;
}

