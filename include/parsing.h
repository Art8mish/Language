#ifndef PARSING_H_INCLUDED
#define PARSING_H_INCLUDED

#include "language.h"

#define EOF_CHECK(buf, err)                                 \
            do {                                            \
                if (*(buf) == EOF)                          \
                    return err;                             \
            } while (false)


#define SKIP_SPACE(buf, err)                                \
            while(isspace(*(buf)) || *(buf) == '\0')        \
            {                                               \
                (buf)++;                                    \
                EOF_CHECK(buf, err);                        \
            }

#define SKIP_SPACE_STR(buf)                                 \
            while(isspace(*(buf)))                          \
            {                                               \
                (buf)++;                                    \
                EOF_CHECK(buf, NULL);                       \
            }

#define SKIP_COMMENT(buf, err)                              \
            while(*(buf) != '>')                            \
            {                                               \
                (buf)++;                                    \
                EOF_CHECK(buf, err);                        \
            }

#define SKIP_EXPANSION(buf, err)                            \
            while(*(buf) != '$')                            \
            {                                               \
                (buf)++;                                    \
                EOF_CHECK(buf, err);                        \
            }


#define LEX_SKIP_SPACE(buf)                                  \
            while(isspace(*(buf)) || *(buf) == '\0')         \
                (buf)++;                                    

#define LEX_ERROR(cond, err, index)                          \
            do                                               \
            {                                                \
                SOFT_ASSERT(cond);                           \
                if (cond)                                    \
                {                                            \
                    printf("ERROR in structs[%d]\n", index); \
                    return err;                              \
                }                                            \
            } while(false)

#define SAFE_LEX_ERROR(cond, code, err, index)               \
            do                                               \
            {                                                \
                SOFT_ASSERT(cond);                           \
                if (cond)                                    \
                {                                            \
                    printf("ERROR in structs[%d]\n", index); \
                    code                                     \
                    return err;                              \
                }                                            \
            } while(false)

enum LexType
{
    LT_NIL = 0,
    LT_ST  = 1,
    LT_VAR = 2,

    LT_IF  = 3,
    LT_ELSE = 4,
    LT_WHILE = 5,

    LT_TYPE_FUNC = 6,
    LT_VOID_FUNC = 7,
    LT_RET  = 8,
    LT_CALL = 9,
    LT_PARAM = 10,

    LT_NUM  = 11,
    LT_EQ   = 12,
    LT_OP   = 13,

    LT_IN    = 15,
    LT_OUT   = 16,
    
    LT_STR  = 17,
    
    LT_EXP_L_BRCKT = 18,
    LT_EXP_R_BRCKT = 19,

    LT_STREAM_L_BRCKT = 20,
    LT_STREAM_R_BRCKT = 21,

    LT_ST_SEP = 22,
    LT_COMMA  = 23,

    LT_END = 24,
};

struct LangLexicalElem
{
    LexType  type      = LT_NIL;
    ArithmOp arithm_op = OP_PSN;
    num_t    num       = NUM_PSN;
    const char *str    = NULL;

    bool func_name = false;
};

enum LexError
{
    ERROR_FULL_LEX_STRUCT    = 1,
    ERROR_LONG_STR           = 2,
    ERROR_PROCESS_VAR_INIT   = 3,
    ERROR_PROCESS_EXPRESSION = 4,
    ERROR_PROCESS_ARG        = 5,
    ERROR_PROCESS_FUNCTION   = 6,
    ERROR_RUS_TRANSLATE      = 7,
    ERROR_PRINT_IF           = 8,
    ERROR_PRINT_WHILE        = 9,
    ERROR_PRINT_VAR          = 10,
    ERROR_PRINT_FUNC_DEF     = 11,
    ERROR_PRINT_ST_STREAM    = 12,
    ERROR_PRINT_ST           = 13,
};

typedef struct LangLexicalElem LexStruct;

const int MAX_CODE_LENGTH  = 1000;
const int MAX_FUNCS_AMOUNT = 20;
const int MAX_EXT_VAR_AMOUNT  = 30;
const int MAX_FUNC_VAR_AMOUNT = 30;
const int MAX_FUNC_RECIEVED_VAR = 5;
const int MAX_CMD_SIZE = 50;
const int MAX_STR_SIZE = 50;

int LexStructsInit(LexStruct *lex_structs);
int LexStructDtor(LexStruct *lex_structs, unsigned int struct_amount);
LexStruct *LexicalAnalisis(char *buf, unsigned int *lex_structs_amount);

double ConvertStrToNum(const char *string);
const char **FindFuncNames(const LexStruct *lex_structs, unsigned int *func_amount);

int ProcessLexValue(LexStruct *lex_structs, char *value, char **buf, unsigned int *index);
//int ProcessExpression(LexStruct *lex_structs, char **buf, unsigned int *index);
int ProcessVarInit(LexStruct *lex_structs, char **buf, unsigned int *index);
int ProcessFunction(LexStruct *lex_structs, char **buf, unsigned int *index);
int ProcessArg(LexStruct *lex_structs, unsigned int *index, char *str);

int LexicalDump(LexStruct *lex_structs, char *buf, int buf_len);

struct TreeNode *ReadLexCode(const LexStruct *lex_structs);
struct TreeNode *GetExternal(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetFuncDef(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetVar(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetStatement(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetStatementStream(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetIf(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetWhile(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetStr(const LexStruct *lex_structs, unsigned int *index);

struct TreeNode *GetExp(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetAdd(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetMul(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetPow(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetUnar(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetBrackets(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetArg(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetInOut(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetFunc(const LexStruct *lex_structs, unsigned int *index);
struct TreeNode *GetParam(const LexStruct *lex_structs, unsigned int *index);

#endif