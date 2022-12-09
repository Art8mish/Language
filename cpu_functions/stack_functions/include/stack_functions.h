#ifndef STACK_FUNCTIONS_H_INCLUDED
#define STACK_FUNCTIONS_H_INCLUDED

const int POISON_INT = -1;

int StackConstructor(struct Stack *stk, size_t capacity,
                     const char *file_name = NULL, int line_num = POISON_INT, const char *func_name = NULL);

int StackDtor(struct Stack *stk);

int GetSize(struct Stack *stk);


int StackVerification(struct Stack *stk, const char *stk_name);

int StackDumpFunction(struct Stack *stk, const char *stk_name,
                                const char *file_name, int line_num, const char *func_name);

int FindStackHash(struct Stack *stk, Guard_t *returned_hash_value = NULL);
int  FindDataHash(struct Stack *stk, Guard_t *returned_hash_value = NULL);
int CalculateGnuHash(char *start_byte, size_t bytes_amount, Guard_t *hash_value);


//---------------------------------------------------------------------------
//! Add value on the top Stack
//!
//! @param [in] stk   Pointer to stack
//! @param [in] value Adding value
//!
//! @return Error code
//!
//---------------------------------------------------------------------------

int StackPush(struct Stack *stk, Elem_t value);

//---------------------------------------------------------------------------
//! Add value on the top Stack
//!
//! @param [in]  stk        Pointer to stack
//! @param [out] poped_elem Adding value
//!
//! @return Error code
//!
//! @note Don't return poped_elem if not transmitted
//---------------------------------------------------------------------------

int StackPop(struct Stack *stk, Elem_t *poped_elem = NULL);

int StackRealloc(struct Stack *stk);

#endif // STACK_FUNCTIONS_H_INCLUDED
