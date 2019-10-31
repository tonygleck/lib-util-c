// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CRT_EXTENSIONS_H
#define CRT_EXTENSIONS_H

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

MOCKABLE_FUNCTION(, int, clone_string, char**, target, const char*, source);

#ifdef __cplusplus
}
#endif

#endif // CRT_EXTENSIONS_H
