// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

typedef struct INTERVAL_TIMER_INFO_TAG* INTERVAL_TIMER_HANDLE;

typedef int(*ON_INTERVAL_TRIGGER_CALLBACK)(void* context);

MOCKABLE_FUNCTION(, INTERVAL_TIMER_HANDLE, interval_timer_create);
MOCKABLE_FUNCTION(, void, interval_timer_destroy, INTERVAL_TIMER_HANDLE, handle);
MOCKABLE_FUNCTION(, int, interval_timer_set_trigger, INTERVAL_TIMER_HANDLE, handle, size_t, trigger_sec, ON_INTERVAL_TRIGGER_CALLBACK, trigger_cb, void*, user_ctx);
MOCKABLE_FUNCTION(, void, interval_timer_stop_trigger, INTERVAL_TIMER_HANDLE, handle);

#ifdef __cplusplus
}
#endif
