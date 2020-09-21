
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/sha_algorithms.h"

typedef struct SHA_CTX_TAG
{
    const SHA_HASH_INTERFACE* hash_interface;
    SHA_IMPL_HANDLE sha_impl_handle;
} SHA_CTX;

SHA_CTX_HANDLE sha_algorithms_init(const SHA_HASH_INTERFACE* hash_interface)
{
    SHA_CTX* result;
    if (hash_interface == NULL)
    {
        log_error("invalid parameter specified");
        result = NULL;
    }
    else if ((result = (SHA_CTX*)malloc(sizeof(SHA_CTX))) == NULL)
    {
        log_error("Unable to allocat sha info");
    }
    else
    {
        result->hash_interface = hash_interface;
        if (result->hash_interface->initialize_fn == NULL ||
            result->hash_interface->deinitialize_fn == NULL ||
            result->hash_interface->process_fn == NULL ||
            result->hash_interface->retrieve_result_fn == NULL)
        {
            log_error("Invalid hash interface");
            free(result);
            result = NULL;
        }
        else
        {
            result->sha_impl_handle = result->hash_interface->initialize_fn();
        }
    }
    return result;
}

void sha_algorithms_deinit(SHA_CTX_HANDLE handle)
{
    if (handle != NULL)
    {
        handle->hash_interface->deinitialize_fn(handle->sha_impl_handle);
        free(handle);
    }
}

int sha_algorithms_process(SHA_CTX_HANDLE handle, const uint8_t* msg_array, size_t array_len, uint8_t msg_digest[], size_t digest_len)
{
    int result;
    if (handle == NULL || msg_array == NULL || array_len == 0 || msg_digest == 0)
    {
        log_error("Invalid parameter specified handle: %p, msg_array: %p, array_len: %lu, msg_digest: %p", handle, msg_array, array_len, msg_digest);
        result = __LINE__;
    }
    else
    {
        if (handle->hash_interface->process_fn(handle->sha_impl_handle, msg_array, array_len) != 0)
        {
            log_error("Failing process sha function");
            result = __LINE__;
        }
        else if (handle->hash_interface->retrieve_result_fn(handle->sha_impl_handle, msg_digest, digest_len) != 0)
        {
            log_error("Failing retrieving result from sha process");
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}
