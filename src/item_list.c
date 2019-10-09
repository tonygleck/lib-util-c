// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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
} ITEM_LIST_INFO;

static void add_new_item(ITEM_LIST_INFO* list_info, void* item, bool local_alloc)
{
    ITEM_NODE* target = (ITEM_NODE*)malloc(sizeof(ITEM_NODE));
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
        list_info->tail_node->next->next = NULL;
        // Set the tail node to the end
        list_info->tail_node = list_info->tail_node->next;
    }
    list_info->item_count++;
}

ITEM_LIST_HANDLE item_list_create(ITEM_LIST_DESTROY_ITEM destroy_cb, void* user_ctx)
{
    ITEM_LIST_INFO* result;
    if (destroy_cb == NULL)
    {
        log_error("Invalid parameter destroy callback");
        result = NULL;
    }
    else if ((result = (ITEM_LIST_INFO*)malloc(sizeof(ITEM_LIST_INFO))) == NULL)
    {
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
        for (size_t index = 0; index < handle->item_count; index++)
        {
            ITEM_NODE* temp = handle->head_node->next;
            if (handle->head_node->locally_allocated)
            {
                free(handle->head_node->node_item);
            }
            else
            {
                handle->destroy_cb(handle->user_ctx, handle->head_node->node_item);
            }
            free(handle->head_node);
            handle->head_node = temp;
        }
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
        add_new_item(handle, (void*)item, false);
        result = 0;
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
            add_new_item(handle, new_item, true);
            result = 0;
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
            rm_pos = handle->head_node->next;
        }
        if (handle->head_node->locally_allocated)
        {
            free(rm_pos->node_item);
        }
        else
        {
            handle->destroy_cb(handle->user_ctx, rm_pos->node_item);
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
        log_error("Invalid index size");
    }
    else
    {
        ITEM_NODE* pos = handle->head_node;
        //ITEM_NODE* prev_item = NULL;

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
