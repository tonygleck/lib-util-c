// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "lib-util-c/app_logging.h"

#include "thread_wrapper.h"

static DWORD timespec_to_ms(const struct timespec* abstime)
{
    DWORD result;

    if (abstime == NULL)
    {
        result = INFINITE;
    }
    else
    {
        if ((result = ((abstime->tv_sec - time(NULL)) * 1000) + (abstime->tv_nsec / 1000000)) < 0)
        {
            result = 1;
        }
    }
    return result;
}

int pthread_create(pthread_t* thread, pthread_attr_t* attr, void *(*start_routine)(void *), void *arg)
{
    (void)attr;
    int result;
    if (thread == NULL || start_routine == NULL)
    {
        result = __LINE__;
    }
    else
    {
        if ((*thread = CreateThread(NULL, 0, start_routine, arg, 0, NULL)) == NULL)
        {
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

int nanosleep(const struct timespec* req, struct timespec *rem);
{
    Sleep(timespec_to_ms(req));
}
