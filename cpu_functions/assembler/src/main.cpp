
#include "../include/asm.h"

int main(int argc, char *argv[])
{
    const char  *input_file_name = NULL;
    const char *output_file_name = NULL;
    ProccessMainArgument(argc, argv, &input_file_name, &output_file_name);

    //printf(" input: %s\n",  input_file_name);
    //printf("output: %s\n", output_file_name);

    struct AsmField field = {};
    AsmFieldCtor(&field);

    int read_err_check = ReadUserFile(input_file_name, &field);
    ERROR_CHECK(read_err_check, READ_USER_FILE_ERROR);

    DumpField(&field);
    
    //two assemblies
    for (int counter = 0; counter < 2; counter++)
    {
        int convert_err_check = AssemblyUserCode(&field);
        ERROR_CHECK(convert_err_check, ASSEMBLY_USER_CODE_ERROR);

        int dump_err = DumpCode(&field);
        ERROR_CHECK(dump_err, ASM_DUMP_ERROR);
    }

    WriteCode(&field, output_file_name);

    AsmFieldDtor(&field);

    printf("\nSUCCESSFUL END.\n");

    getchar();

    return SUCCESS;
}







