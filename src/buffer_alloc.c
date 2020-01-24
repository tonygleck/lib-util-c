// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/buffer_alloc.h"

#define DEFAULT_BUFFER_ALLOC_SIZE       64

typedef struct GENERIC_BUFFER_TAG
{
    void* payload;
    size_t alloc_size;
    size_t default_alloc;
} GENERIC_BUFFER;

static int allocate_buffer(GENERIC_BUFFER* buffer, size_t new_length, bool* reallocated)
{
    int result;
    if (buffer->alloc_size == 0)
    {
        // Are we allocating more than the default alloc
        if (new_length > buffer->default_alloc)
        {
            if (buffer->default_alloc < DEFAULT_BUFFER_ALLOC_SIZE)
            {
                buffer->default_alloc = DEFAULT_BUFFER_ALLOC_SIZE;
            }
            else
            {
                buffer->default_alloc = new_length;
            }
        }
        buffer->alloc_size = new_length+buffer->default_alloc;

        if ((buffer->payload = malloc(buffer->alloc_size+1)) == NULL)
        {
            log_error("Failure allocating buffer value");
            result = __LINE__;
        }
        else
        {
            if (reallocated != NULL)
            {
                *reallocated = false;
            }
            memset(buffer->payload, 0, buffer->alloc_size+1);
            result = 0;
        }
    }
    else
    {
        // Do we need to realloc
        size_t curr_len = strlen(buffer->payload);
        if (curr_len + new_length + 1 >= buffer->alloc_size)
        {
            char* temp_payload;
            size_t alloc_len = buffer->default_alloc + buffer->alloc_size + new_length + 1;
            // Realloc the string
            if ((temp_payload = realloc(buffer->payload, alloc_len)) == NULL)
            {
                log_error("Failure reallocating buffer value");
                result = __LINE__;
            }
            else
            {
                buffer->payload = temp_payload;
                if (reallocated != NULL)
                {
                    *reallocated = true;
                }
                buffer->alloc_size = alloc_len;
                memset(buffer->payload+curr_len, 0, alloc_len-curr_len);
                result = 0;
            }
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

int string_buffer_construct(STRING_BUFFER* buffer, const char* value)
{
    int result;
    if (buffer == NULL || value == NULL)
    {
        log_error("Invalid parameter specified buffer: %p, value: %p", buffer, value);
        result = __LINE__;
    }
    else
    {
        bool reallocated;
        size_t new_length = strlen(value);
        if (allocate_buffer((GENERIC_BUFFER*)buffer, new_length, &reallocated) != 0)
        {
            log_error("Failure allocating string buffer value");
            result = __LINE__;
        }
        else
        {
            size_t current_len = 0;
            if (buffer->payload != NULL)
            {
                current_len = strlen(buffer->payload);
            }
            memcpy(buffer->payload+current_len, value, new_length);
            result = 0;
        }
    }
    return result;
}

int string_buffer_construct_sprintf(STRING_BUFFER* buffer, const char* format, ...)
{
    int result;
    if (buffer == NULL || format == NULL)
    {
        log_error("invalid parameter value buffer: %p, format: %p", buffer, format);
        result = __LINE__;
    }
    else
    {
        va_list arg_list;
        int length;
        va_start(arg_list, format);

        /* Codes_SRS_STRING_07_041: [STRING_construct_sprintf shall determine the size of the resulting string and allocate the necessary memory.] */
        length = vsnprintf(NULL, 0, format, arg_list);
        va_end(arg_list);
        if (length > 0)
        {
            bool reallocated;
            if (allocate_buffer((GENERIC_BUFFER*)buffer, length, &reallocated) != 0)
            {
                log_error("Failure allocating string buffer");
                result = __LINE__;
            }
            else
            {
                size_t current_len = 0;
                if (buffer->payload != NULL)
                {
                    current_len = strlen(buffer->payload);
                }
                va_start(arg_list, format);
                if (vsnprintf(buffer->payload+current_len, length+1, format, arg_list) < 0)
                {
                    if (!reallocated)
                    {
                        free(buffer->payload);
                    }
                    log_error("Failure formatting string value");
                    result = __LINE__;
                }
                else
                {
                    result = 0;
                }
            }
        }
        else if (length == 0)
        {
            result = 0;
        }
        else
        {
            log_error("Failure vsnprintf returned negative length");
            result = __LINE__;
        }
    }
    return result;
}

int byte_buffer_construct(BYTE_BUFFER* buffer, const unsigned char* payload, size_t length)
{
    int result;
    if (buffer == NULL || payload == NULL || length == 0)
    {
        log_error("invalid parameter value buffer: %p, payload: %p, length: %d", buffer, payload, (int)length);
        result = __LINE__;
    }
    else
    {
        if (allocate_buffer((GENERIC_BUFFER*)buffer, length, NULL) != 0)
        {
            log_error("Failure allocating binary buffer");
            result = __LINE__;
        }
        else
        {
            memcpy(buffer->payload+buffer->payload_size, payload, length);
            buffer->payload_size += length;
            result = 0;
        }
    }
    return result;
}
