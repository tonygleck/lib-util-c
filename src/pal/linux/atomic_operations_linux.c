// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"

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

uint64_t atomic_increment64(uint64_t* value)
{
    uint64_t result;
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

long atomic_decrement64(uint64_t* value)
{
    uint64_t result;
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

