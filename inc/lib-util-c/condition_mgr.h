// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#endif

#include "umock_c/umock_c_prod.h"
#include "lib-util-c/mutex_mgr.h"

#ifdef WIN32
    #include <windows.h>

    typedef HANDLE SIGNAL_HANDLE;
#else
    #include <pthread.h>

    typedef pthread_cond_t SIGNAL_HANDLE;
#endif

MOCKABLE_FUNCTION(, int, condition_mgr_init, SIGNAL_HANDLE*, signal_item);
MOCKABLE_FUNCTION(, void, condition_mgr_deinit, SIGNAL_HANDLE, signal_item);
MOCKABLE_FUNCTION(, int, condition_mgr_signal, SIGNAL_HANDLE, signal_item);
MOCKABLE_FUNCTION(, int, condition_mgr_wait, SIGNAL_HANDLE, signal_item, MUTEX_HANDLE, mutex);
MOCKABLE_FUNCTION(, int, condition_mgr_timed_wait, SIGNAL_HANDLE, signal_item, MUTEX_HANDLE, mutex,
        const struct timespec*, abstime);
MOCKABLE_FUNCTION(, int, condition_mgr_broadcast, SIGNAL_HANDLE, signal_item);


#ifdef __cplusplus
}
#endif
