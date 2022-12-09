#ifndef LANGUAGE_H_INCLUDED
#define LANGUAGE_H_INCLUDED

/*
#include "../cpu_functions/assembler/include/asm.h"
#include "../cpu_functions/cpu/include/cpu.h"
#include "../cpu_functions/disasm/include/disasm.h"*/

#include "../tree_functions/include/tree.h"
#include "math.h"
#include "common.h"

enum NodeType
{
    T_NIL = 0,
    T_ST  = 1,
    T_VAR = 2,

    T_IF  = 3,
    T_ELSE = 4,
    T_WHILE = 5,

    T_FUNC = 6,
    T_RET  = 7,
    T_CALL = 8,
    T_PARAM = 9,

    T_NUM  = 10,
    T_EQ   = 11,
    T_OP   = 12,

    T_STR  = 13,
};

enum ArithmOp
{
    OP_PSN = 0,
    OP_ADD = 1,
    OP_SUB = 2,
    OP_MUL = 3,
    OP_DIV = 4,
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



#include "parsing.h"

#endif //LANGUAGE_H_INCLUDED