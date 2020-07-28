// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>

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
        atomic_fetch_add_explicit(value, 1, memory_order_relaxed);
        result = *value;
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
        atomic_fetch_add_explicit(value, 1, memory_order_relaxed);
        result = *value;
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
        atomic_fetch_sub_explicit(value, 1, memory_order_relaxed);
        result = *value;
    }
    return result;
}

long atomic_decrement64(int64_t* value)
{
    int64_t result;
    if (value == NULL)
    {
        result = 0;
    }
    else
    {
        atomic_fetch_sub_explicit(value, 1, memory_order_relaxed);
        result = *value;
    }
    return result;
}

long atomic_add(long* operand, long value)
{
    if (operand == NULL)
    {
        return 0;
    }
    else
    {
        atomic_fetch_add_explicit(operand, value, memory_order_relaxed);
        return *operand;
    }
}

long atomic_subtract(long* operand, long value)
{
    if (operand == NULL)
    {
        return 0;
    }
    else
    {
        atomic_fetch_sub_explicit(operand, value, memory_order_relaxed);
        return *operand;
    }
}

