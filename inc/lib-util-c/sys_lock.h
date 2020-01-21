// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

typedef struct SYS_LOCK_INFO_TAG* SYS_LOCK_HANDLE;

MOCKABLE_FUNCTION(, SYS_LOCK_HANDLE, sys_lock_create);
MOCKABLE_FUNCTION(, void, sys_lock_destroy, SYS_LOCK_HANDLE, handle);
MOCKABLE_FUNCTION(, int, sys_lock_lock, SYS_LOCK_HANDLE, handle);
MOCKABLE_FUNCTION(, void, sys_lock_unlock, SYS_LOCK_HANDLE, handle);

#ifdef __cplusplus
}
#endif
