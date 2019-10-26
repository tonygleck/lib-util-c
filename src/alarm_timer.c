// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "lib-util-c/alarm_timer.h"
#include "lib-util-c/app_logging.h"

typedef struct ALARM_TIMER_INFO_TAG
{
    size_t expire_sec;
    time_t start_time;
} ALARM_TIMER_INFO;

ALARM_TIMER_HANDLE alarm_timer_create(void)
{
    ALARM_TIMER_INFO* result = (ALARM_TIMER_INFO*)malloc(sizeof(ALARM_TIMER_INFO));
    if (result == NULL)
    {
        log_error("Failure allocating timer info");
    }
    else
    {
        result->expire_sec = 0;
        result->start_time = time(NULL);
    }
    return result;
}

void alarm_timer_destroy(ALARM_TIMER_HANDLE handle)
{
    if (handle != NULL)
    {
        free(handle);
    }
}

int alarm_timer_start(ALARM_TIMER_HANDLE handle, size_t expire_sec)
{
    int result;
    if (handle == NULL)
    {
        log_error("Timer handle is NULL");
        result = __LINE__;
    }
    else
    {
        handle->expire_sec = expire_sec*1000;
        handle->start_time = time(NULL);
        result = 0;
    }
    return result;
}

void alarm_timer_reset(ALARM_TIMER_HANDLE handle)
{
    if (handle != NULL)
    {
        handle->start_time = time(NULL);
    }
}

bool alarm_timer_is_expired(ALARM_TIMER_HANDLE handle)
{
    bool result;
    if (handle != NULL)
    {
        time_t curr_time = time(NULL);
        double time_diff = difftime(handle->start_time, curr_time);
        result = ((size_t)time_diff > handle->expire_sec);
    }
    else
    {
        result = true;
    }
    return result;
}
