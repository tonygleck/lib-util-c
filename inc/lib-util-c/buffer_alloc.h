// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
#include <cstdio>
extern "C" {
#else
#include <stdio.h>
#endif

#include "umock_c/umock_c_prod.h"

typedef struct STRING_BUFFER_TAG
{
    char* payload;
    size_t alloc_size;
    size_t default_alloc;
} STRING_BUFFER;

typedef struct BYTE_BUFFER_TAG
{
    unsigned char* payload;
    size_t alloc_size;
    size_t default_alloc;
    size_t payload_size;
} BYTE_BUFFER;

MOCKABLE_FUNCTION(, int, string_buffer_construct, STRING_BUFFER*, buffer, const char*, value);
MOCKABLE_FUNCTION(, void, string_buffer_free, STRING_BUFFER*, buffer);

int string_buffer_construct_sprintf(STRING_BUFFER* buffer, const char* format, ...);

MOCKABLE_FUNCTION(, int, byte_buffer_construct, BYTE_BUFFER*, buffer, const unsigned char*, payload, size_t, length);
MOCKABLE_FUNCTION(, void, byte_buffer_free, BYTE_BUFFER*, buffer);

#ifdef __cplusplus
}
#endif
