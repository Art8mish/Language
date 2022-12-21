
#include "../include/language.h"

struct Tree *LangTreeDeserialize(const char *input_file_name)
{
    ERROR_CHECK(input_file_name == NULL, NULL);

    struct Tree *new_tree = TreeCtor();
    ERROR_CHECK(new_tree == NULL, NULL);

    struct WorkingField *onegin_context = CreateWorkingField(input_file_name);
    ERROR_CHECK(onegin_context == NULL, NULL);

    char *buf = onegin_context->chars_buffer;
    buf[onegin_context->chars_amount] = EOF;
    char *buf_cpy = buf;

    printf("buf: \n");
    char *print_buf = buf;
    int i = 0;
    while (i < onegin_context->chars_amount)
    {
        if (*print_buf == '\0')
            printf("\n");
        else
            printf("%c", *print_buf);
        print_buf++;
        i++;
    }
    printf("code end...\n");

    unsigned int structs_amount = 0;
    LexStruct *lex_structs = LexicalAnalisis(buf, &structs_amount);
    SAFE_ERROR_CHECK(lex_structs == NULL, DestructWorkingField(onegin_context);, NULL);

    printf("structs_amount = %d\n", structs_amount);

    int lex_dump_err = LexicalDump(lex_structs, buf_cpy, onegin_context->chars_amount);
    SAFE_ERROR_CHECK(lex_dump_err, DestructWorkingField(onegin_context); 
                                   free(lex_structs);, NULL);
   
    printf("start readlexcode...\n");
    new_tree->root = ReadLexCode(lex_structs);
    SAFE_ERROR_CHECK(new_tree->root == NULL, DestructWorkingField(onegin_context); 
                                             free(lex_structs);, NULL);
    printf("end readlexcode...\n");

    int ongn_dtor_err = DestructWorkingField(onegin_context);
    SAFE_ERROR_CHECK(ongn_dtor_err, free(lex_structs);, NULL);
    free(lex_structs);

    return new_tree;    
}


int LexicalDump(LexStruct *lex_structs, char *buf, int buf_len)
{
    ERROR_CHECK(lex_structs == NULL, ERROR_NULL_PTR);

    FILE *log_f = fopen(LOG_FILE_PATH, "a");
    ERROR_CHECK(log_f == NULL, ERROR_NULL_PTR);

//print_buf
    fprintf(log_f, "buf: \n");
    for (int i = 0; i < buf_len; i++)
    {
        if (*buf == '\0')
            fprintf(log_f, "\n");
        else
            fprintf(log_f, "%c", *buf);
        buf++;
    }
    fprintf(log_f, "\n\n\n");

//print lex_structs
    fprintf(log_f, "lex_structs: \n");

    unsigned int index = 0;
    while (lex_structs[index].type != LT_END)
    {
        switch (lex_structs[index].type)
        {
            case LT_NIL   : fprintf(log_f, "[%d]: NIL\n", index);
                            break;

            case LT_ST    : fprintf(log_f, "[%d]: ST\n", index);
                            break;

            case LT_VAR   : fprintf(log_f, "[%d]: VAR\n", index);
                            break;

            case LT_IF    : fprintf(log_f, "[%d]: IF\n", index);
                            break;

            case LT_ELSE  : fprintf(log_f, "[%d]: ELSE\n", index);
                            break;

            case LT_WHILE : fprintf(log_f, "[%d]: WHILE\n", index);
                            break;
            
            case LT_TYPE_FUNC  : fprintf(log_f, "[%d]: VAR_FUNC\n", index);
                            break;
            
            case LT_VOID_FUNC : fprintf(log_f, "[%d]: VOID_FUNC\n", index);
                            break;

            case LT_RET   : fprintf(log_f, "[%d]: RET\n", index);
                            break;

            case LT_CALL  : fprintf(log_f, "[%d]: CALL\n", index);
                            break;

            case LT_PARAM : fprintf(log_f, "[%d]: PARAM\n", index);
                            break;

            case LT_NUM   : fprintf(log_f, "[%d]: NUM: %g\n", index, lex_structs[index].num);
                            break;

            case LT_EQ    : fprintf(log_f, "[%d]: EQ\n", index);
                            break;

            case LT_OP    : fprintf(log_f, "[%d]: OP: %d\n", index, lex_structs[index].arithm_op);
                            break;

            case LT_IN    : fprintf(log_f, "[%d]: IN\n", index);
                            break;

            case LT_OUT   : fprintf(log_f, "[%d]: OUT\n", index);
                            break;

            case LT_STR   : fprintf(log_f, "[%d]: STR: %s (func_name: %d)\n", 
                                    index, lex_structs[index].str, lex_structs[index].func_name);
                            break;

            case LT_EXP_L_BRCKT    : fprintf(log_f, "[%d]: (\n", index);
                            break;

            case LT_EXP_R_BRCKT    : fprintf(log_f, "[%d]: )\n", index);
                            break;

            case LT_STREAM_L_BRCKT : fprintf(log_f, "[%d]: {\n", index);
                            break;

            case LT_STREAM_R_BRCKT : fprintf(log_f, "[%d]: }\n", index);
                            break;

            case LT_ST_SEP : fprintf(log_f, "[%d]: ;\n", index);
                            break;

            case LT_COMMA  : fprintf(log_f, "[%d]: ,\n", index);
                            break;

            case LT_END    : fprintf(log_f, "[%d]: END\n", index);
                            break;

            default        : fprintf(log_f, "[%d]: ERROR\n", index);
                            break;
        }

        index++;
    }

    int fclose_err = fclose(log_f);
    ERROR_CHECK(fclose_err, ERROR_CLOSING_FILE);

    return SUCCESS;
}


#define NTYPE(node) node->value->type
#define NSTR(node) node->value->str
#define NNUM(node) node->value->num
#define NOP(node) node->value->arithm_op


int GenerateLangCode(struct Tree *tree)
{
    ERROR_CHECK(tree       == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(tree->root == NULL, ERROR_NULL_PTR);

    FILE *code_f = fopen(CODE_OUT_FILE_PATH, "w");
    ERROR_CHECK(code_f == NULL, ERROR_OPENING_FILE);

    SAFE_ERROR_CHECK(tree->root->value->type != T_ST, fclose(code_f);, ERROR_SYNTAX);

    int print_lnode_err = PrintExt(code_f, tree->root);
    SAFE_ERROR_CHECK(print_lnode_err, fclose(code_f);, ERROR_PRINT_LANG_NODE);

    int fclose_err = fclose(code_f);
    ERROR_CHECK(fclose_err, ERROR_CLOSING_FILE);

    return SUCCESS;
}

int PrintExt(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(node == NULL, ERROR_SYNTAX);

    printf("enter PrintExt\n");

    TreeNode *extra_node = node;
    while (true)
    {
        if (extra_node == NULL)
            break;
        ERROR_CHECK(NTYPE(extra_node) != T_ST, ERROR_SYNTAX);

        int err = PrintExtSt(code_f, extra_node->left);
        ERROR_CHECK(err, ERROR_PRINT_EXT_ST);

        extra_node = extra_node->right;
    }

    printf("exit PrintExt\n");

    return SUCCESS;
}

int PrintExtSt(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    printf("enter PrintExtSt\n");

    if (node == NULL)
        return SUCCESS;

    int err = 0;
    switch (NTYPE(node))
    {
        case T_VAR  : err = PrintVar(code_f, node);
                      ERROR_CHECK(err, ERROR_PRINT_VAR);
                      break;

        case T_FUNC : err = PrintFuncDef(code_f, node);
                      ERROR_CHECK(err, ERROR_PRINT_FUNC_DEF);
                      break;

        default     : fprintf(code_f, " EXT_ST_ERROR(%d) ", NTYPE(node));
                      break;
        
    }

    printf("exit PrintExtSt\n");

    return SUCCESS;
}

int PrintVar(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    printf("enter PrintVar\n");

    ERROR_CHECK(node->left  == NULL, ERROR_SYNTAX);
    ERROR_CHECK(node->right == NULL, ERROR_SYNTAX);

    ERROR_CHECK(NSTR(node->left) == NULL, ERROR_NULL_PTR);
    fprintf(code_f, "%s %s %s ", S_VAR, NSTR(node->left), S_EQ);

    int pr_exp_err = PrintExp(code_f, node->right);
    ERROR_CHECK(pr_exp_err, ERROR_PRINT_EXP);

    fprintf(code_f, " %s\n\n", S_ST_SEP);

    printf("exit PrintVar\n");

    return SUCCESS;
}

int PrintExp(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    printf("enter PrintExp\n");

    if (node == NULL)
        return SUCCESS;

    int err= 0;
    if (!(NTYPE(node) == T_OP && 
         (NOP(node) == OP_POW || 
          NOP(node) == OP_SIN ||
          NOP(node) == OP_COS || 
          NOP(node) == OP_TG)) && !(NTYPE(node) == T_CALL))
    {
        err = PrintExp(code_f, node->left);
        ERROR_CHECK(err, ERROR_PRINT_EXP);
    }
                        
    err = PrintVal(code_f, node);
    ERROR_CHECK(err, ERROR_PRINT_VAL);

    if (!(NTYPE(node) == T_OP && 
         (NOP(node) == OP_POW)) &&
        !(NTYPE(node) == T_CALL))
    {
        err = PrintExp(code_f, node->right);
        ERROR_CHECK(err, ERROR_PRINT_EXP);
    }
    printf("enter PrintExp\n");

    return SUCCESS;
}

int PrintVal(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    printf("enter PrintVal\n");

    int err = 0;
    switch (NTYPE(node))
    {
        case T_NUM  : fprintf(code_f, "%g", NNUM(node));
                      break;

        case T_STR  : ERROR_CHECK(NSTR(node) == NULL, ERROR_NULL_PTR);
                      fprintf(code_f, "%s", NSTR(node));
                      break;

        case T_EQ   : fprintf(code_f, " %s ", S_EQ);
                      break;

        case T_OP   : switch (NOP(node))
                      {
                          case OP_ADD : fprintf(code_f, " %s ", S_ADD);
                                        break;
                          case OP_SUB : fprintf(code_f, " %s ", S_SUB);
                                        break;
                          case OP_MUL : fprintf(code_f, " %s ", S_MUL);
                                        break;
                          case OP_DIV : fprintf(code_f, " %s ", S_DIV);
                                        break;
                          case OP_POW : fprintf(code_f, "%s ( ", S_POW);
                                        err = PrintExp(code_f, node->left);
                                        ERROR_CHECK(err, ERROR_PRINT_EXP);
                                        fprintf(code_f, ", ");
                                        err = PrintExp(code_f, node->right);
                                        ERROR_CHECK(err, ERROR_PRINT_EXP);
                                        fprintf(code_f, " )");
                                        break;
                          case OP_SIN : fprintf(code_f, "%s ( ", S_SIN);
                                        err = PrintExp(code_f, node->left);
                                        ERROR_CHECK(err, ERROR_PRINT_EXP);
                                        fprintf(code_f, " )");
                                        break;
                          case OP_COS : fprintf(code_f, "%s ( ", S_COS);
                                        err = PrintExp(code_f, node->left);
                                        ERROR_CHECK(err, ERROR_PRINT_EXP);
                                        fprintf(code_f, " )");
                                        break;
                          case OP_TG  : fprintf(code_f, "%s ( ", S_TG);
                                        err = PrintExp(code_f, node->left);
                                        ERROR_CHECK(err, ERROR_PRINT_EXP);
                                        fprintf(code_f, " )");
                                        break;
                          default     : fprintf(code_f, "OP_ERROR(%d) ", NOP(node));
                                        break;
                      }
                      break;

        case T_CALL : err = PrintCallFunc(code_f, node->left);
                      ERROR_CHECK(err, ERROR_PRINT_CALL);
                      break;

        default     : fprintf(code_f, "EXP_ERROR(%d) ", NTYPE(node));
                      break;
    }

    printf("exit PrintVal\n");

    return SUCCESS;
}

int PrintCallFunc(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    printf("enter PrintCallFunc\n");

    ERROR_CHECK(node        == NULL, ERROR_SYNTAX);
    ERROR_CHECK(node->right != NULL, ERROR_SYNTAX);

    if (NTYPE(node) == T_IN)
        fprintf(code_f, "%s ", S_IN);
    else if (NTYPE(node) == T_OUT)
        fprintf(code_f, "%s ", S_OUT);
    else if (NTYPE(node) == T_STR)
    {
        ERROR_CHECK(NSTR(node) == NULL, ERROR_NULL_PTR);
        fprintf(code_f, "%s ", NSTR(node));
    }
    else
        return ERROR_SYNTAX;

    fprintf(code_f, "%s ", S_EXP_L_BRCKT);
    TreeNode *extra_node = node->left;
    while (true)
    {
        if (extra_node == NULL)
            break;

        if (NTYPE(node) == T_IN)
            fprintf(code_f, "&");

        int err = PrintExp(code_f, extra_node->left);
        ERROR_CHECK(err, ERROR_PRINT_EXP);

        extra_node = extra_node->right;

        if (extra_node != NULL)
            fprintf(code_f, ", ");
    }
    fprintf(code_f, " %s", S_EXP_R_BRCKT);

    printf("exit PrintCallFunc\n");

    return SUCCESS;
}


int PrintFuncDef(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    printf("enter PrintFuncDef\n");

    ERROR_CHECK(node->left        == NULL, ERROR_SYNTAX);
    ERROR_CHECK(node->left->right == NULL, ERROR_SYNTAX);

    if (NTYPE(node->left->right) == T_TYPE)
        fprintf(code_f, "%s ", S_TYPE);
    else if (NTYPE(node->left->right) == T_VOID)
        fprintf(code_f, "%s ", S_VOID);
    else
        return ERROR_SYNTAX;

    ERROR_CHECK(NTYPE(node->left) != T_STR, ERROR_SYNTAX);
    ERROR_CHECK(NSTR(node->left)  == NULL, ERROR_NULL_PTR);
    fprintf(code_f, "%s ", NSTR(node->left));

    fprintf(code_f, "%s ", S_EXP_L_BRCKT);
    TreeNode *extra_node = node->left->left;
    while (true)
    {
        if (extra_node == NULL)
            break;
        ERROR_CHECK(NTYPE(extra_node) != T_PARAM, ERROR_SYNTAX);

        ERROR_CHECK(extra_node->left  == NULL, ERROR_NULL_PTR);
        ERROR_CHECK(NTYPE(extra_node->left) != T_VAR, ERROR_SYNTAX);

        ERROR_CHECK(extra_node->left->left  == NULL, ERROR_NULL_PTR);
        ERROR_CHECK(extra_node->left->right != NULL, ERROR_SYNTAX);
        ERROR_CHECK(NTYPE(extra_node->left->left) != T_STR, ERROR_SYNTAX);
        ERROR_CHECK(NSTR(extra_node->left->left)  == NULL, ERROR_NULL_PTR);
        fprintf(code_f, "%s %s", S_VAR, NSTR(extra_node->left->left));

        extra_node = extra_node->right;

        if (extra_node != NULL)
            fprintf(code_f, ", ");
    }
    fprintf(code_f, " %s\n", S_EXP_R_BRCKT);


    fprintf(code_f, "%s\n", S_ST_L_BRCKT);

    unsigned int recur_lvl = 0;
    int err = PrintStStream(code_f, node->right, &recur_lvl);
    ERROR_CHECK(err, ERROR_PRINT_ST_STREAM);
    
    fprintf(code_f, "%s\n\n", S_ST_R_BRCKT);

    printf("exit PrintFuncDef\n");

    return SUCCESS;
}

#define TABS(index)                                             \
            for (unsigned int i = 0; i < index; i++)            \
                fprintf(code_f, "\t")

int PrintStStream(FILE* code_f, TreeNode *node, unsigned int *recur_lvl)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    printf("enter PrintStStream\n");

    TreeNode *extra_node = node;
    while (true)
    {
        if (extra_node == NULL)
            break;
        ERROR_CHECK(NTYPE(extra_node) != T_ST, ERROR_SYNTAX);

        TABS(*recur_lvl + 1);
        int err = PrintSt(code_f, extra_node->left, recur_lvl);
        ERROR_CHECK(err, ERROR_PRINT_ST);

        extra_node = extra_node->right;
    }

    printf("exit PrintStStream\n");

    return SUCCESS;

}

int PrintSt(FILE* code_f, TreeNode *node, unsigned int *recur_lvl)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(node   == NULL, ERROR_NULL_PTR);

    if (node == NULL)
        return SUCCESS;

    printf("enter PrintSt\n");

    int err = 0;
    switch (NTYPE(node))
    {
        case T_IF    :  (*recur_lvl)++;
                        err = PrintIf(code_f, node, recur_lvl);
                        ERROR_CHECK(err, ERROR_PRINT_IF);
                        break;

        case T_WHILE :  err = PrintWhile(code_f, node, recur_lvl);
                        ERROR_CHECK(err, ERROR_PRINT_WHILE);
                        break;

        case T_VAR   :  err = PrintVar(code_f, node);
                        ERROR_CHECK(err, ERROR_PRINT_VAR);
                        break;

        case T_RET   :  fprintf(code_f, "%s ", S_RET);

                        ERROR_CHECK(node->right != NULL, ERROR_SYNTAX);
                        
                        err = PrintExp(code_f, node->left);
                        ERROR_CHECK(err, ERROR_PRINT_EXP);
                        fprintf(code_f, " %s\n", S_ST_SEP);
                        break;

        case T_NUM   :  //falling
        case T_STR   :  //falling
        case T_OP    :  //falling
        case T_EQ    :  //falling
        case T_CALL  :  err = PrintExp(code_f, node);
                        ERROR_CHECK(err, ERROR_PRINT_EXP);
                        fprintf(code_f, " %s\n", S_ST_SEP);
                        break;

        case T_IN    :  //falling
        case T_OUT   :  err = PrintCallFunc(code_f, node);
                        ERROR_CHECK(err, ERROR_PRINT_CALL);
                        fprintf(code_f, " %s\n", S_ST_SEP);
                        break;
        
        default      :  fprintf(code_f, " ERROR_PRINT_ST(%d) ", (NTYPE(node)));
                        break;
    }



    printf("exit PrintSt\n");

    return SUCCESS;
}

int PrintIf(FILE* code_f, TreeNode *node, unsigned int *recur_lvl)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(node   == NULL, ERROR_NULL_PTR);

    printf("enter PrintIf\n");

    fprintf(code_f, "%s ", S_IF);

    fprintf(code_f, "%s ", S_EXP_L_BRCKT);
    int err = PrintExp(code_f, node->left);
    ERROR_CHECK(err, ERROR_PRINT_EXP);
    fprintf(code_f, " %s\n", S_EXP_R_BRCKT);

    ERROR_CHECK(node->right == NULL, ERROR_NULL_PTR);

    TreeNode *true_node = NULL;
    TreeNode *false_node = NULL;
    bool with_else = false;
    if (NTYPE(node->right) == T_ELSE)
    {
        with_else  = true;
        true_node  = node->right->left;
        false_node = node->right->right;
    }
    else
        true_node  = node->right;

    TABS(*recur_lvl);
    fprintf(code_f, "%s\n", S_ST_L_BRCKT);
    
    ERROR_CHECK(true_node == NULL, ERROR_NULL_PTR);
    err = PrintStStream(code_f, true_node, recur_lvl);
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
        err = PrintStStream(code_f, false_node, recur_lvl);
        ERROR_CHECK(err, ERROR_PRINT_ST_STREAM);

        TABS(*recur_lvl);
        fprintf(code_f, "%s\n\n", S_ST_R_BRCKT);
    }

    (*recur_lvl)--;
    printf("exit PrintIf\n");

    return SUCCESS;
}

int PrintWhile(FILE* code_f, TreeNode *node, unsigned int *recur_lvl)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(node   == NULL, ERROR_NULL_PTR);

    printf("enter PrintWhile\n");

    fprintf(code_f, " %s ", S_WHILE);

    fprintf(code_f, " %s ", S_EXP_L_BRCKT);
    int err = PrintExp(code_f, node->left);
    ERROR_CHECK(err, ERROR_PRINT_EXP);
    fprintf(code_f, " %s\n", S_EXP_R_BRCKT);

    ERROR_CHECK(node->right == NULL, ERROR_NULL_PTR);

    TreeNode *true_node = node->right;

    fprintf(code_f, "%s\n", S_ST_L_BRCKT);
    
    ERROR_CHECK(true_node == NULL, ERROR_NULL_PTR);
    err = PrintStStream(code_f, true_node, recur_lvl);
    ERROR_CHECK(err, ERROR_PRINT_ST_STREAM);

    fprintf(code_f, "%s\n\n", S_ST_R_BRCKT);

    printf("exit PrintWhile\n");

    return SUCCESS;
}
