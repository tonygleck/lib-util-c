// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
#include <cstdio>
extern "C" {
#else // __cplusplus
#include <stdio.h>
#include <stddef.h>
#endif // __cplusplus

#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

typedef struct BINARY_TREE_INFO_TAG* BINARY_TREE_HANDLE;

typedef void (*tree_remove_callback)(void* data);

// Used as the type value
typedef unsigned char NODE_KEY;

MOCKABLE_FUNCTION(, BINARY_TREE_HANDLE, binary_tree_create);
MOCKABLE_FUNCTION(, void, binary_tree_destroy, BINARY_TREE_HANDLE, handle);

MOCKABLE_FUNCTION(, int, binary_tree_insert, BINARY_TREE_HANDLE, handle, NODE_KEY, value, void*, data);
MOCKABLE_FUNCTION(, int, binary_tree_remove, BINARY_TREE_HANDLE, handle, NODE_KEY, value, tree_remove_callback, remove_callback);
MOCKABLE_FUNCTION(, void*, binary_tree_find, BINARY_TREE_HANDLE, handle, NODE_KEY, find_value);

// Diagnostic function
MOCKABLE_FUNCTION(, size_t, binary_tree_item_count, BINARY_TREE_HANDLE, handle);
MOCKABLE_FUNCTION(, size_t, binary_tree_height, BINARY_TREE_HANDLE, handle);
MOCKABLE_FUNCTION(, void, binary_tree_print, BINARY_TREE_HANDLE, handle);
MOCKABLE_FUNCTION(, char*, binary_tree_construct_visual, BINARY_TREE_HANDLE, handle);

#ifdef __cplusplus
}
#endif
