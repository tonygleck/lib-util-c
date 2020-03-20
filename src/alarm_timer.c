// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "lib-util-c/alarm_timer.h"
#include "lib-util-c/app_logging.h"

int alarm_timer_init(ALARM_TIMER_INFO* alarm_info)
{
    int result;
    if (alarm_info == NULL)
    {
        log_error("Alarm info is NULL");
        result = __LINE__;
    }
    else
    {
        alarm_info->expire_sec = 0;
        alarm_info->start_time = 0;
        result = 0;
    }
    return result;
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
        handle->expire_sec = expire_sec;
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
        double time_diff = difftime(curr_time, handle->start_time);
        result = ((size_t)time_diff > handle->expire_sec);
    }
    else
    {
        result = true;
    }
    return result;
}
