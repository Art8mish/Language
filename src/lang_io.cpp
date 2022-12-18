
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
            
            case LT_FUNC  : fprintf(log_f, "[%d]: FUNC\n", index);
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