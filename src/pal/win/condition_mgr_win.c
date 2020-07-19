// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>

#include "lib-util-c/app_logging.h"
#include "lib-util-c/condition_mgr.h"
#include "lib-util-c/mutex_mgr.h"

static int start_timed_wait(SIGNAL_HANDLE handle, MUTEX_HANDLE mutex, const struct timespec* abstime)
{
    int result;
    return result;
}

int condition_mgr_init(SIGNAL_HANDLE* handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid Parameter specified");
        result = __LINE__;
    }
    else if (pthread_cond_init(handle, NULL) != 0)
    {
        log_error("Failure create mutex object");
        result = __LINE__;
    }
    else
    {
        result = 0;
    }
    return result;
}

void condition_mgr_deinit(SIGNAL_HANDLE handle)
{
    (void)pthread_cond_destroy(&handle);
}

int condition_mgr_signal(SIGNAL_HANDLE handle)
{
    int result;
    if (pthread_cond_signal(&handle) == 0)
    {
        result = 0;
    }
    else
    {
        result = __LINE__;
    }
    return result;
}

int condition_mgr_wait(SIGNAL_HANDLE handle, MUTEX_HANDLE mutex)
{
    return start_timed_wait(handle, mutex, NULL);
}

int condition_mgr_timed_wait(SIGNAL_HANDLE handle, MUTEX_HANDLE mutex,
        const struct timespec* abstime)
{
    return start_timed_wait(handle, mutex, abstime);
}

int condition_mgr_broadcast(SIGNAL_HANDLE handle)
{
    int result;
    if (pthread_cond_broadcast(&handle) == 0)
    {
        result = 0;
    }
    else
    {
        result = __LINE__;
    }
    return result;
}
