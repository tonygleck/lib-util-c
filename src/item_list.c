// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/item_list.h"
#include "lib-util-c/app_logging.h"

typedef struct ITEM_NODE_TAG
{
    struct ITEM_NODE_TAG* next;
    void* node_item;
    bool locally_allocated;
} ITEM_NODE;

typedef struct ITEM_LIST_INFO_TAG
{
    size_t item_count;
    ITEM_NODE* head_node;
    ITEM_NODE* tail_node;
    ITEM_LIST_DESTROY_ITEM destroy_cb;
    void* user_ctx;
    ITEM_NODE* iterator;
} ITEM_LIST_INFO;

typedef struct ITEM_ITERATOR_TAG
{
    struct ITEM_NODE_TAG* item;
} ITEM_ITERATOR;

static int add_new_item(ITEM_LIST_INFO* list_info, void* item, bool local_alloc)
{
    int result;
    ITEM_NODE* target = (ITEM_NODE*)malloc(sizeof(ITEM_NODE));
    if (target == NULL)
    {
        log_error("Failure allocating item node");
        result = __LINE__;
    }
    else
    {
        if (list_info->head_node == NULL)
        {
            list_info->head_node = target;
            list_info->head_node->node_item = item;
            list_info->head_node->locally_allocated = local_alloc;
            list_info->head_node->next = NULL;
            list_info->tail_node = list_info->head_node;
        }
        else
        {
            list_info->tail_node->next = target;
            list_info->tail_node->next->node_item = item;
            list_info->tail_node->next->locally_allocated = local_alloc;
            list_info->tail_node->next->next = NULL;
            // Set the tail node to the end
            list_info->tail_node = list_info->tail_node->next;
        }
        list_info->item_count++;
        result = 0;
    }
    return result;
}

static void clear_all_items(ITEM_LIST_INFO* list_info)
{
    for (size_t index = 0; index < list_info->item_count; index++)
    {
        ITEM_NODE* temp = list_info->head_node->next;
        if (list_info->head_node->locally_allocated)
        {
            free(list_info->head_node->node_item);
        }
        else
        {
            if (list_info->destroy_cb != NULL)
            {
                list_info->destroy_cb(list_info->user_ctx, list_info->head_node->node_item);
            }
        }
        free(list_info->head_node);
        list_info->head_node = temp;
    }
    list_info->item_count = 0;
    list_info->iterator = NULL;
}

ITEM_LIST_HANDLE item_list_create(ITEM_LIST_DESTROY_ITEM destroy_cb, void* user_ctx)
{
    ITEM_LIST_INFO* result;
    if ((result = (ITEM_LIST_INFO*)malloc(sizeof(ITEM_LIST_INFO))) == NULL)
    {
        log_error("Failure allocating item list buffer");
    }
    else
    {
        memset(result, 0, sizeof(ITEM_LIST_INFO));
        result->destroy_cb = destroy_cb;
        result->user_ctx = user_ctx;
    }
    return result;
}

void item_list_destroy(ITEM_LIST_HANDLE handle)
{
    if (handle != NULL)
    {
        clear_all_items(handle);
        free(handle);
    }
}

int item_list_add_item(ITEM_LIST_HANDLE handle, const void* item)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle NULL");
        result = __LINE__;
    }
    else
    {
        result = add_new_item(handle, (void*)item, false);
    }
    return result;
}

int item_list_add_copy(ITEM_LIST_HANDLE handle, const void* item, size_t item_size)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle NULL");
        result = __LINE__;
    }
    else
    {
        void* new_item = malloc(item_size);
        if (new_item == NULL)
        {
            log_error("Failure allocating item");
            result = __LINE__;
        }
        else
        {
            memcpy(new_item, item, item_size);
            if (add_new_item(handle, new_item, true) != 0)
            {
                log_error("Failure adding new item");
                free(new_item);
                result = __LINE__;
            }
            else
            {
                result = 0;
            }
        }
    }
    return result;
}

int item_list_remove_item(ITEM_LIST_HANDLE handle, size_t remove_index)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle NULL");
        result = __LINE__;
    }
    else if (remove_index >= handle->item_count)
    {
        log_error("Failure: invalid index specified %u", (unsigned int)remove_index);
        result = __LINE__;
    }
    else
    {
        ITEM_NODE* rm_pos = handle->head_node;
        ITEM_NODE* prev_item = NULL;
        for (size_t index = 0; index < remove_index; index++)
        {
            if (index > 0)
            {
                prev_item = rm_pos;
            }
            rm_pos = rm_pos->next;
        }
        // If the iterator points to this item
        // then move it
        if (handle->iterator == rm_pos)
        {
            handle->iterator = rm_pos->next;
        }

        if (handle->head_node->locally_allocated)
        {
            free(rm_pos->node_item);
        }
        else
        {
            if (handle->destroy_cb != NULL)
            {
                handle->destroy_cb(handle->user_ctx, rm_pos->node_item);
            }
        }
        if (prev_item == NULL)
        {
            handle->head_node = rm_pos->next;
        }
        else
        {
            prev_item->next = rm_pos->next;
        }
        handle->item_count--;
        free(rm_pos);
        result = 0;
    }
    return result;
}

size_t item_list_item_count(ITEM_LIST_HANDLE handle)
{
    size_t result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle NULL");
        result = 0;
    }
    else
    {
        result = handle->item_count;
    }
    return result;
}

const void* item_list_get_item(ITEM_LIST_HANDLE handle, size_t item_index)
{
    const void* result = NULL;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle NULL");
    }
    else if (item_index >= handle->item_count)
    {
        log_error("Invalid index size item_count: %zu item_index: %zu", handle->item_count, item_index);
    }
    else
    {
        ITEM_NODE* pos = handle->head_node;
        for (size_t index = 0; index < handle->item_count; index++)
        {
            if (index == item_index)
            {
                result = pos->node_item;
                break;
            }
            pos = pos->next;
        }
    }
    return result;
}

const void* item_list_get_front(ITEM_LIST_HANDLE handle)
{
    const void* result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle NULL");
        result = NULL;
    }
    else if (handle->item_count == 0)
    {
        result = NULL;
    }
    else
    {
        result = handle->head_node->node_item;
    }
    return result;
}

ITERATOR_HANDLE item_list_iterator(ITEM_LIST_HANDLE handle)
{
    ITERATOR_HANDLE result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle NULL");
        result = NULL;
    }
    else if (handle->item_count == 0)
    {
        result = NULL;
    }
    else
    {
        result = handle->iterator = handle->head_node;
    }
    return result;
}

const void* item_list_get_next(ITEM_LIST_HANDLE handle, ITERATOR_HANDLE* iterator)
{
    const void* result;
    if (handle == NULL || iterator == NULL)
    {
        log_error("Invalid parameter handle: %p, iterator: %p", handle, iterator);
        result = NULL;
    }
    else
    {
        if ((*iterator) == NULL)
        {
            result = NULL;
        }
        else
        {
            result = (*iterator)->node_item;
            *iterator = (*iterator)->next;
        }
    }
    return result;
}

int item_list_clear(ITEM_LIST_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter handle NULL");
        result = __LINE__;
    }
    else
    {
        clear_all_items(handle);
        result = 0;
    }
    return result;
}