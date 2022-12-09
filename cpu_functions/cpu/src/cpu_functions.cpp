
#include "../include/cpu.h"

int CpuFieldCtor(struct CpuField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    *field = (struct CpuField){};

    field->code_buffer = NULL;
    field->op_count    = 0;
    field->pc          = 0;

    //initialize stacks
    STACKCTOR(&field->stk, 10);      
    STACKCTOR(&field->ret_adr, 10);

    //initialize Regs[]
    for (unsigned int i = 0; i < REGS_AMOUNT; i++)
        field->Regs[i] = 0;

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int CpuFieldDtor(struct CpuField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    StackDtor(&field->stk);
    StackDtor(&field->ret_adr);

    ERROR_CHECK(field->code_buffer == NULL, PTR_NULL);
    free(field->code_buffer);

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int ExecuteCode(struct CpuField *field)
{
    double      arg_val = 0;
    double *ptr_arg_val = &arg_val;

    double *code = field->code_buffer;

    double  lhs  = 0;
    double  rhs  = 0;

    field->pc = 0;

    while (GETKEY(code + field->pc) != HLT_CODE)
    {
        printf("key : %d ", GETKEY(code + field->pc));
        switch(GETKEY(code + field->pc))
        {
            #define DEF_CMD(name, num, arg, cpu_code)                    \
                case name##_CODE :  printf("%s", #name);                \
                                    if (arg)                             \
                                    {\
                                        GetPtrArg(field, &ptr_arg_val);  \
                                        printf(" : arg = %g", *ptr_arg_val);\
                                    }\
                                    cpu_code                             \                              
                                    printf("\n"); \
                                    break;

            #include "../../cmd.h"

            default : printf(" # PerformCode(): ERROR: code = %d. \n", GETKEY(code + field->pc));
                      break;

            #undef DEF_CMD
        }

        arg_val = 0;
        ptr_arg_val = &arg_val;
        field->pc++;
    }

    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------------------

int GetPtrArg(struct CpuField *field, double **val)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(  val == NULL, PTR_NULL);

    int cmd = *((int *)(field->code_buffer + field->pc));
    **val = 0;

    if (cmd & IMMEDIATE_CONST_CODE)
        **val += field->code_buffer[++field->pc];

    if (cmd & REGISTER_CODE)
    {
        field->pc++;
        ERROR_CHECK(*((int *)(field->code_buffer + field->pc)) < 0 || *((int *)(field->code_buffer + field->pc)) > (int)REGS_AMOUNT, SYNTAX_ERROR);

        if ((cmd & IMMEDIATE_CONST_CODE) || (cmd & MEMORY_CODE))
            **val += field->Regs[*((int *)(field->code_buffer + field->pc))];
        else
        {
            *val = &field->Regs[*((int *)(field->code_buffer + field->pc))];
            return SUCCESS;
        }
    }

    if (cmd & MEMORY_CODE)
    {
        ERROR_CHECK(*((int*)(*val)) < 0 || *((int*)(*val)) >= (double)RAM_SIZE, INCORRECT_RAM_CELL_ERROR);
        *val = &field->Ram[*((int*)(*val))];

        return SUCCESS;
    }

    if (cmd & IMMEDIATE_CONST_CODE)
        return SUCCESS;

    return SYNTAX_ERROR;
}

/*
int GetArg(struct CpuField *field, int *value)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(value == NULL, PTR_NULL);

    int val = 0;
    int cmd = field->code_buffer[field->pc];

    if (cmd & IMMEDIATE_CONST_CODE)
        val += field->code_buffer[++field->pc];

    if (cmd & REGISTER_CODE)
        val += field->Regs[field->code_buffer[++field->pc]];

    if (cmd & MEMORY_CODE)
    {
        ERROR_CHECK(val < 0 || val >= (int)RAM_SIZE, INCORRECT_RAM_CELL_ERROR);
        val = field->Ram[val];
    }

    *value = val;

    return SUCCESS;
}*/
//--------------------------------------------------------------------------------------------------------------

int DumpProcessor(struct CpuField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    FILE *dump_file = fopen(LOG_FILE_NAME, "a");
    ERROR_CHECK(dump_file == NULL, OPENING_FILE_ERROR);

    fprintf(dump_file, "\nProcessor Dump:\n");

    //print commands
    int dump_err = DumpCommands(field, dump_file);
    FILE_ERROR_CHECK(dump_err, DUMP_COMMANDS_ERROR, dump_file);

    //print pc
    dump_err = DumpPc(field, dump_file);
    FILE_ERROR_CHECK(dump_err, DUMP_PC_ERROR, dump_file);

    //print stack
    dump_err = DumpStack(field, dump_file);
    FILE_ERROR_CHECK(dump_err, DUMP_STACK_ERROR, dump_file);

    //print Regs
    dump_err = DumpRegs(field, dump_file);
    FILE_ERROR_CHECK(dump_err, DUMP_REGS_ERROR, dump_file);

    //print RAM
    dump_err = DumpRam(field, dump_file);
    FILE_ERROR_CHECK(dump_err, DUMP_RAM_ERROR, dump_file);

    fclose(dump_file);

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int DumpCommands(struct CpuField *field, FILE *dump_file)
{
    ERROR_CHECK(field     == NULL, PTR_NULL);
    ERROR_CHECK(dump_file == NULL, PTR_NULL);

    for (unsigned int i = 0; i < field->op_count; i++)
        fprintf(dump_file, "%-8d ", i+1);
    fprintf(dump_file, "\n");

    double *code = field->code_buffer;
    for (unsigned int i = 0; i < field->op_count; i++)
        fprintf(dump_file, "%-8X ", *code++);
    fprintf(dump_file, "\n");

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int DumpPc(struct CpuField *field, FILE *dump_file)
{
    ERROR_CHECK(field     == NULL, PTR_NULL);
    ERROR_CHECK(dump_file == NULL, PTR_NULL);

    for (unsigned int i = 0; i < field->pc * 9; i++)
        fprintf(dump_file, " ");
    fprintf(dump_file, "^\n");
    for (unsigned int i = 0; i < field->pc * 9; i++)
        fprintf(dump_file, " ");
    fprintf(dump_file, "pc = %d\n", field->pc);

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int DumpStack(struct CpuField *field, FILE *dump_file)
{
    ERROR_CHECK(field     == NULL, PTR_NULL);
    ERROR_CHECK(dump_file == NULL, PTR_NULL);

    fprintf(dump_file, "Stack:\n");
    fprintf(dump_file, "{\n");
    for (unsigned int i = 0; i < field->stk.data_size; i++)
        fprintf(dump_file, "\t[%02d] = " ELEM_T_SPECIFIER "\n", i, field->stk.data[i]);
    fprintf(dump_file, "}\n\n");

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int DumpRegs(struct CpuField *field, FILE *dump_file)
{
    ERROR_CHECK(field     == NULL, PTR_NULL);
    ERROR_CHECK(dump_file == NULL, PTR_NULL);

    fprintf(dump_file, "Regs[]:\n");
    fprintf(dump_file, "{\n");
    for (unsigned int i = 0; i < REGS_AMOUNT; i++)
        fprintf(dump_file, "\tRegs[%02d] = %g\n", i, field->Regs[i]);
    fprintf(dump_file, "}\n\n");

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int DumpRam(struct CpuField *field, FILE *dump_file)
{
    ERROR_CHECK(field     == NULL, PTR_NULL);
    ERROR_CHECK(dump_file == NULL, PTR_NULL);

    fprintf(dump_file, "Ram[]:\n\n");
    for (unsigned int i = 0; i < RAM_SIZE; i++)
    {
        if ((i > 0) && (i % 100 == 0))
            fprintf(dump_file, "\n");
        fprintf(dump_file, "%-2g ", field->Ram[i]);
    }
    fprintf(dump_file, "\n");

    return SUCCESS;
}