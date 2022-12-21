#ifndef DISASM_H_INCLUDED
#define DISASM_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../errors.h"
//#include <TXlib.h>


#ifdef SOFT_ASSERT
#undef SOFT_ASSERT
#endif

#define ASSERT_MODE

#ifdef ASSERT_MODE

#define SOFT_ASSERT(condition)                                                    \
            do                                                                    \
            {                                                                     \
                if (condition)                                                 \
                    printf("Error in %s; file: %s; num of line: %d \n",           \
                           #condition, __FILE__, __LINE__);                       \
            } while(false)

#else

#define SOFT_ASSERT(condition) ;

#endif



#define ERROR_CHECK(cond, rtrn)                             \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                if (cond)                                   \
                    return rtrn;                            \
            } while(false)

#define FILE_ERROR_CHECK(cond, rtrn, closing_file)          \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                if (cond)                                   \
                {                                           \
                    fclose(closing_file);                   \
                    return rtrn;                            \
                }                                           \
            } while(false)

#define GETKEY(arg)                                         \
            (*((int*)(arg)) & 0xFFFF)

#define DBLCMP(arg, num)                                        \
            (fabs((arg) - (num)) <= EPS)

const int DISASM_VERSION = 1;

static const char *CODE_INPUT_FILE = "../io/asm_output";
static const char *LOG_FILE_NAME   = "../io/disasm_out.txt";

const size_t HEADER_SIZE      = 3;

const size_t SIGNATURE_LENGTH = 3;
static const char * const DISASM_SIGNATURE = "DP";


#define DEF_CMD(name, num, arg, code)  \
            name##_CODE = num,

enum Code
{
    #include "../../cmd.h"
};

#undef DEF_CMD

enum CmdConst
{
    IMMEDIATE_CONST_CODE = 1 << 16,
           REGISTER_CODE = 1 << 17,
             MEMORY_CODE = 1 << 18,
};

struct DisAsmField
{
    double *code_buffer = NULL;
    size_t  op_count    = 0;
    unsigned int pc     = 0;
};

enum Register
{
      FIRST_REG = 1,
     SECOND_REG = 2,
      THIRD_REG = 3,
     FOURTH_REG = 4,
      FIFTH_REG = 5,
      SIXTH_REG = 6,
    SEVENTH_REG = 7,
     EIGHTH_REG = 8,
      NINTH_REG = 9,
      TENTH_REG = 10,
};


int ReadCode(const char *code_file_name, struct DisAsmField *field);

int ReadHeader(FILE *input_file, struct DisAsmField *field);


int DisAsmCode(struct DisAsmField *field);

int PrintArg(struct DisAsmField *field, FILE *out_file);

int DisAsmFieldCtor(struct DisAsmField *field);
int DisAsmFieldDtor(struct DisAsmField *field);

#endif // DISASM_H_INCLUDED
