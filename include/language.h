#ifndef LANGUAGE_H_INCLUDED
#define LANGUAGE_H_INCLUDED


#include <windows.h>
/*
#include "../cpu_functions/assembler/include/asm.h"
#include "../cpu_functions/cpu/include/cpu.h"
#include "../cpu_functions/disasm/include/disasm.h"*/

#include "../tree_functions/include/tree.h"
#include "math.h"
#include "common.h"
#include "rus_trans.h"


static const char *const       LOG_FILE_PATH = "io/log.txt";
static const char *const   CODE_IN_FILE_PATH = "io/code_in.txt";
static const char *const  CODE_OUT_FILE_PATH = "io/code_out.txt";
static const char *const ASM_INPUT_FILE_PATH = "io/asm_input.txt";

enum LangError
{
    ERROR_PRINT_LANG_NODE = 1,
    ERROR_PRINT_EXT_ST    = 2,
    ERROR_PRINT_EXP       = 3,
    ERROR_PRINT_CALL      = 4,
    ERROR_PRINT_VAL       = 5,
};

enum NodeType
{
    T_NIL  = 0,
    T_ST   = 1,
    T_VAR  = 2,

    T_IF   = 3,
    T_ELSE  = 4,
    T_WHILE = 5,

    T_FUNC  = 6,
    T_TYPE  = 7,
    T_VOID  = 8,
    T_RET   = 9,
    T_CALL  = 10,
    T_PARAM = 11,

    T_NUM   = 12,
    T_EQ    = 13,
    T_OP    = 14,

    T_IN    = 15,
    T_OUT   = 16,

    T_STR   = 17,
};

enum ArithmOp
{
    OP_PSN = 0,
    OP_ADD = 1,
    OP_SUB = 2,
    OP_MUL = 3,
    OP_DIV = 4,
    OP_POW = 5,
    OP_SIN = 6,
    OP_COS = 7,
    OP_TG  = 8,
};


typedef double num_t;
const num_t NUM_PSN = NAN;

static const char *PSN_STR = "";

struct LangNode
{
    NodeType type      = T_NIL;
    ArithmOp arithm_op = OP_PSN;
    num_t    num       = NUM_PSN;
    const char *str    = NULL;
};
typedef struct LangNode LangNode;

struct LangContext
{
    struct Tree *tree = NULL;
    struct Var  *vars = NULL;
};

struct Var
{
    const char  *name   = PSN_STR;
    unsigned int adress = 0;
};

static const char *S_VAR    = "var";//"���_������";
static const char *S_ST_SEP = ";";//"���_����";
static const char *S_IF     = "if";//"������?";
static const char *S_ELSE   = "else";//"������!";
static const char *S_WHILE  = "while";//"������";
static const char *S_TYPE   = "type";//"���_�_�����";
static const char *S_VOID   = "void";//"���_���_����";
static const char *S_RET    = "return";//"��������";
static const char *S_EQ     = "=";//"���_���";

static const char *S_IN     = "scanf";//"�������_��_�����_��������";
static const char *S_OUT    = "printf";//"����_��������";

static const char *S_EXP_L_BRCKT = "(";//"��_��_�����";
static const char *S_EXP_R_BRCKT = ")";//"�����_������";
static const char *S_ST_L_BRCKT  = "{";//"�����_���";
static const char *S_ST_R_BRCKT  = "}";//"�����_���";

static const char *S_ADD = "+";//"��������";
static const char *S_SUB = "-";//"���������";
static const char *S_MUL = "*";//"�������";
static const char *S_DIV = "/";//"���������";
static const char *S_POW = "pow";//"��������";
static const char *S_SIN = "sin";//"������";
static const char *S_COS = "cos";//"�������";
static const char *S_TG  = "tg";//"��������";

struct Tree *LangTreeDeserialize(const char *input_file_name);

int GenerateLangCode(struct Tree *tree);

int PrintExt(FILE* code_f, struct TreeNode *node);
int PrintExtSt(FILE* code_f, struct TreeNode *node);
int PrintVar(FILE* code_f, struct TreeNode *node);
int PrintExp(FILE* code_f, struct TreeNode *node);
int PrintVal(FILE* code_f, struct TreeNode *node);
int PrintCallFunc(FILE* code_f, struct TreeNode *node);
int PrintFuncDef(FILE* code_f, struct TreeNode *node);
int PrintStStream(FILE* code_f, TreeNode *node, unsigned int *recur_lvl);
int PrintSt(FILE* code_f, TreeNode *node, unsigned int *recur_lvl);
int PrintIf(FILE* code_f, TreeNode *node, unsigned int *recur_lvl);
int PrintWhile(FILE* code_f, TreeNode *node, unsigned int *recur_lvl);

#include "parsing.h"

#endif //LANGUAGE_H_INCLUDED