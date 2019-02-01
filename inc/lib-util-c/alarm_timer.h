#ifndef TIMER_OBJECT_H
#define TIMER_OBJECT_H

#include <stdlib.h>
#include "umock_c_prod.h"

typedef struct ALARM_TIMER_INFO_TAG* ALARM_TIMER_HANDLE;

MOCKABLE_FUNCTION(, ALARM_TIMER_HANDLE, alarm_timer_create);
MOCKABLE_FUNCTION(, void, alarm_timer_destroy, ALARM_TIMER_HANDLE, handle);
MOCKABLE_FUNCTION(, int, alarm_timer_start, ALARM_TIMER_HANDLE, handle, size_t, expire_sec);
MOCKABLE_FUNCTION(, void, alarm_timer_reset, ALARM_TIMER_HANDLE, handle);
MOCKABLE_FUNCTION(, bool, alarm_timer_is_expired, ALARM_TIMER_HANDLE, handle);

#endif // SCHEDULER
