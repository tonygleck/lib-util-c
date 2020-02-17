// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lib-util-c/crt_extensions.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/sys_debug_shim.h"

int clone_string(char** target, const char* source)
{
    int result;
    if (target == NULL || source == NULL)
    {
        log_error("Invalid parameter specified target: %p, source: %p", target, source);
        result = __LINE__;
    }
    else
    {
        size_t length = strlen(source);
        if ((*target = malloc(length+1)) == NULL)
        {
            log_error("Failure allocating target");
            result = __LINE__;
        }
        else
        {
            memset(*target, 0, length+1);
            memcpy(*target, source, length);
            result = 0;
        }
    }
    return result;
}

int clone_string_with_size(char** target, const char* source, size_t source_len)
{
    int result;
    if (target == NULL || source == NULL || source_len == 0)
    {
        log_error("Invalid parameter specified target: %p, source: %p", target, source);
        result = __LINE__;
    }
    else
    {
        if ((*target = malloc(source_len+1)) == NULL)
        {
            log_error("Failure allocating target");
            result = __LINE__;
        }
        else
        {
            memset(*target, 0, source_len+1);
            memcpy(*target, source, source_len);
            result = 0;
        }
    }
    return result;
}

int clone_string_with_format(char** target, const char* format, ...)
{
    int result;
    if (target == NULL || format == NULL)
    {
        log_error("Invalid parameter specified target: %p, format: %p", target, format);
        result = __LINE__;
    }
    else
    {
        size_t maxBufSize = 0;
        char* buf = NULL;

        va_list arg_list;
        int length;
        va_start(arg_list, format);

        length = vsnprintf(buf, maxBufSize, format, arg_list);
        va_end(arg_list);
        if (length > 0)
        {
            if ((*target = (char*)malloc(length+1)) != NULL)
            {
                va_start(arg_list, format);
                if (vsnprintf(*target, length+1, format, arg_list) < 0)
                {
                    free(*target);
                    log_error("Failure: vsnprintf formatting failed.");
                    result = __LINE__;
                }
                else
                {
                    result = 0;
                }
                va_end(arg_list);
            }
            else
            {
                log_error("Failure: allocation failed.");
                result = __LINE__;
            }
        }
        else
        {
            log_error("Failure no format value specified");
            result = __LINE__;
        }
    }
    return result;
}

time_t get_time(void)
{
    return time(NULL);
}

struct tm* get_time_value(void)
{
    time_t mark_time = time(NULL);
    return gmtime(&mark_time);
}
