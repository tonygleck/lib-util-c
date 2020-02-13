// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#include <time.h>
#endif

#include "umock_c/umock_c_prod.h"

typedef struct FILE_MGR_INFO_TAG* FILE_MGR_HANDLE;

MOCKABLE_FUNCTION(, FILE_MGR_HANDLE, file_mgr_open, const char*, filename, const char*, param);
MOCKABLE_FUNCTION(, void, file_mgr_close, FILE_MGR_HANDLE, handle);
MOCKABLE_FUNCTION(, long, file_mgr_get_length, FILE_MGR_HANDLE, handle);
MOCKABLE_FUNCTION(, size_t, file_mgr_read, FILE_MGR_HANDLE, handle, unsigned char*, buffer, size_t, read_len);
MOCKABLE_FUNCTION(, size_t, file_mgr_write, FILE_MGR_HANDLE, handle, const unsigned char*, buffer, size_t, write_len);

#ifdef __cplusplus
}
#endif

