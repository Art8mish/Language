
#include "../include/disasm.h"

int main()
{
    struct DisAsmField field = {};

    DisAsmFieldCtor(&field);

    int read_code_err_check = ReadCode(CODE_INPUT_FILE, &field);
    ERROR_CHECK(read_code_err_check, READ_CODE_ERROR);

    int err_check = DisAsmCode(&field);
    ERROR_CHECK(err_check, DISASM_CODE_ERROR);

    DisAsmFieldDtor(&field);

    printf("\nSUCCESFUL END.\n");

    return SUCCESS;
}