// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>

#include "lib-util-c/atomic_operations.h"

long atomic_increment(long* value)
{
    long result;
    if (value == NULL)
    {
        result = 0;
    }
    else
    {
        result = InterlockedIncrement(value);
    }
    return result;
}

int64_t atomic_increment64(int64_t* value)
{
    int64_t result;
    if (value == NULL)
    {
        result = 0;
    }
    else
    {
        result = InterlockedIncrement64(value);
    }
    return result;
}

long atomic_decrement(long* value)
{
    long result;
    if (value == NULL)
    {
        result = 0;
    }
    else
    {
        result = InterlockedDecrement(value);
    }
    return result;
}

int64_t atomic_decrement64(int64_t* value)
{
    int64_t result;
    if (value == NULL)
    {
        result = 0;
    }
    else
    {
        result = InterlockedDecrement64(value);
    }
    return result;
}

long atomic_add(long* operand, long value)
{
    long result;
    if (operand == NULL)
    {
        result = 0;
    }
    else
    {
        result = InterlockedAdd(operand, value);
    }
    return result;
}

long atomic_subtract(long* operand, long value)
{
    long result;
    if (operand == NULL)
    {
        result = 0;
    }
    else
    {
        result = InterlockedAdd(operand, (value*-1));
    }
    return result;
}

