// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/thread_mgr.h"

typedef struct THREAD_MGR_INFO_TAG
{
    pthread_t main_thread;
    THREAD_START_FUNC thread_func;
    void* thread_parameter;
} THREAD_MGR_INFO;

static void* thread_worker_func(void* parameter)
{
    THREAD_MGR_INFO* thread_mgr = (THREAD_MGR_INFO*)parameter;
    if (thread_mgr != NULL)
    {
        (void)thread_mgr->thread_func(thread_mgr->thread_parameter);
    }
    return NULL;
}

THREAD_MGR_HANDLE thread_mgr_init(THREAD_START_FUNC start_func, void* parameter)
{
    THREAD_MGR_INFO* result;
    if (start_func == NULL)
    {
        log_error("Invalid parameter specified start_func: NULL;");
        result = NULL;
    }
    else if ((result = (THREAD_MGR_INFO*)malloc(sizeof(THREAD_MGR_INFO))) == NULL)
    {
        log_error("Failure allocating thread manager");
    }
    else
    {
        result->thread_func = start_func;
        result->thread_parameter = parameter;
        int thread_res = pthread_create(&result->main_thread, NULL, thread_worker_func, result);
        if (thread_res != 0)
        {
            log_error("Failure creating thread %d", thread_res);
            free(result);
            result = NULL;
        }
    }
    return result;
}

int thread_mgr_join(THREAD_MGR_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified handle: NULL");
        result = __LINE__;
    }
    else
    {
        void* res;
        (void)pthread_join(handle->main_thread, &res);
        free(handle);
        result = 0;
    }
    return result;
}

int thread_mgr_detach(THREAD_MGR_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified handle: NULL");
        result = __LINE__;
    }
    else
    {
        if (pthread_detach(handle->main_thread) == 0)
        {
            free(handle);
            result = 0;
        }
        else
        {
            log_error("Failure detaching from thread");
            result = __LINE__;
        }
    }
    return result;
}

void thread_mgr_sleep(size_t milliseconds)
{
    time_t seconds = milliseconds / 1000;
    long nsRemainder = (milliseconds % 1000) * 1000000;
    struct timespec timeToSleep = { seconds, nsRemainder };
    (void)nanosleep(&timeToSleep, NULL);
}
