// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "umock_c/umock_c_prod.h"

#define CONTAINING_RECORD(address, type, field) ((type *)((uintptr_t)(address) - offsetof(type,field)))

typedef struct DLLIST_ENTRY_TAG
{
    struct DLLIST_ENTRY_TAG* fwd_link;
    struct DLLIST_ENTRY_TAG* bk_link;
} DLLIST_ENTRY, *PDLLIST_ENTRY;

MOCKABLE_FUNCTION(, void, dllist_init_list_head, PDLLIST_ENTRY, list_head);
MOCKABLE_FUNCTION(, bool, dllist_is_empty, const PDLLIST_ENTRY, list_head);
MOCKABLE_FUNCTION(, void, dllist_insert_tail, PDLLIST_ENTRY, list_head, PDLLIST_ENTRY, listEntry);
MOCKABLE_FUNCTION(, void, dllist_insert_head, PDLLIST_ENTRY, list_head, PDLLIST_ENTRY, listEntry);
MOCKABLE_FUNCTION(, void, dllist_append_tail, PDLLIST_ENTRY, list_head, PDLLIST_ENTRY, append_list);
MOCKABLE_FUNCTION(, int, dllist_remove_entry, PDLLIST_ENTRY, listEntry);
MOCKABLE_FUNCTION(, PDLLIST_ENTRY, dllist_remove_head, PDLLIST_ENTRY, list_head);

#ifdef __cplusplus
}
#endif
