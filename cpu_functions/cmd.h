
#ifndef DEF_CMD
#define DEF_CMD(cmd, num, arg, cpu_code)
#endif


DEF_CMD(HLT, 0, 0,
{
    return 0;
})

DEF_CMD(PUSH, (1 << 0), 1,
{
    StackPush(&field->stk, *ptr_arg_val);
})

DEF_CMD(POP,  (1 << 1), 1,
{
    //txSleep(0.5);
    StackPop (&field->stk, &rhs);
    *ptr_arg_val = rhs;
})

DEF_CMD(JMP,  (1 << 2), 1,
{
    field->pc = *((int*)(ptr_arg_val)) - 1;
})

DEF_CMD(JB,   (1 << 3), 1,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    if (lhs < rhs)
        field->pc = *((int*)(ptr_arg_val)) - 1;
})

DEF_CMD(JBE,  (1 << 4), 1,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    if (lhs <= rhs)
        field->pc = *((int*)(ptr_arg_val)) - 1;
})

DEF_CMD(JA,   (1 << 5), 1,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    if (lhs > rhs)
        field->pc = *((int*)(ptr_arg_val)) - 1;
})

DEF_CMD(JAE,  (1 << 6), 1,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    if (lhs >= rhs)
        field->pc = *((int*)(ptr_arg_val)) - 1;
})

DEF_CMD(JE,   (1 << 7), 1,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    if (DBLCMP(lhs, rhs))
        field->pc = *((int*)(ptr_arg_val)) - 1;
})

DEF_CMD(JNE,  (1 << 8), 1,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    if (!DBLCMP(lhs, rhs))
        field->pc = *((int*)(ptr_arg_val)) - 1;
})

DEF_CMD(CALL, (1 << 9), 1,
{
    StackPush(&field->ret_adr, field->pc + 1);
    field->pc = *((int*)(ptr_arg_val)) - 1;
})

DEF_CMD(ADD, 3, 0,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    StackPush(&field->stk,  lhs + rhs);
})

DEF_CMD(SUB, 5, 0,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    StackPush(&field->stk,  lhs - rhs);
})

DEF_CMD(MUL, 6, 0,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    StackPush(&field->stk,  lhs * rhs);
})

DEF_CMD(DIV, 7, 0,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    ERROR_CHECK(DBLCMP(rhs, 0), DIVISION_BY_ZERO_ERROR);
    StackPush(&field->stk,  lhs / rhs);
})

DEF_CMD(SQRT, 9, 0,
{
    StackPop (&field->stk, &rhs);
    ERROR_CHECK(rhs < 0, INCORRECT_SQRT_VALUE_ERROR);
    StackPush(&field->stk, sqrt(rhs));
})

DEF_CMD(DUMP, 10, 0,
{
    DumpProcessor(field);
})

DEF_CMD(OUT, 11, 0,
{
    StackPop (&field->stk, &rhs);
    printf("out: %g\n", rhs);
})

DEF_CMD(IN, 12, 0,
{
    printf("Enter a number: ");
    scanf("%lf", &arg_val);
    StackPush(&field->stk, arg_val);
})

DEF_CMD(RAM, 13, 0,
{
    //print RAM
    for (int i = 0; i < (int)RAM_SIZE; i++)
    {
        if ((i > 0) && (i % 100 == 0))
            printf("\n");
        printf("%c ", (int)field->Ram[i]);
    }

    printf("\n");
})

DEF_CMD(RET, 14, 0,
{
    StackPop (&field->ret_adr, &rhs);
    field->pc = (int)rhs - 1;
})

DEF_CMD(RSIN, 15, 0,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    StackPush(&field->stk, (lhs * sin(rhs * PI / 180)));
})

DEF_CMD(RCOS, 17, 0,
{
    StackPop (&field->stk, &rhs);
    StackPop (&field->stk, &lhs);
    StackPush(&field->stk, (lhs * cos(rhs * PI / 180)));
})

/*//circ equation
DEF_CMD(CIRC, 18, 0,
{
    StackPop (&field->stk, &rhs);
    int R = rhs;
    for (double i = 0; i < PI * 2; i += 0.01)
        field->Ram[100 * (50 + (int)(R * sin(i))) + (50 - (int)(R * cos(i)))] = 64;
})*/