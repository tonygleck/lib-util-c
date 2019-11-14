// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef ITEM_MAP_H
#define ITEM_MAP_H

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "umock_c/umock_c_prod.h"

typedef struct ITEM_MAP_INFO_TAG* ITEM_MAP_HANDLE;

typedef void(*ITEM_MAP_DESTROY_ITEM)(void* user_ctx, const char* key, void* remove_value);
typedef uint32_t(*ITEM_MAP_HASH_FUNCTION)(const char* key);

MOCKABLE_FUNCTION(, ITEM_MAP_HANDLE, item_map_create, size_t, size, ITEM_MAP_DESTROY_ITEM, destroy_cb, void*, user_ctx, ITEM_MAP_HASH_FUNCTION, hash_function);
MOCKABLE_FUNCTION(, void, item_map_destroy, ITEM_MAP_HANDLE, handle);
MOCKABLE_FUNCTION(, int, item_map_add_item, ITEM_MAP_HANDLE, handle, const char*, key, const void*, value, size_t, len);
MOCKABLE_FUNCTION(, const void*, item_map_get_item, ITEM_MAP_HANDLE, handle, const char*, key);
MOCKABLE_FUNCTION(, int, item_map_remove_item, ITEM_MAP_HANDLE, handle, const char*, key);
MOCKABLE_FUNCTION(, int, item_map_clear_all, ITEM_MAP_HANDLE, handle);
MOCKABLE_FUNCTION(, size_t, item_map_size, ITEM_MAP_HANDLE, handle);

#ifdef __cplusplus
}
#endif

#endif // ITEM_MAP_H
