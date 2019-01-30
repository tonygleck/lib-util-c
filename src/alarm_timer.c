#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "alarm_timer.h"

typedef struct ALARM_TIMER_INFO_TAG
{
    size_t expire_sec;
    time_t start_time;
} ALARM_TIMER_INFO;

ALARM_TIMER_HANDLE alarm_timer_create(size_t expire_sec)
{
    ALARM_TIMER_INFO* result = (ALARM_TIMER_INFO*)malloc(sizeof(ALARM_TIMER_INFO));
    if (result == NULL)
    {
        printf("Failure allocating timer info");
    }
    else
    {
        result->expire_sec = expire_sec*1000;
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

bool alarm_timer_is_expired(ALARM_TIMER_HANDLE handle)
{
    bool result;
    if (handle != NULL)
    {
        time_t curr_time = time(NULL);
        double time_diff = difftime(handle->start_time, curr_time);
        printf("time_diff: %u > %u\r\n", (unsigned int)time_diff, (unsigned int)handle->expire_sec);
        result = ((size_t)time_diff > handle->expire_sec);
    }
    else
    {
        result = true;
    }
    return result;
}