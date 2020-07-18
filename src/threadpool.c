// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/threadpool.h"
#include "lib-util-c/mutex_mgr.h"

#define MIN_NUM_OF_THREADS  3

typedef struct POOL_WORK_ITEM_TAG
{
    THREAD_START_FUNC work_func;
    void* parameter;
    struct POOL_WORK_ITEM* next;
} POOL_WORK_ITEM;

typedef struct THREADPOOL_INFO_TAG
{
    POOL_WORK_ITEM* work_list;
    size_t thread_num;
    MUTEX_HANDLE mutex;
    bool continue_processing;
} THREADPOOL_INFO;

static POOL_WORK_ITEM* create_work_item(THREAD_START_FUNC start_func, void* param)
{
    POOL_WORK_ITEM* result;

    if ((result = (POOL_WORK_ITEM*)malloc(sizeof(POOL_WORK_ITEM))) == NULL)
    {
        log_error("Failure allocating threadpool work item");
    }
    else
    {
        result->work_func = start_func;
        result->parameter = param;
        result->next = NULL;
    }
    return result;
}

static void pool_worker_func(void* param)
{
    THREADPOOL_INFO* pool_info = (THREADPOOL_INFO*)param;
    if (pool_info == NULL)
    {
        log_error("pool function specified NULL parameter");
    }
    else
    {
        bool cont_proc;
        POOL_WORK_ITEM* result;

        do
        {
            // Check to see if there is any work to do
            mutex_mgr_lock(pool_info->mutex);

            // TODO: Protect this
            cont_proc = pool_info->continue_processing;

            mutex_mgr_unlock(pool_info->mutex);

            // Check to see if it's time to stop
            mutex_mgr_lock(pool_info->mutex);

            mutex_mgr_unlock(pool_info->mutex);
        } while (1);

    }
}

THREADPOOL_HANDLE threadpool_create(size_t init_num)
{
    THREADPOOL_INFO* result;
    if ((result = (THREADPOOL_INFO*)malloc(sizeof(THREADPOOL_INFO))) == NULL)
    {
        log_error("Failure allocating threadpool info");
    }
    else
    {
        memset(result, 0, sizeof(THREADPOOL_INFO) );
        result->continue_processing = true;
        if (init_num < MIN_NUM_OF_THREADS)
        {
            result->thread_num = thread_mgr_get_num_proc();
        }
        else
        {
            result->thread_num = init_num;
        }

        if (mutex_mgr_create(&result->mutex) != 0)
        {
            log_error("Failure creating mutex");
            free(result);
            result = NULL;
        }
        else
        {
            for (size_t index = 0; index < result->thread_num; index++)
            {
                // Create a new thread for each thread
                THREAD_MGR_HANDLE thread = thread_mgr_init(pool_worker_func, result);
                if (thread_mgr_detach(thread) != 0)
                {
                    break;
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
        POOL_WORK_ITEM* work_item = handle->work_list;
        POOL_WORK_ITEM* temp_work;

        // Lock here
        while (work_item != NULL)
        {
            temp_work = work_item->next;
            free(work_item);
            work_item = temp_work;
        }
        free(handle);
    }
}

int threadpool_initiate_work(THREADPOOL_HANDLE handle, THREAD_START_FUNC start_func, void* param)
{
    (void)param;
    int result;
    if (handle == NULL || start_func == NULL)
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
    }
    return result;
}

int threadpool_wait(THREADPOOL_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
    }
    else
    {
        result = 0;
    }
    return result;
}

