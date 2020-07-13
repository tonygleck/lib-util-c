// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

#ifdef WIN32
    #include <windows.h>

    typedef HANDLE MUTEX_HANDLE;
#else
    #include <pthread.h>

    typedef pthread_mutex_t MUTEX_HANDLE;
#endif

MOCKABLE_FUNCTION(, int, mutex_mgr_create, MUTEX_HANDLE, handle);
MOCKABLE_FUNCTION(, void, mutex_mgr_destroy, MUTEX_HANDLE, handle);
MOCKABLE_FUNCTION(, int, mutex_mgr_lock, MUTEX_HANDLE, handle);
MOCKABLE_FUNCTION(, int, mutex_mgr_trylock, MUTEX_HANDLE, handle);
MOCKABLE_FUNCTION(, int, mutex_mgr_unlock, MUTEX_HANDLE, handle);

#ifdef __cplusplus
}
#endif
