// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>

#include "lib-util-c/app_logging.h"
#include "lib-util-c/mutex_mgr.h"

int mutex_mgr_create(MUTEX_HANDLE* handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified");
        result = __LINE__;
    }
    else if (pthread_mutex_init(*handle, NULL) != 0)
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

int mutex_mgr_destroy(MUTEX_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified");
        result = __LINE__;
    }
    else
    {
        (void)pthread_mutex_destroy(handle);
        result = 0;
    }
    return result;
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
        if (pthread_mutex_lock(handle) == 0)
        {
            result = 0;
        }
        else
        {
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
        if (pthread_mutex_unlock(handle) == 0)
        {
            result = 0;
        }
        else
        {
            result = __LINE__;
        }
    }
    return result;
}
