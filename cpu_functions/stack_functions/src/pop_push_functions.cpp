
#include "../include/libraries.h"

#ifndef CANARY_PROTECTION
#define CANARY_PROTECTION
#endif

#ifndef HASH_PROTECTION
#define HASH_PROTECTION
#endif

#define CHECKERROR(cond, error_num)                             \
            if (cond)                                           \
                return error_num


#define NULLCHECKANDVERIFY()                                    \
            if ((*stk).data == NULL)                            \
            {                                                   \
                STACKVERIFICATOR(stk);                          \
                CHECKERROR((*stk).err_code, REALLOC_ERROR);     \
            }


int StackPush(struct Stack *stk, Elem_t value)
{
    SOFT_ASSERT(stk != 0);
    //SOFT_ASSERT(value != NAN);
    //SOFT_ASSERT(value != INFINITY);

    CHECKERROR(stk == 0, PTR_NULL);
    CHECKERROR((*stk).status == DEAD_STATUS, OPERATION_WITH_DEAD_STACK);

    STACKVERIFICATOR(stk);

    CHECKERROR((*stk).err_code, ERROR_INTO_STACK);


    if ((*stk).data_size == (*stk).data_capacity)
        StackRealloc(stk);

    *((*stk).data + (*stk).data_size) = value;

    (*stk).data_size += 1;

    #ifdef HASH_PROTECTION
    RECALCHASH(stk);
    #endif

    STACKVERIFICATOR(stk);

    return SUCCESS;
}

int StackPop(struct Stack *stk, Elem_t *poped_elem)
{
    SOFT_ASSERT(stk != 0);

    CHECKERROR(stk == 0, PTR_NULL);
    CHECKERROR((*stk).status == DEAD_STATUS, OPERATION_WITH_DEAD_STACK);

    STACKVERIFICATOR(stk);

    CHECKERROR((*stk).err_code, ERROR_INTO_STACK);

    CHECKERROR((*stk).data_size == 0, NONEXISTENT_VALUE);


    Elem_t *value = (*stk).data + (*stk).data_size - 1;
    Elem_t value_copy = *value;

    *value = POISON_VALUE;

    (*stk).data_size -= 1;

    #ifdef HASH_PROTECTION
    RECALCHASH(stk);
    #endif

    StackRealloc(stk);

    STACKVERIFICATOR(stk);

    if (poped_elem != NULL)
        *poped_elem = value_copy;

    return SUCCESS;
}

int StackRealloc(struct Stack *stk)
{
    SOFT_ASSERT(stk != 0);

    CHECKERROR(stk == 0, PTR_NULL);

    CHECKERROR((*stk).status == DEAD_STATUS, OPERATION_WITH_DEAD_STACK);

    STACKVERIFICATOR(stk);

    CHECKERROR((*stk).err_code, ERROR_INTO_STACK);

    int capacity = (*stk).data_capacity;
    bool reduce_capacity = false;

    if((*stk).data_capacity == 0)
        (*stk).data_capacity = 10;

    if ((*stk).data_size >= (*stk).data_capacity)
        (*stk).data_capacity *= 2;

    else if (((*stk).data_size + 5) * 2 < (*stk).data_capacity)
    {
        reduce_capacity = true;
        (*stk).data_capacity /= 2;
    }

    else
    {
        #ifdef HASH_PROTECTION
        RECALCHASH(stk);
        #endif

        STACKVERIFICATOR(stk);

        return SUCCESS;
    }

    #ifdef CANARY_PROTECTION

    (*stk).data--;

    if (!reduce_capacity)
        *((*stk).data + capacity + 1) = POISON_VALUE;

    (*stk).data = (Elem_t*) realloc((*stk).data,
                                    ((*stk).data_capacity + DATA_CANARY_AMOUNT) * sizeof(Elem_t));

    NULLCHECKANDVERIFY();

    *((*stk).data + (*stk).data_capacity + 1) = RIGHT_DATA_CANARY_CONST;

    (*stk).data++;

    #else

    (*stk).data = (Elem_t*) realloc((*stk).data, ((*stk).capacity) * sizeof(Elem_t));

    NULLCHECKANDVERIFY();

    #endif

    for (unsigned int index = capacity; index < (*stk).data_capacity; index++)
        (*stk).data[index] = POISON_VALUE;

    #ifdef HASH_PROTECTION
    RECALCHASH(stk);
    #endif

    STACKVERIFICATOR(stk);

    return SUCCESS;
}
