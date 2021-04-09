// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "lib-util-c/app_logging.h"
#include "lib-util-c/mutex_mgr.h"
#include "lib-util-c/sys_debug_shim.h"

int mutex_mgr_create(MUTEX_HANDLE* handle)
{
    int result;
    HANDLE test;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified");
        result = __LINE__;
    }
    else if ((test = CreateMutex(NULL, TRUE, NULL)) == NULL)
    {
        log_error("Failure create mutex object");
        result = __LINE__;
    }
    else
    {
        *handle = test;
        result = 0;
    }
    return result;
}

void mutex_mgr_destroy(MUTEX_HANDLE handle)
{
    if (handle != NULL)
    {
        CloseHandle(handle);
    }
}

int mutex_mgr_lock(MUTEX_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified");
        result = __LINE__;
    }
    else
    {
        DWORD thread_res = WaitForSingleObject(handle, INFINITE);
        if (thread_res == WAIT_OBJECT_0)
        {
            result = 0;
        }
        else
        {
            log_error("failure waiting for handle");
            result = __LINE__;
        }
    }
    return result;
}

int mutex_mgr_trylock(MUTEX_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified");
        result = __LINE__;
    }
    else
    {
        DWORD thread_res = WaitForSingleObject(handle, 0);
        if (thread_res == WAIT_OBJECT_0)
        {
            result = 0;
        }
        else
        {
            log_error("failure waiting for handle");
            result = __LINE__;
        }
    }
    return result;
}

int mutex_mgr_unlock(MUTEX_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified");
        result = __LINE__;
    }
    else
    {
        if (!ReleaseMutex(handle))
        {
            log_error("failure releasing mutex");
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}
