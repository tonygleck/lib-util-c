
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#else
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#endif

#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

typedef void* SHA_IMPL_HANDLE;
typedef struct SHA_CTX_TAG* SHA_CTX_HANDLE;

typedef SHA_IMPL_HANDLE(*initialize_hash)(void);
typedef void(*deinitialize_hash)(SHA_IMPL_HANDLE handle);
typedef int(*process_hash)(SHA_IMPL_HANDLE handle, const uint8_t* msg_array, size_t array_len);
typedef int(*retrieve_hash_result)(SHA_IMPL_HANDLE handle, uint8_t msg_digest[], size_t digest_len);

typedef struct SHA_HASH_INTERFACE_TAG
{
    initialize_hash initialize_fn;
    deinitialize_hash deinitialize_fn;
    process_hash process_fn;
    retrieve_hash_result retrieve_result_fn;
} SHA_HASH_INTERFACE;

MOCKABLE_FUNCTION(, SHA_CTX_HANDLE, sha_algorithms_init, const SHA_HASH_INTERFACE*, hash_interface);
MOCKABLE_FUNCTION(, int, sha_algorithms_process, SHA_CTX_HANDLE, handle, const uint8_t*, msg_array, size_t, array_len, uint8_t, msg_digest[], size_t, digest_len);
MOCKABLE_FUNCTION(, void, sha_algorithms_deinit, SHA_CTX_HANDLE, handle);

#ifdef __cplusplus
}
#endif
