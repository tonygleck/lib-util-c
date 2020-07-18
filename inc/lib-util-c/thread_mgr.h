// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#endif

#include "umock_c/umock_c_prod.h"

typedef struct THREAD_MGR_INFO_TAG* THREAD_MGR_HANDLE;

typedef void(*THREAD_START_FUNC)(void*);

MOCKABLE_FUNCTION(, THREAD_MGR_HANDLE, thread_mgr_init, THREAD_START_FUNC, start_func, void*, parameter);
MOCKABLE_FUNCTION(, int, thread_mgr_join, THREAD_MGR_HANDLE, handle);
MOCKABLE_FUNCTION(, int, thread_mgr_detach, THREAD_MGR_HANDLE, handle);

MOCKABLE_FUNCTION(, void, thread_mgr_sleep, size_t, ms);
MOCKABLE_FUNCTION(, size_t, thread_mgr_get_num_proc);

#ifdef __cplusplus
}
#endif
