// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>

#include "lib-util-c/app_logging.h"
#include "lib-util-c/mutex_mgr.h"

int mutex_mgr_create(MUTEX_HANDLE handle)
{
    MUTEX_MGR_INFO* result;
    if ((result = (MUTEX_MGR_INFO*)malloc(sizeof(MUTEX_MGR_INFO))) == NULL)
    {
        log_error("Failure allocating mutex manager");
    }
    else
    {
    }
    return NULL;
}
