// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>

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

time_t get_time(void)
{
    return time(NULL);
}

struct tm* get_time_value(void)
{
    time_t mark_time = time(NULL);
    return gmtime(&mark_time);
}
