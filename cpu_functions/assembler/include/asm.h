#ifndef ASM_H_INCLUDED
#define ASM_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
//#include <TXlib.h>
#include <sys/stat.h>
#include "../../onegin_functions/include/onegin_libraries.h"
#define ONEGIN_LIB_INCLUDED


#include "../../errors.h"

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

#define READFILE_ERROR_CHECK(cond, rtrn, closing_file)      \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                if (cond)                                   \
                {                                           \
                    fclose(closing_file);                   \
                    return rtrn;                            \
                }                                           \
            } while(false)


const double EPS = 1e-7;
#define COMPARE_DOUBLE(arg, num)                                        \
            (fabs((arg) - (num)) <= EPS)


const size_t ASM_VERSION     = 1;

const size_t MAX_WORD_LEN    = 34;
const size_t HEADER_SIZE     = 3;
const size_t MAX_LINE_SIZE   = 3;
const size_t LABELS_AMOUNT   = 10;
const size_t REGISTER_LENGTH = 3;

const int LABEL_VALUE_POISON = -1;

static const char *const SIGNATURE = "DP";

const char *const  INPUT_FILE_NAME = "../io/user_file.txt";
//const char * const  INPUT_FILE_NAME = "../io/krujochek.txt";
//const char * const  INPUT_FILE_NAME = "../io/quadratic.txt";
//const char * const  INPUT_FILE_NAME = "../io/factorial.txt";

const char *const OUTPUT_FILE_NAME = "../io/asm_output";

const char *const ASM_DUMP_FILE    = "../io/asm_dump.txt";


#define DEF_CMD(name, num, arg, code) \
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


struct Label
{
    int  value = LABEL_VALUE_POISON;
    char name[MAX_WORD_LEN]= {};
};

struct AsmField
{
    char   *char_buffer = NULL;
    size_t  chars_count = 0;
    double *code_buffer = NULL;
    size_t  lines_count = 0;

    int pc = 0;

    struct Label labels[LABELS_AMOUNT] = {{}};
    struct WorkingField *onegin_field  = NULL;
};

int ProccessMainArgument(int argc, char *argv[], const char **input_file_name, const char **output_file_name);

int ReadUserFile(const char *user_file_name, struct AsmField *field);

int AssemblyUserCode(struct AsmField *field);

int WriteCode(struct AsmField *field, const char *output_file_name);
int WriteHeader(struct AsmField *field, FILE *output_file);

int CountSize(const char *file_name, size_t *file_size);
int CountLines(struct AsmField *field, size_t *value);

int ReadArg(struct AsmField *field, char **buf);

int ReadDigit(struct AsmField *field, char **buf);
int ReadPlusConstruction(struct AsmField *field, char **buf);
int IsReg(char *buf, int *arg);
int ReadLabel(struct AsmField *field, char **buf);

int PrintHeader(struct AsmField *field);

int FindLabelValue(struct AsmField *field, char *label_name, int *arg);
int ProcessLabel(struct AsmField *field, char *cmd);

int InitializeLabels(struct AsmField *field);

int DumpCode(struct AsmField *field);

int DumpField(struct AsmField *field);

int AsmFieldCtor(struct AsmField *field);
int AsmFieldDtor(struct AsmField *field);

#endif // ASM_H_INCLUDED
