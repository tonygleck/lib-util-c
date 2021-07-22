// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

typedef void(*THREADPOOL_ROUTINE)(void*);

/*typedef struct THREADPOOL_WORK_ITEM_TAG
{
    THREADPOOL_ROUTINE routine;
    void* arg;
    struct THREADPOOL_WORK_ITEM_TAG* next;
} THREADPOOL_WORK_ITEM;*/

typedef struct THREADPOOL_WORK_INFO_TAG* THREADPOOL_HANDLE;

MOCKABLE_FUNCTION(, THREADPOOL_HANDLE, threadpool_create, size_t, thread_num);
MOCKABLE_FUNCTION(, void, threadpool_destroy, THREADPOOL_HANDLE, handle);

MOCKABLE_FUNCTION(, int, threadpool_add_workitem, THREADPOOL_HANDLE, handle, THREADPOOL_ROUTINE, routine, void*, user_ctx);

#ifdef __cplusplus
}
#endif
