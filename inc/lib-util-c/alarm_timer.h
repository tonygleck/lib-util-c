// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

typedef struct ALARM_TIMER_INFO_TAG
{
    size_t expire_sec;
    time_t start_time;
} ALARM_TIMER_INFO;

typedef struct ALARM_TIMER_INFO_TAG* ALARM_TIMER_HANDLE;

MOCKABLE_FUNCTION(, int, alarm_timer_init, ALARM_TIMER_INFO*, alarm_info);
MOCKABLE_FUNCTION(, int, alarm_timer_start, ALARM_TIMER_HANDLE, handle, size_t, expire_sec);
MOCKABLE_FUNCTION(, void, alarm_timer_reset, ALARM_TIMER_HANDLE, handle);
MOCKABLE_FUNCTION(, bool, alarm_timer_is_expired, ALARM_TIMER_HANDLE, handle);

#ifdef __cplusplus
}
#endif
