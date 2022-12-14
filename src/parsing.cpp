
#include "../include/parsing.h"

#define READ_WHILE(str, cond)                                                   \
        do                                                                      \
        {                                                                       \
            int i = 0;                                                          \
            while (cond)                                                        \
            {                                                                   \
                str[i++] = *((*buf)++);                                         \
                LEX_ERROR(i == MAX_STR_SIZE - 1, ERROR_LONG_STR, *index);       \
                EOF_CHECK(*buf, ERROR_SYNTAX);                                  \
            }                                                                   \
            str[i] = '\0';                                                      \
            printf("readed str: %s\n", str);                                    \
        } while (false)

#define LEX_STR(string)                                                         \
        do                                                                      \
        {                                                                       \
            lex_structs[*index].type = LT_STR;                                  \
            const char *str_cpy = RusTranslate(string);                         \
            LEX_ERROR(str_cpy == NULL, ERROR_RUS_TRANSLATE, *index);            \
            lex_structs[*index].str = strdup(str_cpy);                          \
            LEX_ERROR(lex_structs[*index].str == NULL, ERROR_STRDUP, *index);   \
            free((void *)str_cpy);                                              \
            (*index)++;                                                         \
        } while (false)

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

LexStruct *LexicalAnalisis(char *buf, unsigned int *lex_structs_amount)
{
    ERROR_CHECK(buf == NULL, NULL);

    LexStruct *lex_structs = (LexStruct *) calloc(MAX_CODE_LENGTH, sizeof(LexStruct));
    ERROR_CHECK(lex_structs == NULL, NULL);

    int init_lex_err = InitializeLexStructs(lex_structs);
    ERROR_CHECK(init_lex_err, NULL);

    unsigned int index = 0;

    LEX_SKIP_SPACE(buf);

    if ((*buf) == EOF)
    {
        free(lex_structs);
        return NULL;
    }

    while ((*buf) != EOF)
    {
        char value[MAX_CMD_SIZE] = { 0 };
        int i = 0; 

        while (true)                                                   
        {
            printf("buf: %s\n", buf);                                                       
            value[i++] = *buf;  
            printf("value[%d]: %c\n", i-1, value[i-1]);                                     
            LEX_ERROR(i == MAX_CMD_SIZE - 1, NULL, index);       
            EOF_CHECK(buf, NULL); 

            buf++;   
            if ((isspace(*buf) || (!isalpha(*buf) && !isdigit(*buf))) || 
                                 (!isalpha(value[i-1]) && !isdigit(value[i-1])))
                break;
                                            
        }  
        value[i] = '\0';                                                      
        printf("readed value: %s\n", value); 

        // int read_val_err = ReadValue(value, &buf);
        // ERROR_CHECK(read_val_err, NULL);

        int proc_arg_err = ProcessLexValue(lex_structs, value, &buf, &index);
        ERROR_CHECK(proc_arg_err, NULL);

        LEX_SKIP_SPACE(buf);
    }

    lex_structs[index].type = LT_END;
    index++;
    *lex_structs_amount = index;

    return lex_structs; 
}

        


int ProcessLexValue(LexStruct *lex_structs, char *value, char **buf, unsigned int *index)
{
    LEX_ERROR(lex_structs == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(value       == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(*buf        == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(buf         == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(index       == NULL, ERROR_NULL_PTR, *index);

    if (strcmp(value, "var") == 0)
    {   
        printf("entered var\n");
        lex_structs[*index].type = LT_VAR;
        (*index)++;

        int proc_var_err = ProcessVarInit(lex_structs, buf, index);
        LEX_ERROR(proc_var_err, ERROR_PROCESS_VAR_INIT, *index);
    }

    else if (isdigit(value[0]))
    {
        printf("entered digit\n");
        double num = ConvertStrToNum(value);
        LEX_ERROR(isnan(num), ERROR_CONVERT_STR_TO_NUM, *index);
        lex_structs[*index].type = LT_NUM;
        lex_structs[*index].num  = num;
        (*index)++;
    }

    else if (strcmp(value, "{") == 0)
    {
        printf("entered {\n");
        lex_structs[*index].type = LT_STREAM_L_BRCKT;
        (*index)++;
    }

    else if (strcmp(value, "}") == 0)
    {
        printf("entered }\n");
        lex_structs[*index].type = LT_STREAM_R_BRCKT;
        (*index)++;
    }

    else if (strcmp(value, "(") == 0)
    {
        printf("entered (\n");
        lex_structs[*index].type = LT_EXP_L_BRCKT;
        (*index)++;
    }

    else if (strcmp(value, ")") == 0)
    {
        printf("entered )\n");
        lex_structs[*index].type = LT_EXP_R_BRCKT;
        (*index)++;
    }

    else if (strcmp(value, ";") == 0)
    {
        printf("entered ;\n");
        lex_structs[*index].type = LT_ST_SEP;
        (*index)++;
    }

    else if (strcmp(value, ",") == 0)
    {
        printf("entered ;\n");
        lex_structs[*index].type = LT_COMMA;
        (*index)++;
    }

    else if (strcmp(value, "=") == 0)
    {
        printf("entered =\n");
        lex_structs[*index].type = LT_EQ;
        (*index)++;
        (*buf)++;

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        char str[MAX_STR_SIZE] = { 0 };
        READ_WHILE(str, (**buf != ';' && !isspace(**buf)));
        LEX_ERROR(str[0] == '\0', ERROR_SYNTAX, *index);

        int proc_var_val = ProcessArg(lex_structs, index, str);
        LEX_ERROR(proc_var_val, ERROR_PROCESS_ARG, *index);
    }

    else if (strcmp(value, "if") == 0)
    {
        printf("entered if\n");
        lex_structs[*index].type = LT_IF;
        (*index)++;

        int proc_exp_err = ProcessExpression(lex_structs, buf, index);
        LEX_ERROR(proc_exp_err, ERROR_PROCESS_EXPRESSION, *index);
    }

    else if (strcmp(value, "else") == 0)
    {
        printf("entered else\n");
        lex_structs[*index].type = LT_ELSE;
        (*index)++;
    }

    else if (strcmp(value, "func") == 0)
    {
        printf("entered func\n");
        lex_structs[*index].type = LT_FUNC;
        (*index)++;

        int proc_exp_err = ProcessFunction(lex_structs, buf, index);
        LEX_ERROR(proc_exp_err, ERROR_PROCESS_FUNCTION, *index);
    }

    else if (strcmp(value, "return") == 0)
    {
        printf("entered return\n");
        lex_structs[*index].type = LT_RET;
        (*index)++;

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        char str[MAX_STR_SIZE] = { 0 };
        READ_WHILE(str, (**buf != ';' && !isspace(**buf)));
        LEX_ERROR(str[0] == '\0', ERROR_SYNTAX, *index);

        int proc_var_val = ProcessArg(lex_structs, index, str);
        LEX_ERROR(proc_var_val, ERROR_PROCESS_ARG, *index);
    }

    else if (strcmp(value, "+") == 0)
    {
        printf("entered +\n");
        lex_structs[*index].type = LT_OP;
        lex_structs[*index].arithm_op = OP_ADD;
        (*index)++;
    }

    else if (strcmp(value, "-") == 0)
    {
        printf("entered -\n");
        lex_structs[*index].type = LT_OP;
        lex_structs[*index].arithm_op = OP_SUB;
        (*index)++;
    }

    else if (strcmp(value, "*") == 0)
    {
        printf("entered *\n");
        lex_structs[*index].type = LT_OP;
        lex_structs[*index].arithm_op = OP_MUL;
        (*index)++;
    }

    else if (strcmp(value, "/") == 0)
    {
        printf("entered /\n");
        lex_structs[*index].type = LT_OP;
        lex_structs[*index].arithm_op = OP_DIV;
        (*index)++;
    }

    else
    {
        printf("entered str\n");
        LEX_STR(value);
    }

    return SUCCESS;
}

int ProcessExpression(LexStruct *lex_structs, char **buf, unsigned int *index)
{
    printf("entered ProcessExpression\n");

    LEX_ERROR(lex_structs == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(*buf        == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(buf         == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(index       == NULL, ERROR_NULL_PTR, *index);
    
    SKIP_SPACE(*buf, ERROR_SYNTAX);
    LEX_ERROR(**buf != '(', ERROR_SYNTAX, *index);
    lex_structs[*index].type = LT_EXP_L_BRCKT;
    (*index)++;
    (*buf)++;
    
    SKIP_SPACE(*buf, ERROR_SYNTAX);
    while (true)
    {
        char str[MAX_STR_SIZE] = { 0 };

        READ_WHILE(str, **buf != '+' && **buf != '-' && **buf != ')' &&
                        **buf != '*' && **buf != '/' && !isspace(**buf));
        
        int proc_var_val = ProcessArg(lex_structs, index, str);
        LEX_ERROR(proc_var_val, ERROR_PROCESS_ARG, *index);

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        if (**buf == ')')
        {
            lex_structs[*index].type = LT_EXP_R_BRCKT;
            (*index)++;
            (*buf)++;
            break;
        }

        printf("is op\n");
        switch (**buf)
        {
            case '+' :  lex_structs[*index].type      = LT_OP;
                        lex_structs[*index].arithm_op = OP_ADD;
                        (*index)++;
                        break;
                        

            case '-' :  lex_structs[*index].type      = LT_OP;
                        lex_structs[*index].arithm_op = OP_SUB;
                        (*index)++;
                        break;

            case '*' :  lex_structs[*index].type      = LT_OP;
                        lex_structs[*index].arithm_op = OP_MUL;
                        (*index)++;
                        break;

            case '/' :  lex_structs[*index].type      = LT_OP;
                        lex_structs[*index].arithm_op = OP_DIV;
                        (*index)++;
                        break;

            default  :  LEX_ERROR(true, ERROR_SYNTAX, *index);
        }
        (*buf)++;

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        LEX_ERROR(**buf == ')', ERROR_SYNTAX, *index);
    }

    return SUCCESS;
}

int ProcessArg(LexStruct *lex_structs, unsigned int *index, char *str)
{
    LEX_ERROR(lex_structs == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(index       == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(str         == NULL, ERROR_NULL_PTR, *index);

    printf("entered process_arg\n");

    if (isdigit(str[0]))
    {
        printf("entered digit\n");
        double num = ConvertStrToNum(str);
        ERROR_CHECK(isnan(num), ERROR_CONVERT_STR_TO_NUM);
        lex_structs[*index].type = LT_NUM;
        lex_structs[*index].num  = num;
        (*index)++;
    }

    else if (str[0] == '\0')
    {
        printf("entered null\n");
        lex_structs[*index].type = LT_NIL;
        (*index)++;
    }
    
    else
    {
        printf("entered str\n");
        LEX_STR(str);
    }

    return SUCCESS;
}

int ProcessVarInit(LexStruct *lex_structs, char **buf, unsigned int *index)
{
    LEX_ERROR(lex_structs == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(*buf        == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(buf         == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(index       == NULL, ERROR_NULL_PTR, *index);

    SKIP_SPACE(*buf, ERROR_SYNTAX);

//read var name
    char str[MAX_STR_SIZE] = { 0 }; 

    READ_WHILE(str, **buf != '=' && !isspace(**buf));
    LEX_ERROR(str[0] == '\0', ERROR_SYNTAX, *index);
    LEX_ERROR(isdigit(str[0]), ERROR_SYNTAX, *index);

    LEX_STR(str);

    SKIP_SPACE(*buf, ERROR_SYNTAX);
    LEX_ERROR(**buf != '=', ERROR_SYNTAX, *index);
    (*buf)++;
    SKIP_SPACE(*buf, ERROR_SYNTAX);

//read init val
    READ_WHILE(str, **buf != ';' && !isspace(**buf));
    LEX_ERROR(str[0] == '\0', ERROR_SYNTAX, *index);

    int proc_var_val = ProcessArg(lex_structs, index, str);
    LEX_ERROR(proc_var_val, ERROR_PROCESS_ARG, *index);

    return SUCCESS;
}

int ProcessFunction(LexStruct *lex_structs, char **buf, unsigned int *index)
{
    LEX_ERROR(lex_structs == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(*buf        == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(buf         == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(index       == NULL, ERROR_NULL_PTR, *index);

    SKIP_SPACE(*buf, ERROR_SYNTAX);
    char str[MAX_STR_SIZE] = { 0 };
    READ_WHILE(str, **buf != '(' && !isspace(**buf));
    LEX_ERROR(str[0] == '\0', ERROR_SYNTAX, *index);
    LEX_STR(str);

    SKIP_SPACE(*buf, ERROR_SYNTAX);
    LEX_ERROR(**buf != '(', ERROR_SYNTAX, *index);
    lex_structs[*index].type = LT_EXP_L_BRCKT;
    (*index)++;
    (*buf)++;

    SKIP_SPACE(*buf, ERROR_SYNTAX);
    while (**buf != ')')
    {
        printf("(func cycle) buf: %s\n", *buf);
        if (**buf == ',')
        {
            lex_structs[*index].type = LT_COMMA;
            (*index)++;
            (*buf)++;
        }

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        READ_WHILE(str, !isspace(**buf));
        LEX_ERROR(strcmp(str, "var") != 0, ERROR_SYNTAX, *index);
        lex_structs[*index].type = LT_VAR;
        (*index)++;

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        READ_WHILE(str, !isspace(**buf) && **buf != ',' && **buf != ')');
        LEX_ERROR(str[0] == '\0', ERROR_SYNTAX, *index);
        
        LEX_STR(str);

        SKIP_SPACE(*buf, ERROR_SYNTAX);
    }

    lex_structs[*index].type = LT_EXP_R_BRCKT;
    (*index)++;
    (*buf)++;

    return SUCCESS;
}


/*
struct TreeNode *ReadExpression(const char *str)
{
    ERROR_CHECK(str == NULL, 0);
    
    struct TreeNode *node = GetAdd(&str);
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

    ERROR_CHECK(*string != '\0', NAN);
    num *= sign;

    return num;
}