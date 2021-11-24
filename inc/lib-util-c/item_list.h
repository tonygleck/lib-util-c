// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else

#endif

#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

typedef struct ITEM_LIST_INFO_TAG* ITEM_LIST_HANDLE;

typedef struct ITEM_NODE_TAG* ITERATOR_HANDLE;

typedef void(*ITEM_LIST_DESTROY_ITEM)(void* user_ctx, void* remove_item);

MOCKABLE_FUNCTION(, ITEM_LIST_HANDLE, item_list_create, ITEM_LIST_DESTROY_ITEM, destroy_cb, void*, user_ctx);
MOCKABLE_FUNCTION(, void, item_list_destroy, ITEM_LIST_HANDLE, handle);

MOCKABLE_FUNCTION(, int, item_list_add_item, ITEM_LIST_HANDLE, handle, const void*, item);
MOCKABLE_FUNCTION(, int, item_list_add_copy, ITEM_LIST_HANDLE, handle, const void*, item, size_t, item_size);
MOCKABLE_FUNCTION(, int, item_list_remove_item, ITEM_LIST_HANDLE, handle, size_t, index);
MOCKABLE_FUNCTION(, size_t, item_list_item_count, ITEM_LIST_HANDLE, handle);
MOCKABLE_FUNCTION(, const void*, item_list_get_item, ITEM_LIST_HANDLE, handle, size_t, item_index);
MOCKABLE_FUNCTION(, const void*, item_list_get_front, ITEM_LIST_HANDLE, handle);
MOCKABLE_FUNCTION(, int, item_list_clear, ITEM_LIST_HANDLE, handle);

// Resets the iterator to the inital item
MOCKABLE_FUNCTION(, ITERATOR_HANDLE, item_list_iterator, ITEM_LIST_HANDLE, handle);
// Moves the iterator the the next item
MOCKABLE_FUNCTION(, const void*, item_list_get_next, ITEM_LIST_HANDLE, handle, ITERATOR_HANDLE*, iterator);


#ifdef __cplusplus
}
#endif
