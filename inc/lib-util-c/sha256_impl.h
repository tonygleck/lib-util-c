// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#endif

#include "sha_algorithms.h"

#define SHA256_HASH_SIZE    32

MOCKABLE_FUNCTION(, const SHA_HASH_INTERFACE*, sha256_get_interface);

#ifdef __cplusplus
}
#endif
