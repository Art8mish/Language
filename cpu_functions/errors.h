#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

#ifndef STACK_LIB_INCLUDED

enum StackError
{
    SUCCESS                   = 0,
    PTR_NULL                  = 1,
    OPERATION_WITH_DEAD_STACK = 2,
    ERROR_INTO_STACK          = 3,
    NONEXISTENT_VALUE         = 4,
    CALLOC_ERROR              = 5,
    REALLOC_ERROR             = 6,
    HASH_CALC_ERROR           = 7,
    CTORING_ALIVE_STACK       = 8,
    CLOSING_FILE_ERROR        = 10,
};

#ifndef ONEGIN_LIB_INCLUDED

enum OneginError
{
    STAT_ERROR         = 16,
    OPENING_FILE_ERROR = 9,
};

#endif

#endif

enum AsmError
{
          USER_FILE_OPENING_ERROR = 11,
          USER_FILE_CLOSING_ERROR = 12,
    OUTPUT_ASM_FILE_OPENING_ERROR = 13,
    OUTPUT_ASM_FILE_CLOSING_ERROR = 14,
                      FREAD_ERROR = 15,
                  COUNTSIZE_ERROR = 17,
                 COUNTLINES_ERROR = 18,
             UNKNOWN_COMAND_ERROR = 19,
                     NO_HLT_ERROR = 20,
                     SYNTAX_ERROR = 21,
             LABEL_OVERFLOW_ERROR = 22,
    NO_LABEL_WITH_THIS_NAME_ERROR = 23,
           FIND_LABEL_VALUE_ERROR = 24,
              PROCESS_LABEL_ERROR = 25,
                READ_HEADER_ERROR = 26,
                 READ_LABEL_ERROR = 27,
                      ISREG_ERROR = 28,
        READ_PLUS_CONSTRUCT_ERROR = 29,
         TOO_MANY_MAIN_ARGS_ERROR = 30,
             READ_USER_FILE_ERROR = 31,
         ASSEMBLY_USER_CODE_ERROR = 32,
               WRITE_HEADER_ERROR = 33,
                 READ_DIGIT_ERROR = 34,
            FIND_FREE_LABEL_ERROR = 35,
                   ASM_DUMP_ERROR = 36,
};

enum CpuError
{
        WRONG_SIGNATURE_ERROR = 40,
          WRONG_VERSION_ERROR = 41,
              READ_CODE_ERROR = 42,
    INCORRECT_USER_CODE_ERROR = 43,
           EXECUTE_CODE_ERROR = 44,
     INCORRECT_RAM_CELL_ERROR = 45,
       DIVISION_BY_ZERO_ERROR = 46,
                  EMPTY_ERROR = 47,
   INCORRECT_SQRT_VALUE_ERROR = 48,
          DUMP_COMMANDS_ERROR = 49,
                DUMP_PC_ERROR = 50,
             DUMP_STACK_ERROR = 51,
              DUMP_REGS_ERROR = 52,
               DUMP_RAM_ERROR = 53,
                 FWRITE_ERROR = 54,

};

enum DisAsmError
{
        DISASM_CODE_ERROR = 55,
          WRONG_REG_ERROR = 56,
           NULL_PTR_ERROR = 57,
};



#endif // ERRORS_H_INCLUDED
