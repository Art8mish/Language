
#include "../include/asm.h"

int ProccessMainArgument(int argc, char *argv[], const char **input_file_name, const char **output_file_name)
{
    ERROR_CHECK(            argv == NULL, PTR_NULL);
    ERROR_CHECK( input_file_name == NULL, PTR_NULL);
    ERROR_CHECK(output_file_name == NULL, PTR_NULL);

    if (argc == 1)
    {
         *input_file_name =  INPUT_FILE_NAME;
        *output_file_name = OUTPUT_FILE_NAME;
    }

    else if (argc == 2)
    {
         *input_file_name = argv[1];
        *output_file_name = OUTPUT_FILE_NAME;
    }

    else if (argc == 3)
    {
         *input_file_name = argv[1];
        *output_file_name = argv[2];
    }

    else
        return TOO_MANY_MAIN_ARGS_ERROR;

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int ReadUserFile(const char *user_file_name, struct AsmField *field)
{
    ERROR_CHECK(user_file_name == NULL, PTR_NULL);
    ERROR_CHECK(         field == NULL, PTR_NULL);

    field->onegin_field = CreateWorkingField(user_file_name);
    ERROR_CHECK(field->onegin_field == NULL, USER_FILE_OPENING_ERROR);

    field->char_buffer = field->onegin_field->chars_buffer;
    field->chars_count = field->onegin_field->chars_amount;
    *(field->char_buffer + field->chars_count)  = EOF;

    field->lines_count = field->onegin_field->lines_amount;
    field->code_buffer = (double*) calloc(field->lines_count * MAX_LINE_SIZE, sizeof(double));

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int WriteHeader(struct AsmField *field, FILE *output_file)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    int header[HEADER_SIZE] = {};

    *((char*)header)     = SIGNATURE[0];
    *((char*)header + 1) = SIGNATURE[1];

    *(header + 1) = ASM_VERSION;
    *(header + 2) = field->pc;

    int fwrite_elem = fwrite(header, sizeof(int), HEADER_SIZE, output_file);
    ERROR_CHECK(fwrite_elem != HEADER_SIZE, FWRITE_ERROR);

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int WriteCode(struct AsmField *field, const char *output_file_name)
{
    ERROR_CHECK(           field == NULL, PTR_NULL);
    ERROR_CHECK(output_file_name == NULL, PTR_NULL);

    FILE *output_file = fopen(output_file_name, "wb");

    int write_header_err = WriteHeader(field, output_file);
    ERROR_CHECK(write_header_err, WRITE_HEADER_ERROR);

    int fwrite_elem = fwrite(field->code_buffer, sizeof(double), field->pc, output_file);
    ERROR_CHECK(fwrite_elem != field->pc, FWRITE_ERROR);

    fclose(output_file);

    printf("\nCODE WRITED SUCCESSFULLY\n");

    return SUCCESS;
}