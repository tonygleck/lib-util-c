// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#endif

#include "umock_c/umock_c_prod.h"
#include "lib-util-c/thread_mgr.h"

typedef struct THREADPOOL_INFO_TAG* THREADPOOL_HANDLE;

MOCKABLE_FUNCTION(, THREADPOOL_HANDLE, threadpool_create, size_t, init_num);
MOCKABLE_FUNCTION(, void, threadpool_destroy, THREADPOOL_HANDLE, handle);
MOCKABLE_FUNCTION(, int, threadpool_start, THREADPOOL_HANDLE, handle);
MOCKABLE_FUNCTION(, int, threadpool_stop, THREADPOOL_HANDLE, handle);
MOCKABLE_FUNCTION(, int, threadpool_initiate_work, THREADPOOL_HANDLE, handle, THREAD_START_FUNC, start_func, void*, param);
MOCKABLE_FUNCTION(, int, threadpool_wait_for_idle, THREADPOOL_HANDLE, handle);
MOCKABLE_FUNCTION(, int, threadpool_wait_for_stop, THREADPOOL_HANDLE, handle);
MOCKABLE_FUNCTION(, size_t, threadpool_get_pool_num, THREADPOOL_HANDLE, handle);

#ifdef __cplusplus
}
#endif
