#ifndef ATOMTRACE_CL_UINT_STACK_HCL_
#define ATOMTRACE_CL_UINT_STACK_HCL_

#define MAX_STACK_SIZE 64

typedef struct UintStack
{
    uint m_data[MAX_STACK_SIZE];
    uint topIndex;
}UintStack;

// Constructor
void UintStackInit(UintStack* st)
{
    st->topIndex = 0;
}

// UintStack operation functions
bool UintStackPush(UintStack* st, uint v)
{
    uint top = st->topIndex;
    if (top >= MAX_STACK_SIZE)
    {
        return false;
    }
    else
    {
        st->m_data[top] = v;
        top++;
        st->topIndex = top;
    }
    return true;
}

bool UintStackPop(UintStack* st)
{
    uint top = st->topIndex;
    if (top == 0)
    {
        return false;
    }
    top--;
    st->m_data[top] = 0;
    st->topIndex = top;
    return true;
}

uint UintStackTop(const UintStack* st)
{
    return st->m_data[st->topIndex - 1];
}

bool UintStackIsEmpty(const UintStack* st)
{
    return (0 == st->topIndex);
}

bool UintStackClear(UintStack* st)
{
    st->topIndex = 0;
    st->m_data[0] = 0;
    return true;
}

#endif // ATOMTRACE_CL_UINT_STACK_HCL_
