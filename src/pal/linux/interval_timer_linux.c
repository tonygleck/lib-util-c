// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/interval_timer.h"
#include "lib-util-c/app_logging.h"

typedef struct INTERVAL_TIMER_INFO_TAG
{
    timer_t timerid;
    ON_INTERVAL_TRIGGER_CALLBACK trigger_cb;
    void* user_ctx;
} INTERVAL_TIMER_INFO;

static void trigger_sig_handler(union sigval val)
{
    INTERVAL_TIMER_INFO* interval_info = (INTERVAL_TIMER_INFO*)val;
    if (interval_info == NULL)
    {
        log_error("Invalid parameter for INTERVAL_TIMER_INFO");
    }
    else
    {
        interval_info->trigger_cb(interval_info->user_ctx);
    }
}

INTERVAL_TIMER_HANDLE interval_timer_create(void)
{
    INTERVAL_TIMER_INFO* result = (INTERVAL_TIMER_INFO*)malloc(sizeof(INTERVAL_TIMER_INFO));
    if (result == NULL)
    {
        log_error("Failure allocating timer info");
    }
    else
    {
        result->timerid = 0;
    }
    return result;
}

void interval_timer_destroy(INTERVAL_TIMER_HANDLE handle)
{
    if (handle != NULL)
    {
        if (handle->timerid > 0)
        {
            timer_delete(handle->timerid);
        }
        free(handle);
    }
}

int interval_timer_set_trigger(INTERVAL_TIMER_HANDLE handle, size_t trigger_sec, ON_INTERVAL_TRIGGER_CALLBACK trigger_cb, void* user_ctx)
{
    int result;
    if (handle == NULL || trigger_cb == NULL || trigger_sec == 0)
    {
        log_error("Timer handle is NULL");
        result = __LINE__;
    }
    else
    {
        handle->trigger_cb = trigger_cb;
        handle->user_ctx = user_ctx;
        handle->timerid = 11;

        struct sigevent sig;
        sig.sigev_notify = SIGEV_THREAD;
        sig.sigev_notify_function = trigger_sig_handler;
        sig.sigev_value.sival_ptr = handle;
        sig.sigev_notify_attributes = NULL;
        if (timer_create(CLOCK_REALTIME, &sig, &handle->timerid) != 0)
        {
            log_error("Failure creating timer object");
            result = __LINE__;
        }
        else
        {
            struct itimerspec tm_spec, out;
            tm_spec.it_value.tv_sec = 1;
            tm_spec.it_value.tv_nsec = 0;
            tm_spec.it_interval.tv_sec = 1;
            tm_spec.it_interval.tv_nsec = 0;
            //issue the periodic timer request here.
            if (timer_settime(handle->timerid, 0, &tm_spec, &out) != 0)
            {
                log_error("failure setting timer: %d", errno);
            }
            else
            {
                result = 0;
            }
        }
    }
    return result;
}

void interval_timer_stop_trigger(INTERVAL_TIMER_HANDLE handle)
{
    if (handle != NULL)
    {
        timer_delete(handle->timerid);
        handle->timerid = 0;
    }
}
