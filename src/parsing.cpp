
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

            if ((value[0]) == '"')
            {
                if (*buf == '"')
                {
                    buf++;
                    printf("buf: %s\n", buf);
                    break;
                }
            }

            else
            {
                if (isspace(*buf))
                    break;

                if (isdigit(value[0]))
                {
                    if (!isdigit(*buf) && *buf != '.')
                        break;
                }

                else 
                {
                    if (!isalnum(value[i-1]) || !isalnum(*buf))
                        break;
                }   
            }                         
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

#define PROC_LVAL_ITER(str, ltype)                          \
            else if (strcmp(value, str) == 0)               \
            {                                               \
                printf("entered %s\n", str);                \
                STRCT_T = ltype;                            \
                (*index)++;                                 \
            }

#define PROC_LVAL_OP_ITER(str, oper)                        \
            else if (strcmp(value, str) == 0)               \
            {                                               \
                printf("entered %s\n", str);                \
                STRCT_T  = LT_OP;                           \
                STRCT_OP = oper;                            \
                (*index)++;                                 \
            }


int ProcessLexValue(LexStruct *lex_structs, char *value, char **buf, unsigned int *index)
{
    LEX_ERROR(lex_structs == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(value       == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(*buf        == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(buf         == NULL, ERROR_NULL_PTR, *index);
    LEX_ERROR(index       == NULL, ERROR_NULL_PTR, *index);

    if (strcmp(value, S_VAR) == 0)
    {   
        printf("entered var\n");
        STRCT_T = LT_VAR;
        (*index)++;
    }
    //     int proc_var_err = ProcessVarInit(lex_structs, buf, index);
    //     LEX_ERROR(proc_var_err, ERROR_PROCESS_VAR_INIT, *index);
    //

    else if (isdigit(value[0]))
    {
        printf("entered digit\n");
        char *new_ptr = NULL;
        double num = strtod(value, &new_ptr);
        LEX_ERROR(new_ptr != value, ERROR_CONVERT_STR_TO_NUM, *index);
        STRCT_T = LT_NUM;
        STRCT_NUM  = num;
        (*index)++;
    }

    PROC_LVAL_ITER(S_ST_L_BRCKT,  LT_STREAM_L_BRCKT)
    PROC_LVAL_ITER(S_ST_R_BRCKT,  LT_STREAM_R_BRCKT)
    
    PROC_LVAL_ITER(S_EXP_L_BRCKT,  LT_EXP_L_BRCKT)
    PROC_LVAL_ITER(S_EXP_R_BRCKT,  LT_EXP_R_BRCKT)
    
    PROC_LVAL_ITER(S_ST_SEP,  LT_ST_SEP)
    PROC_LVAL_ITER(",",  LT_COMMA)

    PROC_LVAL_ITER(S_EQ,  LT_EQ)

    PROC_LVAL_ITER(S_IF,    LT_IF)
    PROC_LVAL_ITER(S_ELSE,  LT_ELSE)

    PROC_LVAL_ITER(S_RET,  LT_RET)

    else if (strcmp(value, S_TYPE) == 0 || strcmp(value, S_VOID) == 0)
    {
        printf("entered func\n");

        if (strcmp(value, S_VOID) == 0)
            STRCT_T = LT_VOID_FUNC;

        else
            STRCT_T = LT_TYPE_FUNC;
        (*index)++;

        int proc_exp_err = ProcessFunction(lex_structs, buf, index);
        LEX_ERROR(proc_exp_err, ERROR_PROCESS_FUNCTION, *index);
    }

    
    PROC_LVAL_OP_ITER(S_ADD,   OP_ADD)
    PROC_LVAL_OP_ITER(S_SUB,   OP_SUB)
    PROC_LVAL_OP_ITER(S_MUL,   OP_MUL)
    PROC_LVAL_OP_ITER(S_DIV,   OP_DIV)
    PROC_LVAL_OP_ITER(S_POW, OP_POW)
    PROC_LVAL_OP_ITER(S_SIN, OP_SIN)
    PROC_LVAL_OP_ITER(S_COS, OP_COS)
    PROC_LVAL_OP_ITER(S_TG,  OP_TG)

    PROC_LVAL_ITER(S_OUT,  LT_IN)
    PROC_LVAL_ITER(S_IN,  LT_OUT)

    else
    {
        printf("entered str\n");
        LEX_STR(value);
        (*index)++;
    }

    return SUCCESS;
}
//___________________________________________________________________________________________________

/*
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
                        **buf != '*' && **buf != '/' && **buf != '^' && !isspace(**buf));
        
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

            case '^' :  STRCT_T  = LT_OP;
                        STRCT_OP = OP_POW;
                        (*index)++;
                        break;

            default  :  LEX_ERROR(true, ERROR_SYNTAX, *index);
        }
        (*buf)++;

        SKIP_SPACE(*buf, ERROR_SYNTAX);
        LEX_ERROR(**buf == ')', ERROR_SYNTAX, *index);
    }

    return SUCCESS;
}*/
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
    (*index)++;

    SKIP_SPACE(*buf, ERROR_SYNTAX);
    LEX_ERROR(**buf != '=', ERROR_SYNTAX, *index);
    (*buf)++;
    SKIP_SPACE(*buf, ERROR_SYNTAX);

//read init val
    // READ_WHILE(str, **buf != ';' && !isspace(**buf));
    // LEX_ERROR(str[0] == '\0', ERROR_SYNTAX, *index);

    // int proc_var_val = ProcessArg(lex_structs, index, str);
    // LEX_ERROR(proc_var_val, ERROR_PROCESS_ARG, *index);

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
        (*index)++;

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
        char *new_ptr = NULL;
        double num = strtod(str, &new_ptr);
        LEX_ERROR(new_ptr != str, ERROR_CONVERT_STR_TO_NUM, *index);
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
        (*index)++;
    }

    return SUCCESS;
}
//___________________________________________________________________________________________________

const char **FindFuncNames(const LexStruct *lex_structs, unsigned int *func_amount)
{
    ERROR_CHECK(lex_structs == NULL, NULL);

    printf("enter find_func_names\n");


    unsigned int func_counter = 0;
    const char **reall_func_names = NULL;

    // bool names_are_found = false;
    // if (names_are_found)
    // {
    //     *func_amount = func_counter;
    //     printf("exit find_func_names\n");
    //     return reall_func_names;
    // }

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
    // names_are_found = true;

    printf("func_names: \n");
    for (unsigned int i = 0; i < func_counter; i++)
        printf("%s\n", func_names[i]);

    printf("exit first time find_func_names\n");

    return reall_func_names;
}
//___________________________________________________________________________________________________

#define T_NCTOR(n_name, n_type, code)                                                   \
            do {                                                                        \
                LangNode *_lang_val = NULL;                                             \
                CREATE_TREE_NODE_VALUE(_lang_val, code, NULL);                          \
                _lang_val->type = n_type;                                               \
                TreeNode *_new_node = TreeNodeCtor(_lang_val, NULL, NULL);              \
                SAFE_LEX_ERROR(_new_node == NULL, code, NULL, *index);                  \
                n_name = _new_node;                                                     \
            } while (false)

#define STR_NCTOR(n_name, n_str, code)                                                  \
            do {                                                                        \
                LangNode *_lang_val = NULL;                                             \
                CREATE_TREE_NODE_VALUE(_lang_val, code, NULL);                          \
                _lang_val->type = T_STR;                                                \
                _lang_val->str  = n_str;                                                \
                TreeNode *_new_node = TreeNodeCtor(_lang_val, NULL, NULL);              \
                SAFE_LEX_ERROR(_new_node == NULL, code, NULL, *index);                             \
                n_name = _new_node;                                                     \
            } while (false)

#define TIE(parent_node, child_node, tie_type, code)                                \
            do {                                                                    \
                int tie_err = TreeNodeTie(parent_node, child_node, tie_type);       \
                SAFE_LEX_ERROR(tie_err, code, NULL, *index);                        \
            } while (false)

#define NDTOR(node) TreeNodeDtor(&node)

TreeNode *ReadLexCode(const LexStruct *lex_structs)
{
    unsigned int index = 0; 
    LEX_ERROR(lex_structs == NULL, NULL, index);

    struct TreeNode *node = GetExternal(lex_structs, &index);
    LEX_ERROR(node == NULL, NULL, index);

    SAFE_LEX_ERROR(lex_structs[index].type != LT_END, NDTOR(node);, NULL, index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetExternal(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("entered GetExternal (%d)\n", *index);

    TreeNode *st_node    = NULL;
    TreeNode *extra_node = NULL;

    bool first_cycle = true;
    while (STRCT_T == LT_VOID_FUNC || STRCT_T == LT_TYPE_FUNC || STRCT_T == LT_VAR)
    {
        TreeNode *curr_st_node = NULL;
        T_NCTOR(curr_st_node, T_ST, ;);

        if (STRCT_T == LT_VOID_FUNC || STRCT_T == LT_TYPE_FUNC)
        {
            TreeNode *func_node = GetFuncDef(lex_structs, index);
            SAFE_LEX_ERROR(func_node == NULL, NDTOR(curr_st_node);, NULL, *index);
            
            TIE(curr_st_node, func_node, TREE_TIE_LEFT, NDTOR(curr_st_node);
                                                        NDTOR(func_node););
        }

        else if (STRCT_T == LT_VAR)
        {
            (*index)++;
            TreeNode *var_node = GetVar(lex_structs, index);
            SAFE_LEX_ERROR(var_node == NULL, NDTOR(curr_st_node);, NULL, *index);
            
            TIE(curr_st_node, var_node, TREE_TIE_LEFT, NDTOR(curr_st_node);
                                                       NDTOR(var_node););
        }

        if (first_cycle)
        {
            st_node = curr_st_node;
            extra_node = curr_st_node;
            first_cycle = false;
            continue;
        }

        TIE(extra_node, curr_st_node, TREE_TIE_RIGHT, NDTOR(extra_node);
                                                      NDTOR(curr_st_node););
        extra_node = curr_st_node;

    }

    printf("exit GetExternal (%d)\n", *index);

    return st_node;
}
//___________________________________________________________________________________________________

TreeNode *GetFuncDef(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    LEX_ERROR(STRCT_T != LT_VOID_FUNC && STRCT_T != LT_TYPE_FUNC, NULL, *index);

    bool void_func = false;
    if (STRCT_T == LT_VOID_FUNC)
        void_func = true;
    (*index)++;

    printf("entered GetFuncDef (%d)\n", *index);

    TreeNode *func_node = NULL;
    T_NCTOR(func_node, T_FUNC, ;);

    SAFE_LEX_ERROR(STRCT_T != LT_STR, NDTOR(func_node);, NULL, *index);
    TreeNode *func_name = NULL;
    STR_NCTOR(func_name, STRCT_STR, NDTOR(func_node););
    (*index)++;

    TIE(func_node, func_name, TREE_TIE_LEFT, NDTOR(func_node);
                                             NDTOR(func_name););

    TreeNode *type_func_node = NULL;
    if (void_func)
        T_NCTOR(type_func_node, T_VOID, NDTOR(func_node););
    else
        T_NCTOR(type_func_node, T_TYPE, NDTOR(func_node););

    TIE(func_name, type_func_node, TREE_TIE_RIGHT, NDTOR(func_node);
                                                   NDTOR(type_func_node););

    SAFE_LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NDTOR(func_node);, NULL, *index);
    (*index)++;

    TreeNode *extra_node = NULL;
    bool first_cycle = true;
    while (STRCT_T != LT_EXP_R_BRCKT)
    {
        if (!first_cycle)
        {
            SAFE_LEX_ERROR(STRCT_T != LT_COMMA, NDTOR(func_node);, NULL, *index);
            (*index)++;
        }

        SAFE_LEX_ERROR(STRCT_T != LT_VAR, NDTOR(func_node);, NULL, *index);
        TreeNode *param_node = NULL;
        T_NCTOR(param_node, T_PARAM, NDTOR(func_node););
        (*index)++;

        TreeNode *var_node = NULL;
        T_NCTOR(var_node, T_VAR, NDTOR(func_node); 
                                 NDTOR(param_node););
        TIE(param_node, var_node, TREE_TIE_LEFT, NDTOR(func_node);
                                                 NDTOR(param_node);
                                                 NDTOR(var_node););

        SAFE_LEX_ERROR(STRCT_T != LT_STR, NDTOR(func_node);
                                          NDTOR(param_node);, NULL, *index);
        TreeNode *var_name = NULL;
        STR_NCTOR(var_name, STRCT_STR, NDTOR(func_node);
                                       NDTOR(param_node););
        (*index)++;
        TIE(var_node, var_name, TREE_TIE_LEFT, NDTOR(func_node);
                                               NDTOR(param_node);
                                               NDTOR(var_name););

        if (first_cycle)
        {
            TIE(func_name, param_node, TREE_TIE_LEFT, NDTOR(func_node);
                                                      NDTOR(param_node););
            extra_node = param_node;
            first_cycle = false;
            continue;
        }

        TIE(extra_node, param_node, TREE_TIE_RIGHT, NDTOR(func_node);
                                                    NDTOR(param_node););
        extra_node = param_node;
    }

    SAFE_LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NDTOR(func_node);, NULL, *index);
    (*index)++;
    
    SAFE_LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NDTOR(func_node);, NULL, *index);
    TreeNode *st_node = GetStatementStream(lex_structs, index);
    SAFE_LEX_ERROR(st_node == NULL, NDTOR(func_node);, NULL, *index);

    TIE(func_node, st_node, TREE_TIE_RIGHT, NDTOR(func_node);
                                            NDTOR(st_node););

    printf("exit GetFuncDef (%d)\n", *index);

    return func_node;
}
//___________________________________________________________________________________________________

TreeNode *GetVar(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetVar (%d)\n", *index);

    TreeNode *var_node = NULL;
    T_NCTOR(var_node, T_VAR, ;);

    SAFE_LEX_ERROR(STRCT_T != LT_STR, NDTOR(var_node);, NULL, *index);
    TreeNode *var_name = NULL;
    STR_NCTOR(var_name, STRCT_STR, NDTOR(var_node););
    (*index)++;

    TIE(var_node, var_name, TREE_TIE_LEFT, NDTOR(var_node);
                                           NDTOR(var_name););

    SAFE_LEX_ERROR(STRCT_T != LT_EQ, NDTOR(var_node);, NULL, *index);
    (*index)++;

    TreeNode *val_node = GetExp(lex_structs, index);
    SAFE_LEX_ERROR(val_node == NULL, NDTOR(var_node);, NULL, *index);

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

    TIE(var_node, val_node, TREE_TIE_RIGHT, NDTOR(var_node);
                                            NDTOR(val_node););

    SAFE_LEX_ERROR(STRCT_T != LT_ST_SEP, NDTOR(var_node);, NULL, *index);
    (*index)++;

    printf("exit GetVar (%d)\n", *index);

    return var_node;
}
//___________________________________________________________________________________________________

TreeNode *GetStatement(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetStatement (%d)\n", *index);

    TreeNode *st_node = NULL;
    T_NCTOR(st_node, T_ST, ;);

    // SAFE_LEX_ERROR(STRCT_T != LT_VAR   &&
    //                STRCT_T != LT_IF    &&
    //                STRCT_T != LT_WHILE &&
    //                STRCT_T != LT_RET   &&
    //                STRCT_T != LT_STR, NDTOR(st_node);, NULL, *index);

    if (STRCT_T == LT_VAR)
    {
        (*index)++;
        TreeNode *var_node = GetVar(lex_structs, index);
        SAFE_LEX_ERROR(var_node == NULL, NDTOR(st_node);, NULL, *index);
        
        TIE(st_node, var_node, TREE_TIE_LEFT, NDTOR(st_node);
                                              NDTOR(var_node););
    }

    else if (STRCT_T == LT_IF)
    {
        (*index)++;
        TreeNode *if_node = GetIf(lex_structs, index);
        SAFE_LEX_ERROR(if_node == NULL, NDTOR(st_node);, NULL, *index);
        
        TIE(st_node, if_node, TREE_TIE_LEFT, NDTOR(st_node);
                                             NDTOR(if_node););
    }

    else if (STRCT_T == LT_WHILE)
    {
        (*index)++;
        TreeNode *while_node = GetWhile(lex_structs, index);
        SAFE_LEX_ERROR(while_node == NULL, NDTOR(st_node);, NULL, *index);
        
        TIE(st_node, while_node, TREE_TIE_LEFT, NDTOR(st_node);
                                                NDTOR(while_node););
    }

    else if (STRCT_T == LT_RET)
    {
        (*index)++;
        TreeNode *exp_node = GetExp(lex_structs, index);
        SAFE_LEX_ERROR(exp_node == NULL, NDTOR(st_node);, NULL, *index);
        
        TIE(st_node, exp_node, TREE_TIE_LEFT, NDTOR(st_node);
                                              NDTOR(exp_node););

        SAFE_LEX_ERROR(STRCT_T != LT_ST_SEP, NDTOR(st_node);, NULL, *index);
        (*index)++;
    }

    else if (STRCT_T == LT_IN || STRCT_T == LT_OUT)
    {
        printf("enter IN/OUT (%d)\n", *index);
        
        TreeNode *exp_node = GetExp(lex_structs, index);
        LEX_ERROR(exp_node == NULL, NULL, *index);

        TIE(st_node, exp_node, TREE_TIE_LEFT, NDTOR(st_node);
                                              NDTOR(exp_node););

        SAFE_LEX_ERROR(STRCT_T != LT_ST_SEP, NDTOR(exp_node);, NULL, *index);
        (*index)++;
    }

    else
    {
        TreeNode *str_node = GetStr(lex_structs, index);
        SAFE_LEX_ERROR(str_node == NULL, NDTOR(st_node);, NULL, *index);
        
        TIE(st_node, str_node, TREE_TIE_LEFT, NDTOR(st_node);
                                              NDTOR(str_node););
    }

    printf("exit GetStatement (%d)\n", *index);

    return st_node;
}
//___________________________________________________________________________________________________

TreeNode *GetStatementStream(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetStatementStream (%d)\n", *index);

    LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NULL, *index);
    (*index)++;

    TreeNode *ret_st_node = NULL;
    TreeNode *extra_node  = NULL;
    bool first_cycle = true;
    while (STRCT_T != LT_STREAM_R_BRCKT)
    {
        TreeNode *st_node = GetStatement(lex_structs, index);
        SAFE_LEX_ERROR(st_node == NULL, NDTOR(ret_st_node);, NULL, *index);

        if (first_cycle)
        {
            ret_st_node = st_node;
            extra_node  = st_node;
            first_cycle = false;
            continue;
        }

        TIE(extra_node, st_node, TREE_TIE_RIGHT, NDTOR(ret_st_node);
                                                 NDTOR(st_node););
        extra_node = st_node;
    }
    (*index)++;

    printf("exit GetStatementStream (%d)\n", *index);

    return ret_st_node;
}
//___________________________________________________________________________________________________

TreeNode *GetIf(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetIf (%d)\n", *index);

    TreeNode *if_node = NULL;
    T_NCTOR(if_node, T_IF, ;);

    SAFE_LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NDTOR(if_node);, NULL, *index);
    (*index)++;

    TreeNode *cond_node = GetExp(lex_structs, index);
    SAFE_LEX_ERROR(cond_node == NULL, NDTOR(if_node);, NULL, *index);

    TIE(if_node, cond_node, TREE_TIE_LEFT, NDTOR(if_node);
                                           NDTOR(cond_node););

    SAFE_LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NDTOR(if_node);, NULL, *index);
    (*index)++;

    SAFE_LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NDTOR(if_node);, NULL, *index);
    TreeNode *if_st_node = GetStatementStream(lex_structs, index);
    SAFE_LEX_ERROR(if_st_node == NULL, NDTOR(if_node);, NULL, *index);

    if (STRCT_T != LT_ELSE)
    {
        TIE(if_node, if_st_node, TREE_TIE_RIGHT, NDTOR(if_node);
                                                 NDTOR(if_st_node););
        printf("exit (no else) GetIf (%d)\n", *index);
        return if_node;
    }

    TreeNode *else_node = NULL;
    T_NCTOR(else_node, T_ELSE, NDTOR(if_node);
                               NDTOR(if_st_node););
    (*index)++;
    TIE(else_node, if_st_node, TREE_TIE_LEFT, NDTOR(if_node);
                                              NDTOR(else_node);
                                              NDTOR(if_st_node););

    TIE(if_node, else_node, TREE_TIE_RIGHT, NDTOR(if_node);
                                            NDTOR(else_node););

    SAFE_LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NDTOR(if_node);, NULL, *index);
    TreeNode *else_st_node = GetStatementStream(lex_structs, index);
    SAFE_LEX_ERROR(else_st_node == NULL, NDTOR(if_node);, NULL, *index);

    TIE(else_node, else_st_node, TREE_TIE_RIGHT, NDTOR(if_node);
                                                 NDTOR(else_st_node););
    
    printf("exit (with else) GetIf (%d)\n", *index);

    return if_node;
}
//___________________________________________________________________________________________________

TreeNode *GetWhile(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetWhile (%d)\n", *index);

    TreeNode *while_node = NULL;
    T_NCTOR(while_node, T_IF, ;);

    SAFE_LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NDTOR(while_node);, NULL, *index);
    (*index)++;

    TreeNode *cond_node = GetExp(lex_structs, index);
    SAFE_LEX_ERROR(cond_node == NULL, NDTOR(while_node);, NULL, *index);

    TIE(while_node, cond_node, TREE_TIE_LEFT, NDTOR(while_node);
                                              NDTOR(cond_node););

    SAFE_LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NDTOR(while_node);, NULL, *index);
    (*index)++;

    SAFE_LEX_ERROR(STRCT_T != LT_STREAM_L_BRCKT, NDTOR(while_node);, NULL, *index);
    TreeNode *while_st_node = GetStatementStream(lex_structs, index);
    SAFE_LEX_ERROR(while_st_node == NULL, NDTOR(while_node);, NULL, *index);

    TIE(while_node, while_st_node, TREE_TIE_RIGHT, NDTOR(while_node);
                                                   NDTOR(while_st_node););

    printf("exit GetWhile (%d)\n", *index);

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

    printf("enter GetStr (%d)\n", *index);
    unsigned int func_amount = 0;
    const char **func_names = FindFuncNames(lex_structs, &func_amount);
    LEX_ERROR(func_names == NULL, NULL, *index);

    bool is_func = false;
    if (STRCT_T == LT_STR)
        FIND_FUNC_NAME(is_func, STRCT_STR);
    free((void *)func_names);

    if ((STRCT_T != LT_STR) || is_func)
    {
        printf("index before GetExp = %d\n", *index);
        TreeNode *exp_node = GetExp(lex_structs, index);
        LEX_ERROR(exp_node == NULL, NULL, *index);

        SAFE_LEX_ERROR(STRCT_T != LT_ST_SEP, NDTOR(exp_node);, NULL, *index);
        (*index)++;

        printf("exit (exp) GetStr\n");

        return exp_node;    
    }
    
    TreeNode *var_name = NULL;
    STR_NCTOR(var_name, STRCT_STR, ;);
    (*index)++;

    SAFE_LEX_ERROR(STRCT_T != LT_EQ, NDTOR(var_name);, NULL, *index);
    (*index)++;

    TreeNode *eq_node = NULL;
    T_NCTOR(eq_node, T_EQ, NDTOR(var_name););

    TIE(eq_node, var_name, TREE_TIE_LEFT, NDTOR(eq_node);
                                          NDTOR(var_name););

    TreeNode *exp_node = GetExp(lex_structs, index);
    SAFE_LEX_ERROR(exp_node == NULL, NDTOR(eq_node);, NULL, *index);

    TIE(eq_node, exp_node, TREE_TIE_RIGHT, NDTOR(eq_node);
                                           NDTOR(exp_node););

    SAFE_LEX_ERROR(STRCT_T != LT_ST_SEP, NDTOR(eq_node);, NULL, *index);
    (*index)++;

    printf("exit (eq) GetStr (%d)\n", *index);

    return eq_node;
}
//___________________________________________________________________________________________________


TreeNode *GetExp(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetExp (%d)\n", *index);
    
    struct TreeNode *exp_node = GetAdd(lex_structs, index);
    LEX_ERROR(exp_node == NULL, NULL, *index);

    printf("exit GetExp (%d)\n", *index);

    return exp_node;
}
//___________________________________________________________________________________________________

TreeNode *GetAdd(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetAdd (%d)\n", *index);

    struct TreeNode *node = GetMul(lex_structs, index);
    LEX_ERROR(node == NULL, NULL, *index);

    while (STRCT_T == LT_OP && (STRCT_OP == OP_ADD || STRCT_OP == OP_SUB))
    {   
        TreeNode *parent_node = NULL;
        T_NCTOR(parent_node, T_OP, NDTOR(node););
        parent_node->value->arithm_op = STRCT_OP;
        (*index)++;

        TIE(parent_node, node, TREE_TIE_LEFT, NDTOR(parent_node);
                                              NDTOR(node););

        TreeNode *extra_node = GetMul(lex_structs, index);
        SAFE_LEX_ERROR(extra_node == NULL, NDTOR(parent_node);, NULL, *index);

        TIE(parent_node, extra_node, TREE_TIE_RIGHT, NDTOR(parent_node);
                                                     NDTOR(extra_node););

        node = parent_node;
    }

    printf("exit GetAdd (%d)\n", *index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetMul(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetMul (%d)\n", *index);

    TreeNode *node = GetPow(lex_structs, index);
    LEX_ERROR(node == NULL, NULL, *index);

    while (STRCT_T == LT_OP && (STRCT_OP == OP_MUL || STRCT_OP == OP_DIV))
    {   
        TreeNode *parent_node = NULL;
        T_NCTOR(parent_node, T_OP, NDTOR(node););
        parent_node->value->arithm_op = STRCT_OP;
        (*index)++;

        TIE(parent_node, node, TREE_TIE_LEFT, NDTOR(parent_node);
                                              NDTOR(node););

        TreeNode *extra_node = GetPow(lex_structs, index);
        SAFE_LEX_ERROR(extra_node == NULL, NDTOR(parent_node);, NULL, *index);

        TIE(parent_node, extra_node, TREE_TIE_RIGHT, NDTOR(parent_node);
                                                     NDTOR(extra_node););

        node = parent_node;
    }

    printf("exit GetMul (%d)\n", *index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetPow(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetPow (%d)\n", *index);

    TreeNode *node = NULL;
    if (STRCT_T == LT_OP && STRCT_OP == OP_POW)
    {   
        TreeNode *parent_node = NULL;
        T_NCTOR(parent_node, T_OP, NDTOR(node););
        parent_node->value->arithm_op = OP_POW;
        (*index)++;

        SAFE_LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NDTOR(parent_node);, NULL, *index);
        (*index)++;

        node = GetAdd(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index); 

        TIE(parent_node, node, TREE_TIE_LEFT, NDTOR(parent_node);
                                              NDTOR(node););

        SAFE_LEX_ERROR(STRCT_T != LT_COMMA, NDTOR(parent_node);, NULL, *index);
        (*index)++;

        TreeNode *extra_node = GetAdd(lex_structs, index);
        SAFE_LEX_ERROR(extra_node == NULL, NDTOR(parent_node);, NULL, *index);

        TIE(parent_node, extra_node, TREE_TIE_RIGHT, NDTOR(parent_node);
                                                     NDTOR(extra_node););

        SAFE_LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NDTOR(parent_node);, NULL, *index);
        (*index)++;

        node = parent_node;
    }

    else
    {
        node = GetUnar(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);
    }

    printf("exit GetPow (%d)\n", *index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetUnar(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetUnar (%d)\n", *index);

    TreeNode *node = NULL;

    if (STRCT_T == LT_OP && (STRCT_OP == OP_SIN || STRCT_OP == OP_COS || STRCT_OP == OP_TG))
    {
        TreeNode *parent_node = NULL;
        T_NCTOR(parent_node, T_OP, ;);
        parent_node->value->arithm_op = STRCT_OP;
        (*index)++;

        node = GetBrackets(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);

        TIE(parent_node, node, TREE_TIE_LEFT, NDTOR(parent_node);
                                              NDTOR(node););
        node = parent_node;
    }

    else
    {
        node = GetBrackets(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);
    }

    printf("exit GetUnar (%d)\n", *index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetBrackets(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetBrackets (%d)\n", *index);
    struct TreeNode *node = NULL;

    if (STRCT_T == LT_EXP_L_BRCKT)
    {
        (*index)++;

        node = GetAdd(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);

        SAFE_LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NDTOR(node);, NULL, *index);
        (*index)++;
    }

    else
    {
        node = GetArg(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);
    }

    printf("exit GetBrackets (%d)\n", *index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetArg(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetArg (%d)\n", *index);

    TreeNode *node = NULL;

    unsigned int i_old = *index;
    if (STRCT_T == LT_NUM)
    {
        TreeNode *num_node = NULL;
        T_NCTOR(num_node, T_NUM, ;);
        num_node->value->num  = STRCT_NUM;

        printf("arg = %g\n", STRCT_NUM);

        node = num_node;
        (*index)++;
    }

    else if (STRCT_T == LT_IN || STRCT_T == LT_OUT)
    {
        node = GetInOut(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);
    }
    
    else if (STRCT_T == LT_STR)
    {
        unsigned int func_amount = 0;
        const char **func_names = FindFuncNames(lex_structs, &func_amount);
        LEX_ERROR(func_names == NULL, NULL, *index);

        bool is_func = false;
        FIND_FUNC_NAME(is_func, STRCT_STR);
        free((void *)func_names);

        if (!is_func)
        {
            TreeNode *var_node = NULL;
            if (strcmp(STRCT_STR, "&") == 0)
                (*index)++;
            STR_NCTOR(var_node, STRCT_STR, ;);  

            printf("arg = %s\n", var_node->value->str);

            node = var_node;
            (*index)++;
        }

        else
        {
            node = GetInOut(lex_structs, index);
            LEX_ERROR(node == NULL, NULL, *index);    
        }
    }

    SAFE_LEX_ERROR(i_old == *index, NDTOR(node);, NULL, *index);

    printf("exit GetArg (%d)\n", *index);

    return node;
}
//___________________________________________________________________________________________________

TreeNode *GetInOut(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetInOut (%d)\n", *index);

    TreeNode *node = NULL;

    if (STRCT_T == LT_IN || STRCT_T == LT_OUT)
    {
        if (STRCT_T == LT_OUT)
            T_NCTOR(node, T_OUT, ;);
        else
            T_NCTOR(node, T_IN, ;);
        (*index)++;

        SAFE_LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NDTOR(node);,  NULL, *index);
        (*index)++;

        SAFE_LEX_ERROR(STRCT_T != LT_STR, NDTOR(node);, NULL, *index);
        SAFE_LEX_ERROR(STRCT_STR[0] != '"', NDTOR(node);, NULL, *index);

        TreeNode *str_node = NULL;
        STR_NCTOR(str_node, strdup(STRCT_STR + 1), NDTOR(node););
        free((void *)STRCT_STR);
        (*index)++;

        TIE(node, str_node, TREE_TIE_RIGHT, NDTOR(node);
                                            NDTOR(str_node););

        SAFE_LEX_ERROR(STRCT_T != LT_COMMA, NDTOR(node);, NULL, *index);
        (*index)++;
        
        TreeNode *param_node = GetParam(lex_structs, index);
        if (param_node != NULL)
            TIE(node, param_node, TREE_TIE_LEFT, NDTOR(node);
                                                 NDTOR(param_node););
        
        SAFE_LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NDTOR(node);, NULL, *index);
        (*index)++;
    } 

    else
    {
        node = GetFunc(lex_structs, index);
        LEX_ERROR(node == NULL, NULL, *index);
    }

    printf("exit GetInOut (%d)\n", *index);

    return node;
}

TreeNode *GetFunc(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    printf("enter GetFunc (%d)\n", *index);

    TreeNode *call_node = NULL;
    T_NCTOR(call_node, T_CALL, ;);

    TreeNode *func_name = NULL;
    STR_NCTOR(func_name, STRCT_STR, NDTOR(call_node););

    TIE(call_node, func_name, TREE_TIE_LEFT, NDTOR(call_node););
    (*index)++;


    SAFE_LEX_ERROR(STRCT_T != LT_EXP_L_BRCKT, NDTOR(call_node);,  NULL, *index);
    (*index)++;

    TreeNode *param_node = GetParam(lex_structs, index);
    if (param_node != NULL)
        TIE(func_name, param_node, TREE_TIE_LEFT, NDTOR(call_node);
                                                  NDTOR(param_node););

    SAFE_LEX_ERROR(STRCT_T != LT_EXP_R_BRCKT, NDTOR(call_node);, NULL, *index);
    (*index)++;

    printf("exit GetFunc (%d)\n", *index);

    return call_node;
}

TreeNode *GetParam(const LexStruct *lex_structs, unsigned int *index)
{
    LEX_ERROR(index       == NULL, NULL, 0);
    LEX_ERROR(lex_structs == NULL, NULL, *index);

    TreeNode *ret_node = NULL;
    TreeNode *extra_node = NULL;
    bool first_cycle = true;
    while (STRCT_T != LT_EXP_R_BRCKT)
    {
        if (!first_cycle)
        {
            SAFE_LEX_ERROR(STRCT_T != LT_COMMA, NDTOR(ret_node);, NULL, *index);
            (*index)++;
        }

        TreeNode *param_node  = NULL;
        T_NCTOR(param_node, T_PARAM, NDTOR(ret_node););                

        TreeNode *exp_node = GetExp(lex_structs, index);
        SAFE_LEX_ERROR(exp_node == NULL, NDTOR(ret_node);
                                         NDTOR(param_node);, NULL, *index);

        TIE(param_node, exp_node, TREE_TIE_LEFT, NDTOR(ret_node);
                                                 NDTOR(param_node);
                                                 NDTOR(exp_node););

        if (first_cycle)
        {
            ret_node    = param_node;
            extra_node  = param_node;
            first_cycle = false;
            continue;
        }

        TIE(extra_node, param_node, TREE_TIE_RIGHT, NDTOR(ret_node);
                                                    NDTOR(param_node););
        extra_node = param_node;
    }

    return ret_node;
}
//___________________________________________________________________________________________________

double ConvertStrToNum(const char *string)
{
    ERROR_CHECK(string == NULL, NAN);

    printf("enter ConvertStrToNum\n");
    
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

    if (*string == '.')
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

    printf("num = %g\n", num);
    printf("exit ConvertStrToNum\n");

    return num;
}