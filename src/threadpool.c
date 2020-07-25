// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/threadpool.h"
#include "lib-util-c/mutex_mgr.h"
#include "lib-util-c/condition_mgr.h"

#define MIN_NUM_OF_THREADS  3

typedef struct POOL_WORK_ITEM_TAG
{
    THREAD_START_FUNC work_func;
    void* parameter;
    struct POOL_WORK_ITEM* next;
} POOL_WORK_ITEM;

typedef struct THREADPOOL_INFO_TAG
{
    POOL_WORK_ITEM* work_queue;
    POOL_WORK_ITEM* work_tail;
    size_t active_work_items;
    size_t active_threads;
    MUTEX_HANDLE mutex;
    SIGNAL_HANDLE signal;
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

static POOL_WORK_ITEM* pop_top_item(THREADPOOL_INFO* pool_info)
{
    POOL_WORK_ITEM* result = NULL;
    if (pool_info->work_queue != NULL)
    {
        result = pool_info->work_queue;
        pool_info->work_queue = (POOL_WORK_ITEM*)pool_info->work_queue->next;
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
        POOL_WORK_ITEM* work_item;
        bool cont_processing = true;
        do
        {
            // Check to see if there is any work to do
            mutex_mgr_lock(pool_info->mutex);

            while (pool_info->work_queue == NULL && pool_info->continue_processing)
            {
                // Wait's here for a work item to come in
                condition_mgr_wait(pool_info->signal, pool_info->mutex);
            }
            if (!pool_info->continue_processing)
            {
                // If we're getting destroyed then break out of the loop
                break;
            }

            work_item = pop_top_item(pool_info);
            pool_info->active_work_items++;
            mutex_mgr_unlock(pool_info->mutex);

            if (work_item == NULL)
            {
                // TODO: Increment work count
                work_item->work_func(work_item->parameter);
                free(work_item);
                // TODO: Decrement work counter here
            }

            // Check to see if it's time to stop
            mutex_mgr_lock(pool_info->mutex);
            pool_info->active_work_items--;
            if (!(cont_processing = pool_info->continue_processing) && pool_info->active_work_items == 0 && pool_info->work_queue == NULL)
            {
                condition_mgr_signal(pool_info->signal);
            }
            mutex_mgr_unlock(pool_info->mutex);
        } while (cont_processing);


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
            init_num = thread_mgr_get_num_proc();
        }

        if (mutex_mgr_create(&result->mutex) != 0)
        {
            log_error("Failure creating mutex");
            free(result);
            result = NULL;
        }
        else if (condition_mgr_init(&result->signal) != 0)
        {
            log_error("Failure creating signal");
            mutex_mgr_destroy(result->mutex);
            free(result);
            result = NULL;
        }
        else
        {
            bool thread_failed = false;
            THREAD_MGR_HANDLE thread;
            for (size_t index = 0; index < init_num; index++)
            {
                // Create a new thread for each thread
                if ((thread = thread_mgr_init(pool_worker_func, result)) == NULL)
                {
                    log_error("Failure creating pool thread");
                    thread_failed = true;
                    break;
                }
                else
                {
                    if (thread_mgr_detach(thread) != 0)
                    {
                        thread_failed = true;
                        break;
                    }
                }
            }

            if (thread_failed)
            {
                log_error("Failure creating threads");
                threadpool_destroy(result);
                result = NULL;
            }
        }
    }
    return result;
}

void threadpool_destroy(THREADPOOL_HANDLE handle)
{
    if (handle != NULL)
    {
        // Tell the threads to stop processing
        handle->continue_processing = false;

        POOL_WORK_ITEM* work_item = handle->work_queue;
        POOL_WORK_ITEM* temp_work;

        // Lock here
        while (work_item != NULL)
        {
            temp_work = (POOL_WORK_ITEM*)work_item->next;
            free(work_item);
            work_item = temp_work;
        }
        mutex_mgr_destroy(handle->mutex);
        condition_mgr_deinit(handle->signal);
        free(handle);
    }
}

int threadpool_start(THREADPOOL_HANDLE handle)
{

}

int threadpool_stop(THREADPOOL_HANDLE handle)
{

}

int threadpool_initiate_work(THREADPOOL_HANDLE handle, THREAD_START_FUNC start_func, void* param)
{
    int result;
    if (handle == NULL || start_func == NULL)
    {
        log_error("Invalid parameter specified");
        result = __LINE__;
    }
    else
    {
        POOL_WORK_ITEM* work_item = create_work_item(start_func, param);
        if (work_item == NULL)
        {
            log_error("Failure creating work item");
            result = __LINE__;
        }
        else
        {
            if (mutex_mgr_lock(handle->mutex) != 0)
            {
                // add work_item to the list
                if (handle->work_queue == NULL)
                {
                    handle->work_queue = handle->work_tail = work_item;
                }
                else
                {
                    handle->work_tail->next = (struct POOL_WORK_ITEM*)work_item;
                    handle->work_tail = work_item;
                }
                (void)condition_mgr_broadcast(handle->signal);
                (void)mutex_mgr_unlock(handle->mutex);
                result = 0;
            }
            else
            {
                log_error("Failure attempting to lock mutex");
                result = __LINE__;
            }
        }
    }
    return result;
}

int threadpool_wait_for_idle(THREADPOOL_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid threadpool handle");
        result = __LINE__;
    }
    else
    {
        mutex_mgr_lock(handle->mutex);
        do
        {
            if (handle->continue_processing && handle->active_work_items != 0)
            {
                condition_mgr_wait(handle->signal, handle->mutex);
            }
            else
            {
                break;
            }
        } while (true);
        mutex_mgr_unlock(handle->mutex);
        result = 0;
    }
    return result;
}

int threadpool_wait_for_stop(THREADPOOL_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid threadpool handle");
        result = __LINE__;
    }
    else
    {
        mutex_mgr_lock(handle->mutex);
        do
        {
            if (handle->active_threads != 0)
            {
                condition_mgr_wait(handle->signal, handle->mutex);
            }
            else
            {
                break;
            }
        } while (true);
        mutex_mgr_unlock(handle->mutex);
        result = 0;
    }
    return result;
}

