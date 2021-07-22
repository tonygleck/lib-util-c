// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/threadpool.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/thread_mgr.h"

typedef struct THREADPOOL_WORK_INFO_TAG
{
    size_t thread_count;
    THREAD_MGR_HANDLE* thread_list;
} THREADPOOL_WORK_INFO;

typedef struct THREADPOOL_WORK_ITEM_TAG
{
    THREADPOOL_ROUTINE routine;
    void* user_arg;
} THREADPOOL_WORK_ITEM;

int threadpool_worker(void* arg)
{
    (void)arg;
    return 0;
}

THREADPOOL_HANDLE threadpool_create(size_t thread_count)
{
    THREADPOOL_WORK_INFO* result;
    if (thread_count == 0)
    {
        log_error("Invalid parameter thread_count: 0");
        result = NULL;
    }
    else
    {
        if ((result = (THREADPOOL_WORK_INFO*)malloc(sizeof(THREADPOOL_WORK_INFO))) == NULL)
        {
            log_error("Failure allocating threadpool items");
            result = NULL;
        }
        else if ((result->thread_list = malloc(sizeof(THREAD_MGR_HANDLE)*thread_count)) == NULL)
        {
            log_error("Failure allocating thread list");
            free(result);
            result = NULL;
        }
        else
        {
            result->thread_count = thread_count;
            for (size_t index = 0; index < result->thread_count; index++)
            {
                if ((result->thread_list[index] = thread_mgr_init(threadpool_worker, result)) == NULL)
                {
                    break;
                }
                else
                {
                    // remove all
                }
            }
        }
    }
    return result;
}

void threadpool_destroy(THREADPOOL_HANDLE handle)
{
    if (handle != NULL)
    {
        // Signal for threads to finish

        // Wait for all threads to finish
        for (size_t index = 0; index < handle->thread_count; index++)
        {
            thread_mgr_terminate_wait(handle->thread_list[index]);
        }

        free(handle->thread_list);
        free(handle);
    }
}

int threadpool_add_workitem(THREADPOOL_HANDLE handle, THREADPOOL_ROUTINE routine, void* user_ctx)
{
    int result;
    if (handle == NULL || routine == NULL)
    {
        log_error("Invalid parameter handle: %p, routine: %p", handle, routine);
        result = __LINE__;
    }
    else
    {
        // Lock here
        (void)user_ctx;
        result = 0;
    }
    return result;
}
