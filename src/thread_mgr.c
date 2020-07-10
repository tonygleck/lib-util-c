// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "lib-util-c/app_logging.h"
#include "lib-util-c/thread_mgr.h"

#include "thread_wrapper.h"

typedef struct THREAD_MGR_INFO_TAG
{
    int j;
} THREAD_MGR_INFO;

THREAD_MGR_HANDLE thread_mgr_init(void)
{
    return NULL;
}

void thread_mgr_sleep(size_t milliseconds)
{
    time_t seconds = milliseconds / 1000;
    long nsRemainder = (milliseconds % 1000) * 1000000;
    struct timespec timeToSleep = { seconds, nsRemainder };
    (void)nanosleep(&timeToSleep, NULL);
}
