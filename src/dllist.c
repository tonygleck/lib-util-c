// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "lib-util-c/dllist.h"

void dllist_init_list_head(PDLLIST_ENTRY list_head)
{
    if (list_head != NULL)
    {
        list_head->fwd_link = list_head->bk_link = list_head;
    }
    return;
}

bool dllist_is_empty(const PDLLIST_ENTRY list_head)
{
    return (list_head->fwd_link == list_head);
}

int dllist_remove_entry(PDLLIST_ENTRY entry)
{
    PDLLIST_ENTRY bk_link;
    PDLLIST_ENTRY fwd_link;

    fwd_link = entry->fwd_link;
    bk_link = entry->bk_link;
    bk_link->fwd_link = fwd_link;
    fwd_link->bk_link = bk_link;
    return (fwd_link == bk_link);
}

PDLLIST_ENTRY dllist_remove_head(PDLLIST_ENTRY list_head)
{
    PDLLIST_ENTRY fwd_link;
    PDLLIST_ENTRY entry;

    entry = list_head->fwd_link;
    fwd_link = entry->fwd_link;
    list_head->fwd_link = fwd_link;
    fwd_link->bk_link = list_head;
    return entry;
}

void dllist_insert_tail(PDLLIST_ENTRY list_head, PDLLIST_ENTRY entry)
{
    PDLLIST_ENTRY bk_link;

    bk_link = list_head->bk_link;
    entry->fwd_link = list_head;
    entry->bk_link = bk_link;
    bk_link->fwd_link = entry;
    list_head->bk_link = entry;
}

void dllist_append_tail(PDLLIST_ENTRY list_head, PDLLIST_ENTRY append_list)
{
    PDLLIST_ENTRY ListEnd = list_head->bk_link;

    list_head->bk_link->fwd_link = append_list;
    list_head->bk_link = append_list->bk_link;
    append_list->bk_link->fwd_link = list_head;
    append_list->bk_link = ListEnd;
}

void dllist_insert_head(PDLLIST_ENTRY listHead, PDLLIST_ENTRY entry)
{
    entry->bk_link = listHead;
    entry->fwd_link = listHead->fwd_link;
    listHead->fwd_link->bk_link = entry;
    listHead->fwd_link = entry;
}
