
#include "../include/language.h"

#define NTYPE(node) node->value->type
#define NSTR(node) node->value->str
#define NNUM(node) node->value->num
#define NOP(node) node->value->arithm_op

int GenerateAsmFile(struct Tree *tree)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(tree->root == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(tree->root->value->type != T_ST, ERROR_SYNTAX);

    LangContext *l_context = LangContextCtor();
    ERROR_CHECK(l_context == NULL, ERROR_LANG_CONTEX_CTOR);

    unsigned int func_amount = 0;
    l_context->funcs = DefFuncs(tree->root, &l_context->funcs_amount);
    SAFE_ERROR_CHECK(l_context->funcs == NULL, LangContextDtor(l_context);, ERROR_DEF_FUNC);

    FILE *asmfile = fopen(ASM_IN_FILE_PATH, "w");
    SAFE_ERROR_CHECK(asmfile == NULL, LangContextDtor(l_context);, ERROR_OPENING_FILE);

    int prep_asm_err = PrepareAsm(asmfile, tree->root, l_context);
    SAFE_ERROR_CHECK(prep_asm_err, LangContextDtor(l_context);, ERROR_PREPARE_ASM);

    int gen_asm_err = AsmExt(asmfile, tree->root, l_context);
    SAFE_ERROR_CHECK(gen_asm_err, LangContextDtor(l_context);, ERROR_ASM_EXT);

    int fclose_err = fclose(asmfile);
    ERROR_CHECK(fclose_err, ERROR_CLOSING_FILE);

    return SUCCESS;
}

int PrepareAsm(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    fprintf(asmfile, "PUSH 0\n");
    fprintf(asmfile, "POP rix\n\n");

    fprintf(asmfile, "JMP :main\n\n");

    fprintf(asmfile, "INCRIX:\n");
    fprintf(asmfile, "PUSH rix\n");
    fprintf(asmfile, "PUSH 1\n");
    fprintf(asmfile, "ADD\n");
    fprintf(asmfile, "POP rix\n");
    fprintf(asmfile, "RET\n");

    return SUCCESS;
}

LangContext *LangContextCtor(void)
{
    LangContext *l_context = (LangContext *)calloc(1, sizeof(LangContext));
    ERROR_CHECK(l_context == NULL, NULL);

    l_context->funcs = NULL;

    l_context->ext_vars = VarStructsCtor(MAX_EXT_VAR_AMOUNT);
    SAFE_ERROR_CHECK(l_context->funcs == NULL, free(l_context);, NULL);
    l_context->ext_vars_amount = 0;

    l_context->rix = 0;

    return l_context;
}

int LangContextDtor(LangContext *l_context)
{
    ERROR_CHECK(l_context == NULL, NULL);

    if (l_context->funcs != NULL)
    {
        for (unsigned int i = 0; i < l_context->funcs_amount; i++)
        {
            if (l_context->funcs[i].vars != NULL)
                free(l_context->funcs[i].vars);
        }
        free(l_context->funcs);
    }

    if (l_context->ext_vars != NULL)
        free(l_context->ext_vars);

    free(l_context);

    return SUCCESS;
}

Var *VarStructsCtor(unsigned int var_amount)
{
    Var *vars = (Var *)calloc(var_amount, sizeof(Var));
    ERROR_CHECK(vars == NULL, NULL);

    for (int i = 0; i < var_amount; i++)
    {
        vars[i].name = NULL;
        vars[i].adress = 0;
    }

    return vars;
}

int FuncStructsInit(Func *funcs)
{
    ERROR_CHECK(funcs == NULL, NULL);

    for (int i = 0; i < MAX_FUNCS_AMOUNT; i++)
    {
        funcs[i].name = NULL;
        funcs[i].ret_type = T_VOID;
        funcs[i].vars_amount = 0;
        funcs[i].vars = VarStructsCtor(MAX_FUNC_VAR_AMOUNT);
        ERROR_CHECK(funcs[i].vars == NULL, NULL);
    }

    return SUCCESS;
}

int FuncsDtor(Func *funcs, unsigned int funcs_amount)
{
    ERROR_CHECK(funcs == NULL, NULL);

    for (unsigned int i = 0; i < funcs_amount; i++)
    {
        if (funcs[i].vars != NULL)
            free(funcs[i].vars);
    }

    free(funcs);

    return SUCCESS;
}

struct Func *DefFuncs(TreeNode *st_node, unsigned int *func_amount)
{
    ERROR_CHECK(st_node == NULL, NULL);
    ERROR_CHECK(func_amount == NULL, NULL);

    printf("enter def_func\n");

    Func *funcs = (Func *)calloc(MAX_FUNCS_AMOUNT, sizeof(Func));
    ERROR_CHECK(funcs == NULL, NULL);

    int init_err = FuncStructsInit(funcs);
    SAFE_ERROR_CHECK(init_err, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);

    unsigned int index = 0;
    TreeNode *extra_node = st_node;
    while (true)
    {
        if (extra_node == NULL)
            break;

        SAFE_ERROR_CHECK(NTYPE(extra_node) != T_ST, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
        SAFE_ERROR_CHECK(extra_node->left == NULL, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);

        if (NTYPE(extra_node->left) == T_VAR)
        {
            extra_node = extra_node->right;
            continue;
        }

        TreeNode *func_node = extra_node->left;
        SAFE_ERROR_CHECK(NTYPE(func_node) != T_FUNC, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);

        SAFE_ERROR_CHECK(func_node->left == NULL, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
        SAFE_ERROR_CHECK(NTYPE(func_node->left) != T_STR, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
        SAFE_ERROR_CHECK(NSTR(func_node->left) == NULL, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
        funcs[index].name = NSTR(func_node->left);

        SAFE_ERROR_CHECK(func_node->left->right == NULL, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
        SAFE_ERROR_CHECK(NTYPE(func_node->left->right) != T_TYPE &&
                             NTYPE(func_node->left->right) != T_VOID,
                         FuncsDtor(funcs, MAX_FUNCS_AMOUNT);
                         , NULL);
        funcs[index].ret_type = NTYPE(func_node->left->right);

        unsigned int var_count = 0;
        TreeNode *param_node = func_node->left->left;
        while (true)
        {
            if (param_node == NULL)
                break;
            SAFE_ERROR_CHECK(param_node->left == NULL, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
            SAFE_ERROR_CHECK(NTYPE(param_node->left) != T_VAR, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
            SAFE_ERROR_CHECK(NTYPE(param_node->left->left) != T_STR, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
            SAFE_ERROR_CHECK(NSTR(param_node->left->left) == NULL, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);
            param_node = param_node->right;
            var_count++;
        }
        funcs[index].vars_amount = var_count;

        extra_node = extra_node->right;
        index++;
    }

    Func *reall_funcs = (Func *)realloc(funcs, sizeof(Func) * index);
    SAFE_ERROR_CHECK(reall_funcs == NULL, FuncsDtor(funcs, MAX_FUNCS_AMOUNT);, NULL);

    *func_amount = index;
    printf("func_names: \n");
    for (unsigned int i = 0; i < index; i++)
    {
        if (reall_funcs[i].name != NULL)
            printf("%s\n", reall_funcs[i].name);
    }

    printf("exit def_func_names\n");

    return reall_funcs;
}

#define EXT_VARS l_cntxt->ext_vars
#define EXT_VARS_NUM l_cntxt->ext_vars_amount
#define FUNCS l_cntxt->funcs
#define FUNCS_NUM l_cntxt->funcs_amount
#define RIX l_cntxt->rix

#define INC_RIX                           \
    fprintf(asmfile, "\nCALL :INCRIX\n"); \
    l_cntxt->rix++

int AsmExt(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    printf("enter AsmExt\n");

    TreeNode *extra_node = node;
    while (true)
    {
        if (extra_node == NULL)
            break;
        ERROR_CHECK(NTYPE(extra_node) != T_ST, ERROR_SYNTAX);

        int err = AsmExtSt(asmfile, extra_node->left, l_cntxt);
        ERROR_CHECK(err, ERROR_PRINT_EXT_ST);

        extra_node = extra_node->right;
    }

    printf("exit AsmExt\n");

    return SUCCESS;
}

int AsmExtSt(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    printf("enter AsmExtSt\n");

    if (node == NULL)
        return SUCCESS;

    int err = 0;
    switch (NTYPE(node))
    {
    case T_VAR:
        err = AsmExtVarDef(asmfile, node, l_cntxt);
        ERROR_CHECK(err, ERROR_PRINT_VAR);
        break;

    case T_FUNC:
        err = AsmFuncDef(asmfile, node, l_cntxt);
        ERROR_CHECK(err, ERROR_PRINT_FUNC_DEF);
        break;

    default:
        fprintf(asmfile, " EXT_ST_ERROR(%d) ", NTYPE(node));
        break;
    }

    printf("exit AsmExtSt\n");

    return SUCCESS;
}

int AsmExtVarDef(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    printf("enter AsmVar\n");

    ERROR_CHECK(node->left == NULL, ERROR_SYNTAX);
    ERROR_CHECK(node->right == NULL, ERROR_SYNTAX);

    ERROR_CHECK(NSTR(node->left) == NULL, ERROR_NULL_PTR);

    EXT_VARS[EXT_VARS_NUM].name = NSTR(node->left);

    int pr_exp_err = AsmExp(asmfile, node->right, l_cntxt);
    ERROR_CHECK(pr_exp_err, ERROR_PRINT_EXP);

    fprintf(asmfile, "POP [rix]\n\n\n");
    EXT_VARS[EXT_VARS_NUM].adress = RIX;
    INC_RIX;
    EXT_VARS_NUM++;

    printf("exit AsmVar\n");

    return SUCCESS;
}

int AsmExp(FILE *code_f, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    printf("enter AsmExp\n");

    if (node == NULL)
        return SUCCESS;

    int err = 0;

    err = AsmExp(code_f, node->left, l_cntxt);
    ERROR_CHECK(err, ERROR_PRINT_EXP);

    err = AsmExp(code_f, node->right, l_cntxt);
    ERROR_CHECK(err, ERROR_PRINT_EXP);

    err = ProcAsmVal(code_f, node, l_cntxt);
    ERROR_CHECK(err, ERROR_PRINT_VAL);

    printf("enter AsmExp\n");

    return SUCCESS;
}

int ProcAsmVal(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    printf("enter AsmVal\n");

    int err = 0;
    switch (NTYPE(node))
    {
        case T_NUM: fprintf(asmfile, "PUSH %g\n", NNUM(node));
                    break;

        case T_STR: ERROR_CHECK(NSTR(node) == NULL, ERROR_NULL_PTR); //.....
                    int err = ProcAsmVar(asmfile, node, l_cntxt);
                    ERROR_CHECK(err, ERROR_PROC_ASM_STR);
                    break;

        case T_OP : switch (NOP(node))
                    {
                        case OP_ADD: fprintf(asmfile, "ADD\n");
                                     break;
                        case OP_SUB: fprintf(asmfile, "SUB\n");
                                     break;
                        case OP_MUL: fprintf(asmfile, "MUL\n");
                                     break;
                        case OP_DIV: fprintf(asmfile, "DIV\n");
                                     break;
                        case OP_POW: fprintf(asmfile, "POW\n");
                                     break;
                        case OP_SIN: fprintf(asmfile, "SIN\n");
                                     break;
                        case OP_COS: fprintf(asmfile, "COS\n");
                                     break;
                        case OP_TG:  fprintf(asmfile, "COS\n");
                                     break;
                        default:     fprintf(asmfile, "OP_ERROR(%d) ", NOP(node));
                                     break;
                    }
                    break;

        case T_CALL: err = AsmCallFunc(asmfile, node->left, l_cntxt);
                     ERROR_CHECK(err, ERROR_PRINT_CALL);
                     break;

        default:     fprintf(asmfile, "EXP_ERROR(%d) ", NTYPE(node));
                     break;
    }

    printf("exit AsmVal\n");

    return SUCCESS;
}

// int ProcAsmVar(FILE* asmfile, TreeNode *node, LangContext *l_cntxt)
// {
//     ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
//     ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

//     return SUCCESS;
// }

int AsmCallFunc(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    printf("enter AsmCallFunc\n");

    ERROR_CHECK(node == NULL, ERROR_SYNTAX);
    ERROR_CHECK(node->right != NULL, ERROR_SYNTAX);

    ERROR_CHECK(NTYPE(node) != T_STR, ERROR_SYNTAX);
    ERROR_CHECK(NSTR(node) == NULL, ERROR_NULL_PTR);

    int f_index = FindFunc(NSTR(node), l_cntxt);
    ERROR_CHECK(f_index < 0, ERROR_SYNTAX);

    unsigned int index = 0;
    TreeNode *extra_node = node->left;
    while (true)
    {
        if (extra_node == NULL)
            break;

        ERROR_CHECK(NTYPE(extra_node) != T_PARAM, ERROR_SYNTAX);

        int err = AsmExp(asmfile, extra_node->left, l_cntxt);
        ERROR_CHECK(err, ERROR_PRINT_EXP);

        fprintf(asmfile, "POP [rix]\n\n");
        INC_RIX;

        extra_node = extra_node->right;
    }

    fprintf(asmfile, "CALL :%s\n\n", FUNCS[f_index].name);

    printf("exit AsmCallFunc\n");

    return SUCCESS;
}

int FindFunc(const char *str, LangContext *l_cntxt)
{
    ERROR_CHECK(str == NULL, -1);
    ERROR_CHECK(l_cntxt == NULL, -1);

    int index = 0;
    for (; index < FUNCS_NUM; index++)
    {
        ERROR_CHECK(FUNCS[index].name == NULL, ERROR_NULL_PTR);
        if (strcmp(str, FUNCS[index].name) == 0)
            break;
    }

    if (index == FUNCS_NUM)
        return -1;

    return index;
}

int AsmFuncDef(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    printf("enter AsmFuncDef\n");

    ERROR_CHECK(node->left == NULL, ERROR_SYNTAX);
    ERROR_CHECK(NTYPE(node->left) != T_STR, ERROR_SYNTAX);
    ERROR_CHECK(NSTR(node->left)  == NULL, ERROR_NULL_PTR);

    int find_func = FindFunc(NSTR(node->left), l_cntxt);
    ERROR_CHECK(find_func >= 0, ERROR_SYNTAX);

    FUNCS[FUNCS_NUM].name = NSTR(node->left);
    fprintf(asmfile, "%s:\n", FUNCS[FUNCS_NUM].name);

    ERROR_CHECK(node->left->right == NULL, ERROR_SYNTAX);
    ERROR_CHECK(!(NTYPE(node->left->right) == T_TYPE || 
                  NTYPE(node->left->right) == T_VOID), ERROR_SYNTAX);

    FUNCS[FUNCS_NUM].ret_type = NTYPE(node->left->right);

    int var_count = 0;
    TreeNode *extra_node = node->left->left;
    while (true)
    {
        if (extra_node == NULL)
            break;
        ERROR_CHECK(NTYPE(extra_node) != T_PARAM, ERROR_SYNTAX);

        ERROR_CHECK(      extra_node->left  == NULL,  ERROR_NULL_PTR);
        ERROR_CHECK(NTYPE(extra_node->left) != T_VAR, ERROR_SYNTAX);

        ERROR_CHECK(      extra_node->left->left  == NULL,  ERROR_NULL_PTR);
        ERROR_CHECK(      extra_node->left->right != NULL,  ERROR_SYNTAX);
        ERROR_CHECK(NTYPE(extra_node->left->left) != T_STR, ERROR_SYNTAX);
        ERROR_CHECK( NSTR(extra_node->left->left) == NULL,  ERROR_NULL_PTR);

        FUNCS[FUNCS_NUM].vars[var_count].name = NSTR(extra_node->left->left);
        var_count++;

        extra_node = extra_node->right;
    }

    unsigned int recur_lvl = 0;
    int err = AsmStStream(asmfile, node->right, l_cntxt);
    ERROR_CHECK(err, ERROR_ASM_ST_STREAM);

    printf("exit AsmFuncDef\n");

    return SUCCESS;
}


int AsmStStream(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    printf("enter AsmStStream\n");

    TreeNode *extra_node = node;
    while (true)
    {
        if (extra_node == NULL)
            break;
        ERROR_CHECK(NTYPE(extra_node) != T_ST, ERROR_SYNTAX);

        int err = AsmSt(asmfile, extra_node->left, l_cntxt);
        ERROR_CHECK(err, ERROR_PRINT_ST);

        extra_node = extra_node->right;
    }

    printf("exit AsmStStream\n");

    return SUCCESS;
}

int AsmSt(FILE *asmfile, TreeNode *node, LangContext *l_cntxt)
{
    ERROR_CHECK(asmfile == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(l_cntxt == NULL, ERROR_NULL_PTR);

    if (node == NULL)
        return SUCCESS;

    printf("enter AsmSt\n");

    int err = 0;
    switch (NTYPE(node))
    {
        case T_IF:      err = AsmIf(asmfile, node, l_cntxt);
                        ERROR_CHECK(err, ERROR_ASM_IF);
                        break;

        case T_WHILE:   err = AsmWhile(asmfile, node, l_cntxt);
                        ERROR_CHECK(err, ERROR_ASM_WHILE);
                        break;

        case T_VAR:     err = AsmLocVar(asmfile, node, l_cntxt);
                        ERROR_CHECK(err, ERROR_ASM_VAR);
                        break;

        case T_RET:     ERROR_CHECK(node->left  == NULL, ERROR_SYNTAX);
                        ERROR_CHECK(node->right != NULL, ERROR_SYNTAX);

                        err = AsmExp(asmfile, node->left, l_cntxt);
                        ERROR_CHECK(err, ERROR_ASM_EXP);
                        fprintf(asmfile, "POP rhx\n");
                        break;

        case T_EQ:      err = AsmExp(asmfile, node, l_cntxt);
                        ERROR_CHECK(err, ERROR_PRINT_EXP);

        case T_NUM: // falling
        case T_STR: // falling
        case T_OP:  // falling
        case T_CALL:    err = AsmExp(asmfile, node, l_cntxt);
                        ERROR_CHECK(err, ERROR_PRINT_EXP);
                        fprintf(code_f, " %s\n", S_ST_SEP);
                        break;

        case T_IN:      
        case T_OUT:     err = AsmInOut(code_f, node, l_cntxt);
                        ERROR_CHECK(err, ERROR_PRINT_CALL);
                        fprintf(code_f, " %s\n", S_ST_SEP);
                        break;

        default:        fprintf(code_f, " ERROR_PRINT_ST(%d) ", (NTYPE(node)));
                        break;
    }

    printf("exit AsmSt\n");

    return SUCCESS;
}

int AsmIf(FILE *code_f, TreeNode *node, unsigned int *recur_lvl)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(node == NULL, ERROR_NULL_PTR);

    printf("enter AsmIf\n");

    fprintf(code_f, "%s ", S_IF);

    fprintf(code_f, "%s ", S_EXP_L_BRCKT);
    int err = AsmExp(code_f, node->left);
    ERROR_CHECK(err, ERROR_PRINT_EXP);
    fprintf(code_f, " %s\n", S_EXP_R_BRCKT);

    ERROR_CHECK(node->right == NULL, ERROR_NULL_PTR);

    TreeNode *true_node = NULL;
    TreeNode *false_node = NULL;
    bool with_else = false;
    if (NTYPE(node->right) == T_ELSE)
    {
        with_else = true;
        true_node = node->right->left;
        false_node = node->right->right;
    }
    else
        true_node = node->right;

    TABS(*recur_lvl);
    fprintf(code_f, "%s\n", S_ST_L_BRCKT);

    ERROR_CHECK(true_node == NULL, ERROR_NULL_PTR);
    err = AsmStStream(code_f, true_node, recur_lvl);
    ERROR_CHECK(err, ERROR_PRINT_ST_STREAM);

    TABS(*recur_lvl);
    fprintf(code_f, "%s\n\n", S_ST_R_BRCKT);

    if (with_else)
    {
        TABS(*recur_lvl);
        fprintf(code_f, "%s\n", S_ELSE);

        TABS(*recur_lvl);
        fprintf(code_f, "%s\n", S_ST_L_BRCKT);

        ERROR_CHECK(false_node == NULL, ERROR_NULL_PTR);
        err = AsmStStream(code_f, false_node, recur_lvl);
        ERROR_CHECK(err, ERROR_PRINT_ST_STREAM);

        TABS(*recur_lvl);
        fprintf(code_f, "%s\n\n", S_ST_R_BRCKT);
    }

    (*recur_lvl)--;
    printf("exit AsmIf\n");

    return SUCCESS;
}

int AsmWhile(FILE *code_f, TreeNode *node, unsigned int *recur_lvl)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(node == NULL, ERROR_NULL_PTR);

    printf("enter AsmWhile\n");

    fprintf(code_f, " %s ", S_WHILE);

    fprintf(code_f, " %s ", S_EXP_L_BRCKT);
    int err = AsmExp(code_f, node->left);
    ERROR_CHECK(err, ERROR_PRINT_EXP);
    fprintf(code_f, " %s\n", S_EXP_R_BRCKT);

    ERROR_CHECK(node->right == NULL, ERROR_NULL_PTR);

    TreeNode *true_node = node->right;

    fprintf(code_f, "%s\n", S_ST_L_BRCKT);

    ERROR_CHECK(true_node == NULL, ERROR_NULL_PTR);
    err = AsmStStream(code_f, true_node, recur_lvl);
    ERROR_CHECK(err, ERROR_PRINT_ST_STREAM);

    fprintf(code_f, "%s\n\n", S_ST_R_BRCKT);

    printf("exit AsmWhile\n");

    return SUCCESS;
}
