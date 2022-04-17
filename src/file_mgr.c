// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "umock_c/umock_c_prod.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/file_mgr.h"

typedef struct FILE_MGR_INFO_TAG
{
    FILE* file;
} FILE_MGR_INFO;

static const char* get_file_mode(file_mode_value mode)
{
    const char* result;
    switch (mode)
    {
        default:
        case file_mode_read:
            result = "r";
            break;
        case file_mode_write:
            result = "w";
            break;
        case file_mode_append:
            result = "a";
            break;
        case file_mode_read_write:
            result = "r+";
            break;
        case file_mode_write_new:
            result = "w+";
            break;
        case file_mode_read_append:
            result = "a+";
            break;
    }
    return result;
}

FILE_MGR_HANDLE file_mgr_open(const char* filename, file_mode_value mode)
{
    FILE_MGR_INFO* result;
    if ((result = (FILE_MGR_INFO*)malloc(sizeof(FILE_MGR_INFO))) == NULL)
    {
        log_error("Failure allocating file manager info");
    }
    else if ((result->file = fopen(filename, get_file_mode(mode))) == NULL)
    {
        log_error("Failure loading wav %d: %s", errno, filename);
        free(result);
        result = NULL;
    }
    return result;
}

void file_mgr_close(FILE_MGR_HANDLE handle)
{
    if (handle != NULL)
    {
        fclose(handle->file);
        free(handle);
    }
}

long file_mgr_get_length(FILE_MGR_HANDLE handle)
{
    long result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle: NULL");
        result = 0;
    }
    else
    {
        fseek(handle->file, 0, SEEK_END);
        result = ftell(handle->file);
        fseek(handle->file, 0, SEEK_SET);
    }
    return result;
}

size_t file_mgr_read(FILE_MGR_HANDLE handle, unsigned char* buffer, size_t read_len)
{
    size_t result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle: NULL");
        result = 0;
    }
    else
    {
        result = fread(buffer, 1, read_len, handle->file);
    }
    return result;
}

size_t file_mgr_write(FILE_MGR_HANDLE handle, const unsigned char* buffer, size_t write_len)
{
    size_t result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle: NULL");
        result = 0;
    }
    else
    {
        result = fwrite(buffer, 1, write_len, handle->file);
    }
    return result;
}
