
#include "../include/libraries.h"

#ifndef CANARY_PROTECTION
#define CANARY_PROTECTION
#endif

#ifndef HASH_PROTECTION
#define HASH_PROTECTION
#endif

#ifndef CTOR_DEBUG
#define CTOR_DEBUG
#endif

int StackConstructor(struct Stack *stk, size_t capacity,
                     const char *file_name, int line_num, const char *func_name)
{
    SOFT_ASSERT(stk != 0);
    SOFT_ASSERT(file_name != 0);
    SOFT_ASSERT(func_name != 0);

    if (stk == NULL)
        return PTR_NULL;

    if ((*stk).status == ALIVE_STATUS)
        return CTORING_ALIVE_STACK;

    #ifdef CANARY_PROTECTION

    (*stk).left_stk_canary  = LEFT_STK_CANARY_CONST;
    (*stk).right_stk_canary = RIGHT_STK_CANARY_CONST;

    (*stk).data = (Elem_t*) calloc(capacity + DATA_CANARY_AMOUNT, sizeof(Elem_t));

    *((*stk).data)                =  LEFT_DATA_CANARY_CONST;
    *((*stk).data + capacity + 1) = RIGHT_DATA_CANARY_CONST;

    (*stk).data++;

    #else

    (*stk).data = (Elem_t *) calloc(capacity, sizeof(Elem_t));

    #endif


    if ((*stk).data == NULL)
        return CALLOC_ERROR;

    (*stk).data_size = 0;

    (*stk).data_capacity = capacity;

    #ifdef CTOR_DEBUG
    if (file_name != NULL)
        (*stk).info.file_name = file_name;
    if (line_num != POISON_INT)
        (*stk).info.line_num  = line_num;
    if (func_name != NULL)
        (*stk).info.func_name = func_name;
    #else
    (*stk).info.file_name = POISON_STR;
    (*stk).info.line_num  = POISON_INT;
    (*stk).info.func_name = POISON_STR;
    #endif

    (*stk).err_code = 0;
    (*stk).dumped   = false;
    (*stk).status   = ALIVE_STATUS;

    for (unsigned int index = 0; index < capacity; index++)
        *((*stk).data + index) = POISON_VALUE;

    #ifdef HASH_PROTECTION
    RECALCHASH(stk);
    #endif

    STACKVERIFICATOR(stk);

    return SUCCESS;
}

int StackDtor(struct Stack *stk)
{
    SOFT_ASSERT(stk != 0);

    if (stk == NULL)
        return PTR_NULL;

    #ifdef CANARY_PROTECTION
    (*stk).data--;
    #endif

    free((*stk).data);
    //for (int index = 0; index < data_size; index++)
      //  (*stk).data[index] = POISON_VALUE;

    (*stk).data = (Elem_t*)POISON_POINTER;

    (*stk).data_size = 0xB040BA;

    (*stk).data_capacity = 0xB040BA;

    (*stk).err_code = 0xB040BA;

    (*stk).status = DEAD_STATUS;

    return SUCCESS;
}


int GetSize(struct Stack *stk)
{
    SOFT_ASSERT(stk != 0);

    if (stk == NULL)
        return -1;

    return (*stk).data_size;
}
