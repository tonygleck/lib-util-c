#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "lib-util-c/item_list.h"

typedef struct ITEM_NODE_TAG
{
    ITEM_NODE* next;
    void* node_item;
} ITEM_NODE;

typedef struct ITEM_LIST_INFO_TAG
{
    size_t item_count;
    ITEM_NODE* head_node;
    ITEM_NODE* tail_node;
} ITEM_LIST_INFO;

ITEM_LIST_HANDLE item_list_create(void)
{
    ITEM_LIST_INFO* result;
    if ((result = (ITEM_LIST_INFO*)malloc(sizeof(ITEM_LIST_INFO))) == NULL)
    {

    }
    else
    {
        memset(result, 0, sizeof(ITEM_LIST_INFO));
    }
    return result;
}

void item_list_destroy(ITEM_LIST_HANDLE handle)
{
    if (handle != NULL)
    {

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
        if (handle->head_node == NULL)
        {
            handle->head_node = (ITEM_NODE*)malloc(sizeof(ITEM_NODE));
            handle->head_node->node_item = item;
            handle->head_node->next = NULL;
            handle->tail_node = handle->head_node->next;
        }
        else
        {
            handle->tail_node->next = (ITEM_NODE*)malloc(sizeof(ITEM_NODE));
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

int item_list_remove_item(ITEM_LIST_HANDLE handle, size_t index)
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
