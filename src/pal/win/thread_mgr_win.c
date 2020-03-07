// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "lib-util-c/alarm_timer.h"
#include "lib-util-c/app_logging.h"

static void thread_mgr_sleep(unsigned int milliseconds)
{
    Sleep(milliseconds);
}
