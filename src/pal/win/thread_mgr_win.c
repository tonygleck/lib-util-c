// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "lib-util-c/app_logging.h"
#include "lib-util-c/thread_mgr.h"

typedef struct THREAD_MGR_INFO_TAG
{
    HANDLE main_thread;
    THREAD_START_FUNC thread_func;
    void* thread_parameter;
} THREAD_MGR_INFO;

static DWORD thread_worker_func(void* parameter)
{
    THREAD_MGR_INFO* thread_mgr = (THREAD_MGR_INFO*)parameter;
    if (thread_mgr != NULL)
    {
        thread_mgr->thread_func(thread_mgr->thread_parameter);
    }
    return 0;
}

THREAD_MGR_HANDLE thread_mgr_init(THREAD_START_FUNC start_func, void* parameter)
{
    THREAD_MGR_INFO* result;
    if ((result = (THREAD_MGR_INFO*)malloc(sizeof(THREAD_MGR_INFO))) == NULL)
    {
        log_error("Failure allocating thread manager");
    }
    else
    {
        result->thread_func = start_func;
        result->thread_parameter = parameter;
        if ((result->main_thread = CreateThread(NULL, 0, thread_worker_func, result, 0, NULL)) == NULL)
        {
            log_error("Failure creating thread %d", GetLastError());
            free(result);
            result = NULL;
        }
    }
    return NULL;
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
        WaitForSingleObject(handle->main_thread, INFINITE);
        CloseHandle(handle->main_thread);
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
        CloseHandle(handle->main_thread);
        result = 0;
    }
    return result;
}

void thread_mgr_sleep(size_t milliseconds)
{
    Sleep((DWORD)milliseconds);
}

size_t thread_mgr_get_num_proc(void)
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return (size_t)sysinfo.dwNumberOfProcessors;
}