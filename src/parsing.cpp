
#include "../include/parsing.h"

int InitializeLexStructs(LexStruct *lex_structs)
{
    ERROR_CHECK(lex_structs == NULL, ERROR_NULL_PTR);

    for (int i = 0; i < MAX_CODE_LENGTH; i++)
    {
        lex_structs[i].type      = LT_NIL;
        lex_structs[i].arithm_op = OP_PSN;
        lex_structs[i].num       = NUM_PSN;
        lex_structs[i].str       = NULL;
    }

    return SUCCESS;
}

LexStruct *LexicalAnalisis(char *buf)
{
    ERROR_CHECK(buf == NULL, NULL);

    LexStruct *lex_structs = (LexStruct *) calloc(MAX_CODE_LENGTH, sizeof(LexStruct));
    ERROR_CHECK(lex_structs == NULL, NULL);

    int init_lex_err = InitializeLexStructs(lex_structs);
    ERROR_CHECK(init_lex_err, NULL);

    unsigned int index = 0;

    LEX_SKIP_SPACE(buf);

    if ((*buf) != EOF)
    {
        free(lex_structs);
        return NULL;
    }

    while ((*buf) != EOF)
    {
        char value[MAX_CMD_SIZE] = { 0 };
        int read_val_err = ReadValue(value, &buf);
        ERROR_CHECK(read_val_err, NULL);

        lex_structs[index].struct_ip = index;
        int pr_arg_err = ProcessLexArg(&lex_structs[index], value, &buf);
        ERROR_CHECK(pr_arg_err, NULL);
        index++;

        LEX_SKIP_SPACE(buf);
    }

    return lex_structs; 
}


int ProcessLexArg(LexStruct *lex_struct, char *value, char **buf)
{
    ERROR_CHECK(lex_struct == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(value      == NULL, ERROR_NULL_PTR);

    return SUCCESS;
}



/*
struct TreeNode *ReadProgram(const char *str)
{
    ERROR_CHECK(str == NULL, 0);
    
    struct TreeNode *node = GetStatement(&str);
    ERROR_CHECK(*str != '\0', NULL);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetAdd(const char **buf)
{
    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = GetMul(buf);

    while (**buf == '+' || **buf == '-')
    {
        int op = **buf;
        (*buf)++;

        struct TreeNode *node2 = GetMul(buf);

        struct TreeNode *parent_node = NULL; 
        if (op == '+')
            parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(ADD_OP), node, node2);
        else
            parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(SUB_OP), node, node2);

        node = parent_node;
    }

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetMul(const char **buf)
{
    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = GetPow(buf);

    while (**buf == '*' || **buf == '/')
    {
        int op = **buf;
        (*buf)++;

        struct TreeNode *node2 = GetPow(buf);

        struct TreeNode *parent_node = NULL; 
        if (op == '*')
            parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(MUL_OP), node, node2);
        else
        {
            ERROR_CHECK(node2->value->type_arg == TYPE_NUM && 
                        node2->value->diff_arg->num - EPS <= 0  &&
                        node2->value->diff_arg->num + EPS >= 0, NULL);
            parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(DIV_OP), node, node2);
        }

        node = parent_node;
    }

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetPow(const char **buf)
{
    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = GetUnarFunc(buf);

    while (**buf == '^')
    {
        (*buf)++;
        struct TreeNode *node2 = GetUnarFunc(buf);
        struct TreeNode *parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(POW_OP), node, node2); 

        node = parent_node;
    }

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetUnarFunc(const char **buf)
{
    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = NULL;

    if (**buf == 's' || **buf == 'c' || **buf == 't' || **buf == 'a' || **buf == 'l')
    {
        int symb_count = 0;
        char op[MAX_ARG_LEN] = { 0 };
        sscanf((*buf), "%[a-z] %n", op, &symb_count);                     
        (*buf) += symb_count;

        struct TreeNode *node2 = GetBrackets(buf);

        if(strcmp(op, "sin") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(SIN_OP), NULL, node2);

        else if(strcmp(op, "cos") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(COS_OP), NULL, node2);

        else if(strcmp(op, "tg") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(TAN_OP), NULL, node2);

        else if(strcmp(op, "arcsin") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(ASIN_OP), NULL, node2);

        else if(strcmp(op, "arccos") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(ACOS_OP), NULL, node2);

        else if(strcmp(op, "arctg") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(ATAN_OP), NULL, node2);

        else if(strcmp(op, "sh") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(SINH_OP), NULL, node2);

        else if(strcmp(op, "ch") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(COSH_OP), NULL, node2);

        else if(strcmp(op, "th") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(TANH_OP), NULL, node2);

        else if(strcmp(op, "ln") == 0)
        {
            ERROR_CHECK(node2->value->type_arg == TYPE_NUM && 
                        node2->value->diff_arg->num < EPS, NULL);
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(LN_OP), NULL, node2);
        }
    }

    else
        node = GetBrackets(buf);

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetBrackets(const char **buf)
{
    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = NULL;

    if (**buf == '(')
    {
        (*buf)++;

        node = GetAdd(buf);

        ERROR_CHECK((**buf) != ')', 0);
        (*buf)++;
    }

    else
        node = GetArg(buf);

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetArg(const char **buf)
{
    SKIP_SPACE_STR(*buf);

    tree_elem_t val = NULL;
    CREATE_TREE_NODE_VALUE(val, NULL);

    struct TreeNode *new_node = TreeNodeCtor(val, NULL, NULL);
    const char *s_old = *buf;
    if ((**buf >= '0' && **buf <= '9') || **buf == 'p' || **buf == 'e')
    {
        new_node->value->type_arg = TYPE_NUM;
        diff_num_t num = DIFF_NUM_PSN;

        if (**buf == 'p')
        {
            (*buf)++;
            ERROR_CHECK(**buf != 'i', NULL);
            num = PI;
            (*buf)++;
        }

        else if (**buf == 'e')
        {
            num = EXP;
            (*buf)++;
        }

        else
        {
            size_t num_length = ConvertStrToNum(buf, &num);
            ERROR_CHECK(num_length == 0, TREE_VAL_PSN);
        }

        new_node->value->diff_arg->num = num;
    }
    
    else if (**buf >= 'a' && **buf <= 'z' && 
             **buf != 's' && **buf != 'c' && **buf != 't' &&
             **buf != 'a' && **buf != 'l' && **buf != 'p' &&
             **buf != 'e')
    {
        new_node->value->type_arg = TYPE_VAR;

        new_node->value->diff_arg->var = **buf;

        (*buf)++;
    }

    ERROR_CHECK(*buf == s_old, TREE_VAL_PSN);

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(new_node == NULL, NULL);
    return new_node;
}*/

double ConvertStrToNum(const char *string)
{
    ERROR_CHECK(string == NULL, NAN);
    
    double num = 0;

    int sign = 1;
    if(*string == '-' || *string == '+')
    {
        if (*string == '-')
            sign = -1;
        string++;
    }

    while (*string >= '0' && *string <= '9')
    {
        num = num * 10 + *string - '0';
        string++;
    }

    if (*string == '.' || *string == ',')
    {
        string++;

        double degree = 0.1;
        while (*string >= '0' && *string <= '9')
        {
            num += (*string - '0') * degree;
            string++;
            degree *= 0.1;
        }
    }
    
    num *= sign;

    return num;
}