#ifndef LIBRARIES_H_INCLUDED
#define LIBRARIES_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"


#ifndef CANARY_PROTECTION
#define CANARY_PROTECTION
#endif

#ifndef HASH_PROTECTION
#define HASH_PROTECTION
#endif

#define STACK_VERIFY


#define ASSERT_MODE

#ifdef ASSERT_MODE

#define SOFT_ASSERT(condition)                                                    \
            do                                                                    \
            {                                                                     \
                if (!(condition))                                                 \
                    printf("Error in %s; file: %s; num of line: %d \n",           \
                           #condition, __FILE__, __LINE__);                       \
            } while(false)
#else

#define SOFT_ASSERT(condition) ;

#endif

#define Assert(condition, error_type)                                             \
        SOFT_ASSERT(condition);                                                   \
        do                                                                        \
        {                                                                         \
            if (!(condition))                                                     \
            {                                                                     \
                return error_type;                                                \
            }                                                                     \
        } while(false)


#ifdef STACK_VERIFY

#define STACKVERIFICATOR(cond)                                                    \
            StackVerification(cond, #cond)

#else

#define STACKVERIFICATOR(cond) ;

#endif


#ifdef CANARY_PROTECTION
    #define ON_CANARY_PROT(cond)                                                  \
        cond
#else
    #define ON_CANARY_PROT(cond)

#endif

#ifdef HASH_PROTECTION

#define RECALCHASH(stk)                                                           \
            FindStackHash(stk);                                                   \
            FindDataHash(stk)

#endif

#define CTOR_DEBUG

#ifdef CTOR_DEBUG
#define STACKCTOR(cond1, cond2)                                                   \
            StackConstructor(cond1, cond2, __FILE__, __LINE__, __FUNCTION__)

#else

#define STACKCTOR(cond1, cond2)                                                   \
            StackConstructor(cond1, cond2)

#endif

#define ARGNAME(arg)                                                              \
            #arg

#define STACKDUMP(cond1, cond2)                                                   \
            StackDumpFunction(cond1, cond2, __FILE__, __LINE__, __FUNCTION__)

#define POISON_POINTER 13
#define POISON_STR "POISON_STR"

typedef unsigned long long Guard_t;

const Guard_t  LEFT_STK_CANARY_CONST = 0xDEADBEEF;
const Guard_t RIGHT_STK_CANARY_CONST = 0xDEADBEEF;

const Elem_t  LEFT_DATA_CANARY_CONST = (Elem_t)0xABCDDCBA;
const Elem_t RIGHT_DATA_CANARY_CONST = (Elem_t)0xABCDDCBA;

const int DATA_CANARY_AMOUNT = 2;


/*#define GetPoison(X) _Generic((X), \
        int: GetPoisonInt,           \
      float: GetPoisonFloat,         \
     double: GetPoisonDouble,        \
      void*: GetPoisonPointer,       \
    default: GetPoisonInt            \
    )(X) */



enum StackStatus
{
    DEAD_STATUS  = 0,
    ALIVE_STATUS = 1
};

enum FunctionError
{
    SUCCESS                   = 0,
    PTR_NULL                  = 1,
    OPERATION_WITH_DEAD_STACK = 2,
    ERROR_INTO_STACK          = 3,
    NONEXISTENT_VALUE         = 4,  //for StackPop
    CALLOC_ERROR              = 5,
    REALLOC_ERROR             = 6,
    HASH_CALC_ERROR           = 7,
    CTORING_ALIVE_STACK       = 8,
    OPENING_FILE_ERROR        = 9,
    CLOSING_FILE_ERROR        = 10
};

enum StackError
{
             NULL_POINTER_ERROR = 1 << 0,
    SIZES_INCOMPATIBILITY_ERROR = 1 << 1,
     LEFT_STK_CANARY_DEAD_ERROR = 1 << 2,
    RIGHT_STK_CANARY_DEAD_ERROR = 1 << 3,
    LEFT_DATA_CANARY_DEAD_ERROR = 1 << 4,
   RIGHT_DATA_CANARY_DEAD_ERROR = 1 << 5,
      STACK_HASH_MISMATCH_ERROR = 1 << 6,
       DATA_HASH_MISMATCH_ERROR = 1 << 7,
      DATA_SIZE_LESS_NULL_ERROR = 1 << 8,
       CAPACITY_LESS_NULL_ERROR = 1 << 9
};


struct StackInfo
{
    const char *file_name;
    int   line_num;
    const char *func_name;
};

struct Stack
{
    ON_CANARY_PROT(Guard_t left_stk_canary);

    Elem_t          *data;

    size_t           data_size;
    size_t           data_capacity;

    struct StackInfo info;

    int              err_code;
    StackStatus      status;
    bool             dumped;

    Guard_t          stack_hash;
    Guard_t          data_hash;

    ON_CANARY_PROT(Guard_t right_stk_canary);
};

#include "stack_functions.h"

#endif // LIBRARIES_H_INCLUDED
