#ifndef TIMER_OBJECT_H
#define TIMER_OBJECT_H

#include <stdlib.h>

typedef struct ALARM_TIMER_INFO_TAG* ALARM_TIMER_HANDLE;

extern ALARM_TIMER_HANDLE alarm_timer_create(size_t expire_sec);
extern bool alarm_timer_is_expired(ALARM_TIMER_HANDLE handle);
extern void alarm_timer_destroy(ALARM_TIMER_HANDLE handle);

#endif // SCHEDULER
