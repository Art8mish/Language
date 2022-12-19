
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

    int print_lnode_err = PrintLangNode(code_f, tree->root);
    ERROR_CHECK(print_lnode_err, ERROR_PRINT_LANG_NODE);

    int fclose_err = fclose(code_f);
    ERROR_CHECK(fclose_err, ERROR_CLOSING_FILE);

    return SUCCESS;
}

int PrintLangNode(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    if (node == NULL)
        return SUCCESS;
    

    int err = PrintLangNode(code_f, node->left);
    ERROR_CHECK(err, ERROR_PRINT_LANG_NODE);

        err = PrintLangNode(code_f, node->right);
    ERROR_CHECK(err, ERROR_PRINT_LANG_NODE);
    

    return SUCCESS;
}

int PrintExtSt(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    if (node == NULL)
        return SUCCESS;
    ERROR_CHECK(node->left == NULL, ERROR_SYNTAX);

    int err = 0;
    switch (NTYPE(node->left))
    {
        case T_VAR  : err = PrintVar(code_f, node->left);
                      ERROR_CHECK(err, ERROR_PRINT_EXT_ST);
                      break;

        case T_FUNC : err = PrintFunc(code_f, node->left);
                      ERROR_CHECK(err, ERROR_PRINT_EXT_ST);
                      break;

        default     : return ERROR_SYNTAX;
        
    }

    int pr_st_err = PrintExtSt(code_f, node->right);
    ERROR_CHECK(pr_st_err, ERROR_PRINT_EXT_ST);

    return SUCCESS;
}

int PrintVar(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    fprintf(code_f, "%s %s ", S_VAR, S_EQ);

    int pr_exp_err = PrintExp(code_f, node->right);
    ERROR_CHECK(pr_exp_err, ERROR_PRINT_EXP);

    return SUCCESS;
}

int PrintExp(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    if (node == NULL)
        return SUCCESS;

    int err= 0;
    err = PrintExp(code_f, node->left);
    ERROR_CHECK(err, ERROR_PRINT_EXP);
    
    err = PrintVal(code_f, node);
    ERROR_CHECK(err, ERROR_PRINT_EXP);

    err = PrintExp(code_f, node->right);
    ERROR_CHECK(err, ERROR_PRINT_EXP);


    return SUCCESS;
}

int PrintVal(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    int err = 0;
    switch (NTYPE(node))
    {
        case T_NUM  : fprintf(code_f, " %g ", NNUM(node));
                      break;

        case T_STR  : fprintf(code_f, " %s ", NSTR(node));
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
                          case OP_POW : fprintf(code_f, " %s ", S_POW);
                                        break;
                          case OP_SIN : fprintf(code_f, " %s ", S_SIN);
                                        break;
                          case OP_COS : fprintf(code_f, " %s ", S_COS);
                                        break;
                          case OP_TG  : fprintf(code_f, " %s ", S_TG);
                                        break;
                          default     : fprintf(code_f, " OP_ERROR ");
                                        break;
                      }
                      break;

        case T_CALL : err = PrintCall(code_f, node);
                      ERROR_CHECK(err, ERROR_PRINT_CALL);
                      break;

        case T_NUM : fprintf(code_f, " %g ", NNUM(node));
                     break;
    }

    return SUCCESS;
}

int PrintCall(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    ERROR_CHECK(node->left  == NULL, ERROR_SYNTAX);
    ERROR_CHECK(node->right == NULL, ERROR_SYNTAX);

    //...................z    

    return SUCCESS;
}

int PrintFunc(FILE* code_f, TreeNode *node)
{
    ERROR_CHECK(code_f == NULL, ERROR_NULL_PTR);

    

    return SUCCESS;
}