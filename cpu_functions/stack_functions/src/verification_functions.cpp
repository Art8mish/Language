
#include "../include/libraries.h"

static const Guard_t HASH_CONST = 0xBACC;

#ifndef CANARY_PROTECTION
#define CANARY_PROTECTION
#endif

#ifndef HASH_PROTECTION
#define HASH_PROTECTION
#endif

#ifndef CTOR_DEBUG
#define CTOR_DEBUG
#endif

#define PRINTERROR(error, print_text)               \
            if ((*stk).err_code & error)            \
                fprintf(log_file, print_text)

const char *log_file_name = "log_file.txt";

const double EPS = 1e-7;
#define DBL_CMP(arg, num)                                        \
            (fabs((arg) - (num)) <= EPS)


#define PRINT_CANARY

int StackVerification(struct Stack *stk, const char *stk_name)
{
    SOFT_ASSERT(stk != 0);

    if (stk == NULL)
        return PTR_NULL;

    if ((*stk).status == DEAD_STATUS)
        return OPERATION_WITH_DEAD_STACK;


    if ((*stk).data == NULL)
        (*stk).err_code |= NULL_POINTER_ERROR;

    if ((*stk).data_capacity < (*stk).data_size)
        (*stk).err_code |= SIZES_INCOMPATIBILITY_ERROR;

    if ((int)(*stk).data_size < 0)
        (*stk).err_code |= DATA_SIZE_LESS_NULL_ERROR;

    if ((int)(*stk).data_capacity  < 0)
        (*stk).err_code |= CAPACITY_LESS_NULL_ERROR;


    #ifdef CANARY_PROTECTION
    if ((*stk).left_stk_canary != LEFT_STK_CANARY_CONST)
        (*stk).err_code |= LEFT_STK_CANARY_DEAD_ERROR;

    if ((*stk).right_stk_canary != RIGHT_STK_CANARY_CONST)
        (*stk).err_code |= RIGHT_STK_CANARY_DEAD_ERROR;

    if(!((*stk).err_code & NULL_POINTER_ERROR))
    {
        if (!DBL_CMP(*((*stk).data - 1), LEFT_DATA_CANARY_CONST))
            (*stk).err_code |= LEFT_DATA_CANARY_DEAD_ERROR;

        if (!DBL_CMP(*((*stk).data + (*stk).data_capacity), RIGHT_DATA_CANARY_CONST))
            (*stk).err_code |= RIGHT_DATA_CANARY_DEAD_ERROR;
    }
    #endif

    #ifdef HASH_PROTECTION
    if (!(*stk).err_code && (*stk).status)
    {
        Guard_t stack_hash_value = 0;
        FindStackHash(stk, &stack_hash_value);

        Guard_t  data_hash_value =  0;
        FindDataHash (stk, &data_hash_value);

        if ((*stk).stack_hash != stack_hash_value)
            (*stk).err_code |= STACK_HASH_MISMATCH_ERROR;

        if ((*stk).data_hash != data_hash_value)
            (*stk).err_code |= DATA_HASH_MISMATCH_ERROR;
    }
    #endif

    if ((*stk).err_code && (*stk).status && !(*stk).dumped)
        STACKDUMP(stk, stk_name);

    return SUCCESS;
}

int StackDumpFunction(struct Stack *stk, const char *stk_name,
                      const char *file_name, int line_num, const char *func_name)
{
    SOFT_ASSERT(stk != 0);
    SOFT_ASSERT(stk_name != 0);
    SOFT_ASSERT(file_name != 0);
    SOFT_ASSERT(func_name != 0);

    if (stk == NULL)
        return PTR_NULL;

    FILE *log_file = fopen(log_file_name, "a");

    if (log_file == NULL)
        return OPENING_FILE_ERROR;

    const char *error_status = ((*stk).err_code) ? "ERROR" : "ok";

    (*stk).dumped = true;

    fprintf(log_file, "%s at %s(%d)\n", func_name, file_name, line_num);
    fprintf(log_file, "Stack[%p] (%s) \"%s\" ", stk, error_status, stk_name);

    #ifdef CTOR_DEBUG
    if (!strcmp((*stk).info.func_name, POISON_STR))
        fprintf(log_file, "at %s ", (*stk).info.func_name);
    else
        fprintf(log_file, "at DON'T KNOW ");

    if (!strcmp((*stk).info.file_name, POISON_STR))
        fprintf(log_file, "at %s", (*stk).info.file_name);
    else
        fprintf(log_file, "at DON'T KNOW ");

    if ((*stk).info.line_num != POISON_INT)
        fprintf(log_file, "(%d)\n", (*stk).info.line_num);
    else
        fprintf(log_file, "(DON'T KNOW)\n");
    #endif

    fprintf(log_file, "{\n");

    if ((*stk).status)
        fprintf(log_file, "    status   = ALIVE (%d)\n", (*stk).status);

    else
    {
        fprintf(log_file, "    status = DEAD  (%d)\n", (*stk).status);
        fprintf(log_file, "}\n\n");
        return SUCCESS;
    }

    #ifdef CANARY_PROTECTION

    #ifdef PRINT_CANARY
    if ((*stk).left_stk_canary == LEFT_STK_CANARY_CONST)
        fprintf(log_file, "    left_stk_canary  = %X (ALIVE)\n", (*stk).left_stk_canary);

    else
        fprintf(log_file, "    left_stk_canary  = %X (DEAD)\n", (*stk).left_stk_canary);

    if ((*stk).right_stk_canary == RIGHT_STK_CANARY_CONST)
        fprintf(log_file, "    right_stk_canary  = %X (ALIVE)\n", (*stk).right_stk_canary);

    else
        fprintf(log_file, "    right_stk_canary  = %X (DEAD)\n", (*stk).right_stk_canary);


    if (((*stk).err_code & LEFT_DATA_CANARY_DEAD_ERROR) && !((*stk).err_code & NULL_POINTER_ERROR))
    {
        fprintf(log_file, "    left_data_canary   = ");
        fprintf(log_file, ELEM_T_SPECIFIER, *((*stk).data - 1));
        fprintf(log_file, " (DEAD)\n");
    }

    else if (!((*stk).err_code & NULL_POINTER_ERROR))
    {
        fprintf(log_file, "    left_data_canary   = ");
        fprintf(log_file, ELEM_T_SPECIFIER, *((*stk).data - 1));
        fprintf(log_file, " (ALIVE)\n");
    }

    if ((*stk).err_code & RIGHT_DATA_CANARY_DEAD_ERROR && !((*stk).err_code & NULL_POINTER_ERROR))
    {
        fprintf(log_file, "    right_data_canary  = ");
        fprintf(log_file, ELEM_T_SPECIFIER, *((*stk).data + (*stk).data_capacity));
        fprintf(log_file, " (DEAD)\n");
    }

    else if (!((*stk).err_code & NULL_POINTER_ERROR))
    {
        fprintf(log_file, "    right_data_canary  = ");
        fprintf(log_file, ELEM_T_SPECIFIER, *((*stk).data + (*stk).data_capacity));
        fprintf(log_file, " (ALIVE)\n");
    }
    #endif

    #endif


    fprintf(log_file, "    size     = %d\n", (*stk).data_size);
    fprintf(log_file, "    capacity = %d\n", (*stk).data_capacity);
    fprintf(log_file, "    data[%p]\n", (*stk).data);

    fprintf(log_file, "     {\n");

    if ((*stk).data == (Elem_t*)POISON_POINTER)
        fprintf(log_file, "        NO DATA (POISON)\n");

    else if ((*stk).data == NULL)
        fprintf(log_file, "        NO DATA (POINTER IS NULL)\n");

    else
    {
        for (unsigned int index = 0; index < (*stk).data_capacity; index++)
        {
            if (!DBL_CMP(*((*stk).data + index), POISON_VALUE))
            {
                fprintf(log_file, "        *[%d] = ", index);
                fprintf(log_file, ELEM_T_SPECIFIER, *((*stk).data + index));
                fprintf(log_file, "\n");
            }

            else
            {
                fprintf(log_file, "         [%d] = ", index);
                fprintf(log_file, ELEM_T_SPECIFIER, *((*stk).data + index));
                fprintf(log_file, " (POISON)\n");
            }
        }
    }

    fprintf(log_file, "     }\n");

    fprintf(log_file, "}\n\n");

    if ((*stk).status)
    {
        if ((*stk).err_code)
            fprintf(log_file, "ERRORS:\n");

        PRINTERROR(NULL_POINTER_ERROR,           "ERROR_1:  NULL_POINTER_ERROR : Pointer to data is NULL\n");

        PRINTERROR(SIZES_INCOMPATIBILITY_ERROR,  "ERROR_2:  SIZES_INCOMPATIBILITY_ERROR : size > capacity\n");

        PRINTERROR(LEFT_STK_CANARY_DEAD_ERROR,   "ERROR_3:  LEFT_STK_CANARY_DEAD_ERROR : ...\n");

        PRINTERROR(RIGHT_STK_CANARY_DEAD_ERROR,  "ERROR_4:  RIGHT_STK_CANARY_DEAD_ERROR : ...\n");

        PRINTERROR(LEFT_DATA_CANARY_DEAD_ERROR,  "ERROR_5:  LEFT_DATA_CANARY_DEAD_ERROR : ...\n");

        PRINTERROR(RIGHT_DATA_CANARY_DEAD_ERROR, "ERROR_6:  RIGHT_DATA_CANARY_DEAD_ERROR : ...\n");

        PRINTERROR(STACK_HASH_MISMATCH_ERROR,    "ERROR_7:  STACK_HASH_MISMATCH_ERROR\n");

        PRINTERROR(DATA_HASH_MISMATCH_ERROR,     "ERROR_8:  DATA_HASH_MISMATCH_ERROR\n");

        PRINTERROR(DATA_SIZE_LESS_NULL_ERROR,    "ERROR_9:  DATA_SIZE_LESS_NULL_ERROR\n");

        PRINTERROR(CAPACITY_LESS_NULL_ERROR,     "ERROR_10: CAPACITY_LESS_NULL_ERROR\n");
    }

    fprintf(log_file, "\n");

    #ifdef HASH_PROTECTION
    RECALCHASH(stk);
    #endif

    STACKVERIFICATOR(stk);

    if (fclose(log_file))
        return CLOSING_FILE_ERROR;

    return SUCCESS;
}


int FindStackHash(struct Stack *stk, Guard_t *returned_hash_value)
{
    SOFT_ASSERT(stk != NULL);

    if (stk == NULL)
        return PTR_NULL;


    Guard_t hash_value = HASH_CONST;
    Guard_t prev_data_hash_value  = (*stk).data_hash;
    Guard_t prev_stack_hash_value = (*stk).stack_hash;

    size_t stack_size = sizeof(*stk);

    char    *stk_byte = (char*)stk;

    (*stk).stack_hash = 0;
    (*stk).data_hash  = 0;


    int hash_calc_error = CalculateGnuHash(stk_byte, stack_size, &hash_value);

    if (hash_calc_error)
        return HASH_CALC_ERROR;


    if (returned_hash_value == NULL)
        (*stk).stack_hash = hash_value;

    else
    {
        (*stk).stack_hash    = prev_stack_hash_value;
        *returned_hash_value = hash_value;
    }

    (*stk).data_hash = prev_data_hash_value;

    return SUCCESS;
}

int FindDataHash(struct Stack *stk, Guard_t *returned_hash_value)
{
    SOFT_ASSERT(stk != NULL);

    if (stk == NULL)
        return PTR_NULL;


    Guard_t hash_value = HASH_CONST;
    Guard_t prev_data_hash_value  = (*stk).data_hash;

    size_t data_size = ((*stk).data_capacity) * sizeof(Elem_t);

    char  *data_byte = (char*)(*stk).data;

    (*stk).data_hash  = 0;


    int hash_calc_error = CalculateGnuHash(data_byte, data_size, &hash_value);

    if (hash_calc_error != SUCCESS)
        return HASH_CALC_ERROR;


    if (returned_hash_value == NULL)
        (*stk).data_hash = hash_value;

    else
    {
        (*stk).data_hash     = prev_data_hash_value;
        *returned_hash_value = hash_value;
    }

    return SUCCESS;
}

int CalculateGnuHash(char *start_byte, size_t bytes_amount, Guard_t *hash_value)
{
    SOFT_ASSERT(start_byte != NULL);
    SOFT_ASSERT(hash_value != NULL);

    if (start_byte == NULL)
        return PTR_NULL;

    if (hash_value == NULL)
        return PTR_NULL;

    *hash_value = HASH_CONST;

    for (unsigned int i = 0; i < bytes_amount; i++)
    {
        *hash_value *= 33;
        *hash_value += *start_byte;
        start_byte++;
    }

    return SUCCESS;
}
