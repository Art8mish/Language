#include "../include/cpu.h"

#define PRINT_ARG()                                                      \
    if (*((int*)code) & MEMORY_CODE)                                     \
        printf("RAM : ");                                                \
    if ((*((int*)code) & REGISTER_CODE) &&                               \
        (*((int*)code) & IMMEDIATE_CONST_CODE))                          \
    {                                                                    \
        printf("%lf (num) & %d (reg)", *(code+1), *(code+2));            \
        two_arg = true;                                                  \
    }                                                                    \
    else if (*((int*)code) & REGISTER_CODE)                              \
        printf("%d (reg)", *((int *)(code + 1)));                        \
    else if (*((int*)code) & IMMEDIATE_CONST_CODE)                       \
        printf("%g (num)", *(code + 1));                                 \
    else                                                                 \
        printf("ERROR/label : %d", *(code + 1));                         \
    printf("\n");                                                        \
                                                                         \
    if (two_arg)                                                         \
    {                                                                    \
        op_counter++;                                                    \
        code++;                                                          \
    }                                                                    \
    op_counter++;                                                        \
    code++


int ReadCode(const char *code_file_name, struct CpuField *field)
{
    ERROR_CHECK(code_file_name == NULL, PTR_NULL);
    ERROR_CHECK(         field == NULL, PTR_NULL);

    FILE *input_file = fopen(code_file_name, "rb");
    FILE_ERROR_CHECK(input_file == NULL, OPENING_FILE_ERROR, input_file);

    int read_header_err = ReadHeader(input_file, field);
    FILE_ERROR_CHECK(read_header_err, READ_HEADER_ERROR, input_file);

    field->code_buffer = (double *) calloc(field->op_count, sizeof(double));
    ERROR_CHECK(field->code_buffer == NULL, CALLOC_ERROR);

    fread(field->code_buffer, sizeof(double), field->op_count, input_file);
    int fread_err_check = ferror(input_file);
    FILE_ERROR_CHECK(fread_err_check, FREAD_ERROR, input_file);

    /*
    double *code = field->code_buffer;
    //print Code
    printf("\nCode: \n");
    for (int i = 0; i < field->op_count; i++)
        printf("%g ", code[i]);
    printf("\n\n");

    bool two_arg = false;

    //print commands
    for (int op_counter = 0; op_counter < field->op_count; op_counter++)
    {
        #define DEF_CMD(name, num, arg, cpu_code)                        \
            else if ((*((int*)code) & 0xFFFF) == name##_CODE)            \
            {                                                            \
                if (arg)                                                 \
                {                                                        \
                    printf("%-3d %-4s : ",*((int*)code) & 0xFFFF, #name); \
                    PRINT_ARG();                                         \
                }                                                        \
                else                                                     \
                    printf("%-3d %-4s\n", *((int*)code), #name);  \
            }

        if (false) {}

        #include "../../cmd.h"

        else
            printf("UNKNOWN COMMAND\n");

        code++;
        two_arg = false;

        #undef DEF_CMD
    }*/

    fclose(input_file);

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int ReadHeader(FILE *input_file, struct CpuField *field)
{
    ERROR_CHECK(input_file == NULL, PTR_NULL);
    ERROR_CHECK(     field == NULL, PTR_NULL);

    int header[HEADER_SIZE] = {};

    fread(header, sizeof(int), HEADER_SIZE, input_file);
    int fread_err_check = ferror(input_file);
    ERROR_CHECK(fread_err_check, FREAD_ERROR);

    ERROR_CHECK(strncmp((char*)header, SIGNATURE, 2), WRONG_SIGNATURE_ERROR);

    int version = header[1];
    ERROR_CHECK(version != CPU_VERSION, WRONG_VERSION_ERROR);

    field->op_count = header[2];

    return SUCCESS;
}
