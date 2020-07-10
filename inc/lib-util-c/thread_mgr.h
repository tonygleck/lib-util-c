// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

typedef int(*THREAD_WORK_FUNC)(void*);

typedef struct THREAD_MGR_INFO_TAG* THREAD_MGR_HANDLE;

MOCKABLE_FUNCTION(, THREAD_MGR_HANDLE, thread_mgr_init);
MOCKABLE_FUNCTION(, void, thread_mgr_sleep, size_t, ms);


#ifdef __cplusplus
}
#endif
