// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "lib-util-c/app_logging.h"
#include "lib-util-c/condition_mgr.h"
#include "lib-util-c/mutex_mgr.h"

static DWORD timespec_to_ms(const struct timespec* abstime)
{
    DWORD result;
    if (abstime == NULL)
    {
        result = INFINITE;
    }
    else
    {
        result = (DWORD)(((abstime->tv_sec - time(NULL)) * 1000) + (abstime->tv_nsec / 1000000));
        if (result < 0)
        {
            result = 1;
        }
    }
    return result;
}

static int start_timed_wait(SIGNAL_HANDLE handle, MUTEX_HANDLE mutex, const struct timespec* abstime)
{
    int result;
    if (SleepConditionVariableCS(&handle, mutex, timespec_to_ms(abstime)))
    {
        result = 0;
    }
    else
    {
        result = __LINE__;
    }
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
    else
    {
        InitializeConditionVariable(handle);
        result = 0;
    }
    return result;
}

void condition_mgr_deinit(SIGNAL_HANDLE handle)
{
    (void)handle;
    // You don't have to deinit the condition variable
}

int condition_mgr_signal(SIGNAL_HANDLE handle)
{
    WakeConditionVariable(&handle);
    return 0;
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
    WakeAllConditionVariable(&handle);
    return 0;
}
