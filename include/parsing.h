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


#define LEX_SKIP_SPACE(buf)                                 \
            while(isspace(*(buf)) || *(buf) == '\0')        \
                (buf)++;                                    

#define LEX_ERROR(cond, buf_pos)                            \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                printf("ERROR in symb %d\n", buf_pos);      \
                if (cond)                                   \
                    return buf_pos;                         \
            } while(false)

enum LexType
{
    LT_NIL = 0,
    LT_ST  = 1,
    LT_VAR = 2,

    LT_IF  = 3,
    LT_ELSE = 4,
    LT_WHILE = 5,

    LT_FUNC = 6,
    LT_RET  = 7,
    LT_CALL = 8,
    LT_PARAM = 9,

    LT_NUM  = 10,
    LT_EQ   = 11,
    LT_OP   = 12,
    
    LT_STR  = 13,
    
    LT_L_BRCKT = 14,
    LT_R_BRCKT = 15,
};


struct LangLexicalElem
{
    LexType  type      = LT_NIL;
    ArithmOp arithm_op = OP_PSN;
    num_t    num       = NUM_PSN;
    const char *str    = NULL;

    unsigned int struct_ip = 0;
};

enum LexError
{
    ERROR_FULL_LEX_STRUCT  = 1,
    ERROR_LONG_STR         = 2,
    ERROR_PROCESS_VAR_INIT = 3,
};

typedef struct LangLexicalElem LexStruct;

const int MAX_CODE_LENGTH = 1000;
const int MAX_CMD_SIZE = 25;
const int MAX_STR_SIZE = 20;

int InitializeLexStructs(LexStruct *lex_structs);

LexStruct *LexicalAnalisis(char *buf);

double ConvertStrToNum(const char *string);
int ProcessLexArg(LexStruct *lex_structs, char *value, char **buf, unsigned int *index);



#endif