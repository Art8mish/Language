
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
            lex_structs[*index].str = str_cpy;                                  \
            (*index)++;                                                         \
        } while (false)

#define STRCT_T     lex_structs[*index].type
#define STRCT_OP    lex_structs[*index].arithm_op
#define STRCT_STR   lex_structs[*index].str
#define STRCT_NUM   lex_structs[*index].num
#define STRCT_FNAME lex_structs[*index].func_name


int LexStructsInit(LexStruct *lex_structs)
{
    ERROR_CHECK(lex_structs == NULL, ERROR_NULL_PTR);

    for (int i = 0; i < MAX_CODE_LENGTH; i++)
    {
        lex_structs[i].type      = LT_NIL;
        lex_structs[i].arithm_op = OP_PSN;
        lex_structs[i].num       = NUM_PSN;
        lex_structs[i].str       = NULL;
        lex_structs[i].func_name = false;
    }

    return SUCCESS;
}
//___________________________________________________________________________________________________

LexStruct *LexicalAnalisis(char *buf, unsigned int *lex_structs_amount)
{
    ERROR_CHECK(buf == NULL, NULL);

    LexStruct *lex_structs = (LexStruct *) calloc(MAX_CODE_LENGTH, sizeof(LexStruct));
    ERROR_CHECK(lex_structs == NULL, NULL);

    int init_lex_err = LexStructsInit(lex_structs);
    SAFE_ERROR_CHECK(init_lex_err, free(lex_structs);, NULL);

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
            SAFE_LEX_ERROR(i == MAX_CMD_SIZE - 1, LexStructDtor(lex_structs, index);, NULL, index);       

            buf++;
            if ((*buf) == EOF)
            {
                LexStructDtor(lex_structs, index);
                return NULL;
            } 

            if ((isspace(*buf) || (!isalpha(*buf) && !isdigit(*buf))) || 
                                  (!isalpha(value[i-1]) && !isdigit(value[i-1])))
                break;
                                            
        }  
        value[i] = '\0';                                                      
        printf("readed value: %s\n", value); 

        int proc_arg_err = ProcessLexValue(lex_structs, value, &buf, &index);
        SAFE_ERROR_CHECK(proc_arg_err, LexStructDtor(lex_structs, index);, NULL);

        LEX_SKIP_SPACE(buf);
    }

    lex_structs[index].type = LT_END;
    index++;

    LexStruct *reall_lex_structs = (LexStruct *) realloc(lex_structs, sizeof(LexStruct) * (index));
    SAFE_ERROR_CHECK(reall_lex_structs == NULL, LexStructDtor(lex_structs, index);, NULL);

    *lex_structs_amount = index;

    return reall_lex_structs; 
}
//___________________________________________________________________________________________________

int LexStructDtor(LexStruct *lex_structs, unsigned int struct_amount)
{
    ERROR_CHECK(lex_structs == NULL, ERROR_NULL_PTR);

    for (unsigned int index = 0; index < struct_amount; index++)
    {
        if (lex_structs[index].str != NULL)
            free((void *)lex_structs[index].str);
    }

    free(lex_structs);

    return SUCCESS;
}
//_____________________________________________________________________________________________________

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
        STRCT_T = LT_VAR;
        (*index)++;

        int proc_var_err = ProcessVarInit(lex_structs, buf, index);
        LEX_ERROR(proc_var_err, ERROR_PROCESS_VAR_INIT, *index);
    }

    else if (isdigit(value[0]))
    {
        printf("entered digit\n");
        double num = ConvertStrToNum(value);
        LEX_ERROR(isnan(num), ERROR_CONVERT_STR_TO_NUM, *index);
        STRCT_T = LT_NUM;
        STRCT_NUM  = num;
        (*index)++;
    }

    else if (strcmp(value, "{") == 0)
    {
        printf("entered {\n");
        STRCT_T = LT_STREAM_L_BRCKT;
        (*index)++;
    }

    else if (strcmp(value, "}") == 0)
    {
        printf("entered }\n");
        STRCT_T = LT_STREAM_R_BRCKT;
        (*index)++;
    }

    else if (strcmp(value, "(") == 0)
    {
        printf("entered (\n");
        STRCT_T = LT_EXP_L_BRCKT;
        (*index)++;
    }

    else if (strcmp(value, ")") == 0)
    {
        printf("entered )\n");
        STRCT_T = LT_EXP_R_BRCKT;
        (*index)++;
    }

    else if (strcmp(value, ";") == 0)
    {
        printf("entered ;\n");
        STRCT_T = LT_ST_SEP;
        (*index)++;
    }

    else if (strcmp(value, ",") == 0)
    {
        printf("entered ;\n");
        STRCT_T = LT_COMMA;
        (*index)++;
    }

    else if (strcmp(value, "=") == 0)
    {
        printf("entered =\n");
        STRCT_T = LT_EQ;
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
        STRCT_T = LT_IF;
        (*index)++;

        int proc_exp_err = ProcessExpression(lex_structs, buf, index);
        LEX_ERROR(proc_exp_err, ERROR_PROCESS_EXPRESSION, *index);
    }

    else if (strcmp(value, "else") == 0)
    {
        printf("entered else\n");
        STRCT_T = LT_ELSE;
        (*index)++;
    }

    else if (strcmp(value, "func") == 0)
    {
        printf("entered func\n");
        STRCT_T = LT_FUNC;
        (*index)++;

        int proc_exp_err = ProcessFunction(lex_structs, buf, index);
        LEX_ERROR(proc_exp_err, ERROR_PROCESS_FUNCTION, *index);
    }

    else if (strcmp(value, "return") == 0)
    {
        printf("entered return\n");
        STRCT_T = LT_RET;
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
        STRCT_T  = LT_OP;
        STRCT_OP = OP_ADD;
        (*index)++;
    }

    else if (strcmp(value, "-") == 0)
    {
        printf("entered -\n");
        STRCT_T  = LT_OP;
        STRCT_OP = OP_SUB;
        (*index)++;
    }

    else if (strcmp(value, "*") == 0)
    {
        printf("entered *\n");
        STRCT_T  = LT_OP;
        STRCT_OP = OP_MUL;
        (*index)++;
    }

    else if (strcmp(value, "/") == 0)
    {
        printf("entered /\n");
        STRCT_T  = LT_OP;
        STRCT_OP = OP_DIV;
        (*index)++;
    }

    else
    {
        printf("entered str\n");
        LEX_STR(value);
    }

    return SUCCESS;
}
//___________________________________________________________________________________________________

int ProcessExpression(LexStruct *lex_structs, char **buf, unsigned int *index)
{
    printf("entered ProcessExpression\n");

    LEX_ERROR(lex_structs == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(*buf        == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(buf         == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(index       == NULL, ERROR_NULL_PTR, *index);
    
    SKIP_SPACE(*buf, ERROR_SYNTAX);
    LEX_ERROR(**buf != '(', ERROR_SYNTAX, *index);
    STRCT_T = LT_EXP_L_BRCKT;
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
            STRCT_T = LT_EXP_R_BRCKT;
            (*index)++;
            (*buf)++;
            break;
        }

        printf("is op\n");
        switch (**buf)
        {
            case '+' :  STRCT_T  = LT_OP;
                        STRCT_OP = OP_ADD;
                        (*index)++;
                        break;
                        

            case '-' :  STRCT_T  = LT_OP;
                        STRCT_OP = OP_SUB;
                        (*index)++;
                        break;

            case '*' :  STRCT_T  = LT_OP;
                        STRCT_OP = OP_MUL;
                        (*index)++;
                        break;

            case '/' :  STRCT_T  = LT_OP;
                        STRCT_OP = OP_DIV;
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
//___________________________________________________________________________________________________

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
    STRCT_FNAME = true;
    (*index)++;              

    SKIP_SPACE(*buf, ERROR_SYNTAX);
    LEX_ERROR(**buf != '(', ERROR_SYNTAX, *index);
    STRCT_T = LT_EXP_L_BRCKT;
    (*index)++;
    (*buf)++;

    SKIP_SPACE(*buf, ERROR_SYNTAX);
    while (**buf != ')')
    {
        printf("(func cycle) buf: %s\n", *buf);
        if (**buf == ',')
        {
            STRCT_T = LT_COMMA;
            (*index)++;
            (*buf)++;
        }

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        READ_WHILE(str, !isspace(**buf));
        LEX_ERROR(strcmp(str, "var") != 0, ERROR_SYNTAX, *index);
        STRCT_T = LT_VAR;
        (*index)++;

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        READ_WHILE(str, !isspace(**buf) && **buf != ',' && **buf != ')');
        LEX_ERROR(str[0] == '\0', ERROR_SYNTAX, *index);
        
        LEX_STR(str);

        SKIP_SPACE(*buf, ERROR_SYNTAX);
    }

    STRCT_T = LT_EXP_R_BRCKT;
    (*index)++;
    (*buf)++;

    return SUCCESS;
}
//___________________________________________________________________________________________________

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
        STRCT_T = LT_NUM;
        STRCT_NUM  = num;
        (*index)++;
    }

    else if (str[0] == '\0')
    {
        printf("entered null\n");
        STRCT_T = LT_NIL;
        (*index)++;
    }
    
    else
    {
        printf("entered str\n");
        LEX_STR(str);
    }

    return SUCCESS;
}
//___________________________________________________________________________________________________

const char **FindFuncNames(const LexStruct *lex_structs, unsigned int *func_amount)
{
    ERROR_CHECK(lex_structs == NULL, NULL);

    static bool names_are_found = false;
    static unsigned int func_counter = 0;
    static const char **reall_func_names = NULL;

    if (names_are_found)
    {
        *func_amount = func_counter;
        return reall_func_names;
    }

    const char **func_names = (const char **) calloc(MAX_FUNCS_AMOUNT, sizeof(const char *));
    ERROR_CHECK(func_names == NULL, NULL);

    unsigned int index = 0;
    while (lex_structs[index].type != LT_END)
    {
        if (lex_structs[index].func_name == true)
        {
            func_names[func_counter] = lex_structs[index].str;
            func_counter++;
        }

        index++;
    }

    reall_func_names = (const char **) realloc(func_names, sizeof(const char *) * func_counter);
    SAFE_ERROR_CHECK(reall_func_names == NULL, free(func_names);, NULL);

    *func_amount = func_counter;
    names_are_found = true;

    return reall_func_names;
}
//___________________________________________________________________________________________________


#define T_NCTOR(n_name, n_type)                                                         \
            do {                                                                        \
                LangNode *_lang_val = NULL;                                             \
                CREATE_TREE_NODE_VALUE(_lang_val, NULL);                                \
                lang_val->type = n_type;                                                \
                Treenode *_new_node = TreeNodeCtor(_lang_val, NULL, NULL);              \
                LEX_ERROR(_new_node == NULL, NULL, *index);                             \
                n_name = _new_node;                                                     \
            } while (false)

#define STR_NCTOR(n_name, n_str)                                                        \
            do {                                                                        \
                LangNode *_lang_val = NULL;                                             \
                CREATE_TREE_NODE_VALUE(_lang_val, NULL);                                \
                lang_val->type = T_STR;                                                 \
                lang_val->str  = n_str;                                                 \
                Treenode *_new_node = TreeNodeCtor(_lang_val, NULL, NULL);              \
                LEX_ERROR(_new_node == NULL, NULL, *index);                             \
                n_name = _new_node;                                                     \
            } while (false)

#define TIE(parent_node, child_node, tie_type)                                      \
            do {                                                                    \
                int tie_err = TreeNodeTie(parent_node, child_node, tie_type);       \
                LEX_ERROR(tie_err, NULL, *index);                                   \
            } while (false)

TreeNode *ReadLexCode(const LexStruct *lex_structs)
{
    unsigned int index = 0; 
    LEX_ERROR(lex_structs == NULL, NULL, index);

    struct TreeNode *node = GetExternal(lex_structs, &index);
    LEX_ERROR(lex_structs[index].type != LT_END, NULL, index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetExternal(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    TreeNode *st_node    = NULL;
    TreeNode *extra_node = NULL;

    bool first_cycle = true;
    while (STRCT_T == LT_FUNC || STRCT_T == LT_VAR)
    {
        T_NCTOR(curr_st_node, T_ST);

        if (STRCT_T == LT_FUNC)
        {
            index++;
            TreeNode *func_node = GetFuncDef(lex_structs, index);
            LEX_ERROR(func_node == NULL, NULL, *index);
            
            TIE(curr_st_node, func_node, TREE_TIE_LEFT);
        }

        else if (STRCT_T == LT_VAR)
        {
            index++;
            TreeNode *var_node = GetVar(lex_structs, index);
            LEX_ERROR(var_node == NULL, NULL, *index);
            
            TIE(curr_st_node, var_node, TREE_TIE_LEFT);
        }

        if (first_cycle)
        {
            st_node = curr_st_node;
            extra_node = curr_st_node;
            first_cycle = false;
            continue;
        }

        TIE(extra_node, curr_st_node, TREE_TIE_RIGHT);
        extra_node = curr_st_node;

    }

    return st_node;
}
//___________________________________________________________________________________________________

TreeNode *GetFuncDef(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    T_NCTOR(func_node, T_FUNC);

    LEX_ERROR(STRCT_T != LT_STR, NULL, *index);
    STR_NCTOR(func_name, STRCT_STR);
    (*index)++;

    LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NULL, *index);
    (*index)++;

    TreeNode *extra_node = NULL;
    bool first_cycle = true;
    while (STRCT_T != LT_EXP_R_BRCKT)
    {
        if (!first_cycle)
        {
            LEX_ERROR(STRCT_T != LT_COMMA, NULL, *index);
            (*index)++;
        }

        LEX_ERROR(STRCT_T != LT_VAR, NULL, *index);
        T_NCTOR(param_node, T_PARAM);
        (*index++);

        T_NCTOR(var_node, T_VAR);
        TIE(param_node, var_node, TREE_TIE_LEFT);

        LEX_ERROR(STRCT_T != LT_STR, NULL, *index);
        STR_NCTOR(var_name, STRCT_STR);
        (*index)++;
        TIE(var_node, var_name, TREE_TIE_LEFT);

        if (first_cycle)
        {
            TIE(func_name, param_node, TREE_TIE_LEFT);
            extra_node = param_node;
            first_cycle = false;
            continue;
        }

        TIE(extra_node, param_node, TREE_TIE_RIGHT);
        extra_node = param_node;
    }

    LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NULL, *index);
    (*index)++;
    
    LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NULL, *index);
    TreeNode *st_node = GetStatementStream(lex_structs, index);
    LEX_ERROR(st_node == NULL, NULL, *index);

    TIE(func_name, st_node, TREE_TIE_RIGHT);

    return func_node;
}
//___________________________________________________________________________________________________

TreeNode *GetVar(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    T_NCTOR(var_node, T_VAR);

    LEX_ERROR(STRCT_T != LT_STR, NULL, *index);
    STR_NCTOR(var_name, STRCT_STR);
    (*index)++;

    TIE(var_node, var_name, TREE_TIE_LEFT);

    TreeNode *val_node = GetExp(lex_structs, index);
    LEX_ERROR(val_node == NULL, NULL, *index);

    // LEX_ERROR(STRCT_T != LT_NUM && STRCT_T != LT_STR, NULL, *index);
    // if (STRCT_T == LT_NUM)
    // {
    //     val_node->value->type = T_NUM;
    //     val_node->value->num  = STRCT_NUM;  
    // }

    // else
    // {
    //     val_node->value->type = T_STR;
    //     val_node->value->str  = STRCT_STR;
    // }
    // (*index)++;

    TIE(var_node, val_node, TREE_TIE_RIGHT);

    LEX_ERROR(STRCT_T != LT_ST_SEP, NULL, *index);
    (*index)++;

    return var_node;
}
//___________________________________________________________________________________________________

TreeNode *GetStatement(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    T_NCTOR(st_node, T_ST);

    LEX_ERROR(STRCT_T != LT_VAR   &&
              STRCT_T != LT_IF    &&
              STRCT_T != LT_WHILE &&
              STRCT_T != LT_STR, NULL, *index);

    if (STRCT_T == LT_VAR)
    {
        (*index)++;
        TreeNode *var_node = GetVar(lex_structs, index);
        LEX_ERROR(var_node == NULL, NULL, *index);
        
        TIE(st_node, var_node, TREE_TIE_LEFT);
    }

    else if (STRCT_T == LT_IF)
    {
        (*index)++;
        TreeNode *if_node = GetIf(lex_structs, index);
        LEX_ERROR(if_node == NULL, NULL, *index);
        
        TIE(st_node, if_node, TREE_TIE_LEFT);
    }

    else if (STRCT_T == LT_WHILE)
    {
        (*index)++;
        TreeNode *while_node = GetWhile(lex_structs, index);
        LEX_ERROR(while_node == NULL, NULL, *index);
        
        TIE(st_node, while_node, TREE_TIE_LEFT);
    }

    else
    {
        (*index)++;
        TreeNode *str_node = GetStr(lex_structs, index);
        LEX_ERROR(str_node == NULL, NULL, *index);
        
        TIE(st_node, str_node, TREE_TIE_LEFT);    

        LEX_ERROR(STRCT_T != LT_ST_SEP, NULL, *index);
        (*index)++;
    }

    return st_node;
}
//___________________________________________________________________________________________________

TreeNode *GetStatementStream(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NULL, *index);
    (*index)++;

    TreeNode *ret_st_node = NULL;
    TreeNode *extra_node  = NULL;
    bool first_cycle = true;
    while (STRCT_T != LT_STREAM_R_BRCKT)
    {
        TreeNode *st_node = GetStatement(lex_structs, index);
        LEX_ERROR(st_node == NULL, NULL, *index);

        if (first_cycle)
        {
            ret_st_node = st_node;
            extra_node  = st_node;
            first_cycle = false;
            continue;
        }

        TIE(extra_node, st_node, TREE_TIE_RIGHT);
        extra_node = st_node;
    }
    (*index)++;

    return ret_st_node;
}
//___________________________________________________________________________________________________

TreeNode *GetIf(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    T_NCTOR(if_node, T_IF);

    LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NULL, *index);
    (*index)++;

    TreeNode *cond_node = GetExp(lex_structs, index);
    LEX_ERROR(cond_node == NULL, NULL, *index);

    LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NULL, *index);
    (*index)++;

    TIE(if_node, cond_node, TREE_TIE_LEFT);

    LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NULL, *index);
    TreeNode *if_st_node = GetStatementStream(lex_structs, index);
    LEX_ERROR(if_st_node == NULL, NULL, *index);

    if (STRCT_T != LT_ELSE)
    {
        TIE(if_node, if_st_node, TREE_TIE_RIGHT);
        return if_node;
    }

    T_NCTOR(else_node, T_ELSE);
    (*index)++;
    TIE(else_node, if_st_node, TREE_TIE_LEFT);

    LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NULL, *index);
    TreeNode *else_st_node = GetStatementStream(lex_structs, index);
    LEX_ERROR(else_st_node == NULL, NULL, *index);

    TIE(else_node, else_st_node, TREE_TIE_RIGHT);
    TIE(if_node, else_node, TREE_TIE_RIGHT);

    return if_node;
}
//___________________________________________________________________________________________________

TreeNode *GetWhile(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    T_NCTOR(while_node, T_IF);

    LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NULL, *index);
    (*index)++;

    TreeNode *cond_node = GetExp(lex_structs, index);
    LEX_ERROR(cond_node == NULL, NULL, *index);

    LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NULL, *index);
    (*index)++;

    TIE(while_node, cond_node, TREE_TIE_LEFT);

    LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NULL, *index);
    TreeNode *while_st_node = GetStatementStream(lex_structs, index);
    LEX_ERROR(while_st_node == NULL, NULL, *index);

    TIE(while_node, while_st_node, TREE_TIE_RIGHT);

    return while_node;
}
//___________________________________________________________________________________________________

#define FIND_FUNC_NAME(is_func, name)                                                   \
            do                                                                          \
            {                                                                           \
                for (unsigned int i = 0; i < func_amount; i++)                          \
                    if (strcmp(name, func_names[i]) == 0)                               \
                        is_func = true;                                                 \
            } while (false)

TreeNode *GetStr(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    unsigned int func_amount = 0;
    const char **func_names = FindFuncNames(lex_structs, &func_amount);
    LEX_ERROR(func_names == NULL, NULL, *index);

    bool is_func = false;
    if (STRCT_T == LT_STR)
        FIND_FUNC_NAME(is_func, STRCT_STR);

    if ((STRCT_T != LT_STR) || is_func)
    {
        TreeNode *exp_node = GetExp(lex_structs, index);
        LEX_ERROR(exp_node == NULL, NULL, *index);

        LEX_ERROR(STRCT_T != LT_ST_SEP, NULL, *index);
        (*index)++;

        return exp_node;    
    }
    
    STR_NCTOR(var_name, STRCT_STR);
    (*index)++;

    LEX_ERROR(STRCT_T != LT_EQ, NULL, *index);
    (*index)++;
    T_NCTOR(eq_node, T_EQ);

    TIE(eq_node, var_name, TREE_TIE_LEFT);

    TreeNode *exp_node = GetExp(lex_structs, index);
    LEX_ERROR(exp_node == NULL, NULL, *index);

    TIE(eq_node, exp_node, TREE_TIE_RIGHT);

    return eq_node;
}
//___________________________________________________________________________________________________


TreeNode *GetExp(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);
    
    struct TreeNode *exp_node = GetAdd(lex_structs, index);
    LEX_ERROR(exp_node == NULL, NULL, *index);

    return exp_node;
}
//___________________________________________________________________________________________________

TreeNode *GetAdd(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    struct TreeNode *node = GetMul(lex_structs, index);
    LEX_ERROR(node == NULL, NULL, *index);

    while (STRCT_T == T_OP && (STRCT_OP == OP_ADD || STRCT_OP == OP_SUB))
    {
        ArithmOp op = STRCT_OP;
        (*index)++;

        struct TreeNode *scnd_node = GetMul(lex_structs, index);
        LEX_ERROR(scnd_node == NULL, NULL, *index);

        T_NCTOR(parent_node, T_OP);
        parent_node->value->arithm_op = op;

        TIE(parent_node,      node, TREE_TIE_LEFT);
        TIE(parent_node, scnd_node, TREE_TIE_RIGHT);

        node = parent_node;
    }

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetMul(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    TreeNode *node = GetBrackets(lex_structs, index);
    LEX_ERROR(node == NULL, NULL, *index);

    while (STRCT_T == T_OP && (STRCT_OP == OP_MUL || STRCT_OP == OP_DIV))
    {
        ArithmOp op = STRCT_OP;
        (*index)++;

        TreeNode *scnd_node = GetBrackets(lex_structs, index);
        LEX_ERROR(scnd_node == NULL, NULL, *index);

        T_NCTOR(parent_node, T_OP);
        parent_node->value->arithm_op = op;

        TIE(parent_node,      node, TREE_TIE_LEFT);
        TIE(parent_node, scnd_node, TREE_TIE_RIGHT);

        node = parent_node;
    }

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetBrackets(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    struct TreeNode *node = NULL;

    if (STRCT_T == LT_EXP_L_BRCKT)
    {
        (*index)++;

        node = GetAdd(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);

        LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NULL, *index);
        (*index)++;
    }

    else
    {
        node = GetArg(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);
    }

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetArg(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    TreeNode *node = NULL;

    int i_old = *index;
    if (STRCT_T == LT_NUM)
    {
        T_NCTOR(num_node, T_NUM);
        num_node->value->num  = STRCT_NUM;
        node = num_node;
        (*index)++;
    }
    
    else if (STRCT_T == LT_STR)
    {
        unsigned int func_amount = 0;
        const char **func_names = FindFuncNames(lex_structs, &func_amount);
        LEX_ERROR(func_names == NULL, NULL, *index);

        bool is_func = false;
        FIND_FUNC_NAME(is_func, STRCT_STR);

        if (!is_func)
        {
            STR_NCTOR(var_node, STRCT_STR);
            node = var_node;
            (*index)++;
        }

        else
        {
            node = GetFunc(lex_structs, index);
            LEX_ERROR(node == NULL, NULL, *index);    
        }
    }

    LEX_ERROR(i_old == *index, NULL, *index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetFunc(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    T_NCTOR(call_node, T_CALL);
    STR_NCTOR(func_name, STRCT_STR);
    TIE(call_node, func_name, TREE_TIE_LEFT);
    (*index)++;

    LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NULL, *index);
    (*index)++;

    TreeNode *extra_node  = NULL;
    bool first_cycle = true;
    while (STRCT_T != LT_EXP_R_BRCKT)
    {
        if (!first_cycle)
        {
            LEX_ERROR(STRCT_T != LT_COMMA, NULL, *index);
            (*index)++;
        }

        T_NCTOR(param_node, T_PARAM);                

        TreeNode *exp_node = GetExp(lex_structs, index);
        LEX_ERROR(exp_node == NULL, NULL, *index);

        TIE(param_node, exp_node, TREE_TIE_LEFT);

        if (first_cycle)
        {
            TIE(func_name, param_node, TREE_TIE_LEFT);
            extra_node  = param_node;
            first_cycle = false;
            continue;
        }

        TIE(extra_node, param_node, TREE_TIE_RIGHT);
        extra_node = param_node;
    }
    (*index)++;

    return call_node;
}
//___________________________________________________________________________________________________

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