// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "lib-util-c/item_list.h"

typedef struct ITEM_NODE_TAG
{
    struct ITEM_NODE_TAG* next;
    void* node_item;
} ITEM_NODE;

typedef struct ITEM_LIST_INFO_TAG
{
    size_t item_count;
    ITEM_NODE* head_node;
    ITEM_NODE* tail_node;
    ITEM_LIST_DESTROY_ITEM destroy_cb;
    void* user_ctx;
} ITEM_LIST_INFO;

ITEM_LIST_HANDLE item_list_create(ITEM_LIST_DESTROY_ITEM destroy_cb, void* user_ctx)
{
    ITEM_LIST_INFO* result;
    if ((result = (ITEM_LIST_INFO*)malloc(sizeof(ITEM_LIST_INFO))) == NULL)
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
            handle->destroy_cb(handle->user_ctx, handle->head_node->node_item);
            free(handle->head_node);
            handle->head_node = temp;
        }
        free(handle);
    }
}

int item_list_add_item(ITEM_LIST_HANDLE handle, void* item)
{
    int result;
    if (handle == NULL)
    {
        result = __LINE__;
    }
    else
    {
        ITEM_NODE* target = (ITEM_NODE*)malloc(sizeof(ITEM_NODE));
        if (handle->head_node == NULL)
        {
            handle->head_node = target;
            handle->head_node->node_item = item;
            handle->head_node->next = NULL;
            handle->tail_node = handle->head_node->next;
        }
        else
        {
            handle->tail_node->next = target;
            handle->tail_node->next->node_item = item;
            handle->tail_node->next->next = NULL;
            // Set the tail node to the end
            handle->tail_node = handle->tail_node->next;
        }
        handle->item_count++;
        result = 0;
    }
    return result;
}

int item_list_add_copy(ITEM_LIST_HANDLE handle, const void* item, size_t item_size)
{
    int result;
    if (handle == NULL)
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
    }
    return result;
}

int item_list_remove_item(ITEM_LIST_HANDLE handle, size_t remove_index)
{
    int result;
    if (handle == NULL)
    {
        result = __LINE__;
    }
    else if (remove_index == 0 || remove_index >= handle->item_count)
    {
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
        handle->destroy_cb(handle->user_ctx, rm_pos->node_item);
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
        result = 0;
    }
    else
    {
        result = handle->item_count;
    }
    return result;
}
