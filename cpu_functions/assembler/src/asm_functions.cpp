
#include "../include/asm.h"

#define CHECK_AND_WRITE(cmd, str_code, code_num)       \
            if (stricmp(cmd, str_code) == 0)           \
            {                                          \
                code[field->pc] = code_num;            \
            }


#define SKIP_DIGIT()                                   \
            while(isdigit(**buf) || (**buf) == '.' || (**buf) == ',') {(*buf)++;}


int AssemblyUserCode(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    double *code = field->code_buffer;
    char   *buf  = field->char_buffer;

    char cmd[MAX_WORD_LEN] = {};

    field->pc = 0;   //needed for second assembly

    while (true)
    {
        char *is_label = NULL;

        while(isspace(*buf) || *buf == '\0') 
        { 
            buf++; 
            if (*buf == EOF)
                break;
        }

        if (*buf == EOF)
            break;
        
        sscanf(buf, " %s", cmd);
        buf += strlen(cmd) + 1;

        //find label symb
        is_label = strchr(cmd, ':');

        //process label
        if (is_label)
        {
            int process_label_err = ProcessLabel(field, cmd);
            ERROR_CHECK(process_label_err, PROCESS_LABEL_ERROR);
        }

        #define DEF_CMD(name, num, arg, cpu_code)                     \
                if (stricmp(cmd, #name) == 0)                         \
                {                                                     \
                    if (arg)                                          \
                    {                                                 \
                        *((int *)(code + field->pc)) |= name##_CODE;  \
                        int readarg_err = ReadArg(field, &buf);       \
                        ERROR_CHECK(readarg_err, SYNTAX_ERROR);       \
                    }                                                 \
                    else                                              \
                        *((int *)(code + field->pc)) = name##_CODE;   \
                }                                                     \

        #include "../../cmd.h"

        #undef DEF_CMD

        if (!is_label)
            field->pc++;
    }

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int ProcessLabel(struct AsmField *field, char *cmd)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(cmd   == NULL, PTR_NULL);

    int cmd_len = strlen(cmd);
    int arg  = 0;

    ERROR_CHECK(cmd[cmd_len - 1] != ':', SYNTAX_ERROR);
    cmd[cmd_len - 1] = '\0';

    FindLabelValue(field, cmd, &arg);

    if (arg == LABEL_VALUE_POISON)
    {
        for (int i = 0; i <= (int)LABELS_AMOUNT; i++)
        {
            ERROR_CHECK(i == LABELS_AMOUNT, LABEL_OVERFLOW_ERROR);

            if (field->labels[i].value == LABEL_VALUE_POISON
                && strcmp(field->labels[i].name, "") == 0)
            {
                strcpy(field->labels[i].name, cmd);

                field->labels[i].value = field->pc;

                break;
            }
        }
    }

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int ReadArg(struct AsmField *field, char **buf)
{
    char str[MAX_WORD_LEN] = {};

    bool label   = false;
    bool is_plus = false;
    bool memory  = false;

    if (**buf == '[')
    {
        memory = true;
        *((int *)(field->code_buffer + field->pc)) |= MEMORY_CODE;
        (*buf)++;
    }

    sscanf(*buf, "%s", str);

    char  *plus_ptr = strchr(str, '+');
    char *label_ptr = strchr(str, ':');

    if (plus_ptr)
    {
        is_plus = true;
        *((int *)(field->code_buffer + field->pc)) |= REGISTER_CODE;
        *((int *)(field->code_buffer + field->pc)) |= IMMEDIATE_CONST_CODE;
    }

    if (label_ptr)
        label = true;

    ERROR_CHECK(label && is_plus, SYNTAX_ERROR);

    if (is_plus)
    {
        int read_plus_construct_err = ReadPlusConstruction(field, buf);
        ERROR_CHECK(read_plus_construct_err, READ_PLUS_CONSTRUCT_ERROR);
    }

    //read label
    else if (label)
    {
        int read_label_err = ReadLabel(field, buf);
        ERROR_CHECK(read_label_err, READ_LABEL_ERROR);
    }

    //read register
    else if (**buf == 'r')
    {
        *((int *)(field->code_buffer + field->pc)) |= REGISTER_CODE;

        int reg_val = 0;
        int isreg_err = IsReg(*buf, &reg_val);
        ERROR_CHECK(isreg_err, ISREG_ERROR);

        *((int *)(field->code_buffer + (++field->pc))) = reg_val;

        (*buf) += REGISTER_LENGTH;
    }

    //read digit
    else if (isdigit(**buf) || **buf == '-')
    {
        int read_digit_err = ReadDigit(field, buf);
        ERROR_CHECK(read_digit_err, READ_DIGIT_ERROR);
    }
    
    else
       return SYNTAX_ERROR;

    if (memory)
    {
        ERROR_CHECK(**buf != ']', SYNTAX_ERROR);
        (*buf)++;
    }

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int ReadDigit(struct AsmField *field, char **buf)
{
    ERROR_CHECK(buf == NULL, PTR_NULL);
    ERROR_CHECK(field == NULL, PTR_NULL);

    double arg  = 0;

    *((int *)(field->code_buffer + field->pc)) |= IMMEDIATE_CONST_CODE;

    sscanf(*buf, "%lf", &arg);

    field->code_buffer[++field->pc] = arg;

    (*buf)++;
    SKIP_DIGIT();

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int ReadPlusConstruction(struct AsmField *field, char **buf)
{
    ERROR_CHECK(buf == NULL, PTR_NULL);
    ERROR_CHECK(field == NULL, PTR_NULL);

    double arg = 0;

    sscanf(*buf, "%lf", &arg);

    field->code_buffer[++(field->pc)] = arg;

    SKIP_DIGIT();

    ERROR_CHECK(**buf != '+', SYNTAX_ERROR);
    (*buf)++;

    int reg_val = 0;
    int isreg_err = IsReg(*buf, &reg_val);
    ERROR_CHECK(isreg_err, ISREG_ERROR);

    *((int *)(field->code_buffer + (++field->pc))) = reg_val;

    (*buf) += REGISTER_LENGTH;

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int IsReg(char *buf, int *arg)
{
    ERROR_CHECK(buf == NULL, PTR_NULL);
    ERROR_CHECK(arg == NULL, PTR_NULL);

    ERROR_CHECK(*(buf + 1) < 'a' || *(buf + 1) > 'd', SYNTAX_ERROR);
    ERROR_CHECK(*(buf + 2) != 'x', SYNTAX_ERROR);

    *arg = *(buf+1) - 'a' + 1;

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int ReadLabel(struct AsmField *field, char **buf)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(buf   == NULL, PTR_NULL);

    int arg = 0;
    char label_name[MAX_WORD_LEN] = {};

    ERROR_CHECK(**buf != ':', SYNTAX_ERROR);

    *((int *)(field->code_buffer + field->pc)) |= IMMEDIATE_CONST_CODE;

    (*buf)++;

    sscanf(*buf, "%s", label_name);

    int check_err = FindLabelValue(field, label_name, &arg);
    ERROR_CHECK(check_err, FIND_LABEL_VALUE_ERROR);

    printf("arg = %d, file: %s, line: %d\n", arg, __FILE__, __LINE__);
    *((int *)(field->code_buffer + (++field->pc))) = arg;
    (*buf) += strlen(label_name) + 1;

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int FindLabelValue(struct AsmField *field, char *label_name, int *arg)
{
    ERROR_CHECK(field      == NULL, PTR_NULL);
    ERROR_CHECK(label_name == NULL, PTR_NULL);
    ERROR_CHECK(arg        == NULL, PTR_NULL);

    int label_num = 0;

    for (int index = 0; index < (int)LABELS_AMOUNT; index++, label_num = index)
    {
        if (strcmp(field->labels[label_num].name, label_name) == 0)
        {
            *arg = field->labels[label_num].value;
            break;
        }
    }

    if (label_num == LABELS_AMOUNT)
            *arg = LABEL_VALUE_POISON;

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int AsmFieldCtor(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    *field = (struct AsmField){};

    field->char_buffer = NULL;
    field->chars_count = 0;
    field->code_buffer = NULL;
    field->lines_count = 0;

    field->pc = 0;

    field->onegin_field = NULL;

    InitializeLabels(field);

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int InitializeLabels(struct AsmField *field)
{
    const char *label_poison_name = "";

    for (unsigned int label_num = 0; label_num < LABELS_AMOUNT; label_num++)
    {
        strcpy(field->labels[label_num].name, label_poison_name);

        field->labels[label_num].value = LABEL_VALUE_POISON;
    }

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------

int AsmFieldDtor(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    ERROR_CHECK(field->char_buffer == NULL, PTR_NULL);
    ERROR_CHECK(field->code_buffer == NULL, PTR_NULL);
    free(field->char_buffer);
    free(field->code_buffer);

    return SUCCESS;
}
