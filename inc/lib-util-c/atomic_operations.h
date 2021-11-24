// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

MOCKABLE_FUNCTION(, long, atomic_increment, long*, value);
MOCKABLE_FUNCTION(, int64_t, atomic_increment64, int64_t*, value);
MOCKABLE_FUNCTION(, long, atomic_decrement, long*, value);
MOCKABLE_FUNCTION(, int64_t, atomic_decrement64, int64_t*, value);
MOCKABLE_FUNCTION(, long, atomic_add, long*, operand, long, value);
MOCKABLE_FUNCTION(, long, atomic_subtract, long*, operand, long, value);

#ifdef __cplusplus
}
#endif
