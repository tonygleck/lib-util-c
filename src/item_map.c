// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "lib-util-c/item_map.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/crt_extensions.h"

typedef struct KEY_VALUE_MAPPING_TAG
{
    char* key;
    void* value;
    size_t len;
    bool locally_alloc;
    struct KEY_VALUE_MAPPING_TAG* next;
} KEY_VALUE_MAPPING;

typedef struct ITEM_MAP_INFO_TAG
{
    KEY_VALUE_MAPPING** value_array;
    size_t max_slots;
    ITEM_MAP_DESTROY_ITEM destroy_cb;
    void* user_ctx;
    ITEM_MAP_HASH_FUNCTION hash_function;
    size_t item_len;
} ITEM_MAP_INFO;

#define MIN_SLOT_SIZE       10
#define START_HASH_VALUE    5381

// Using djb2 Algorithm reported by dan bernstein many years ago in comp.lang.c
static uint32_t default_hash_function(const char* key)
{
    uint32_t result = START_HASH_VALUE;
    char str_value;

    while (str_value = *key++)
    {
        result = ((result << 5) + result) + str_value; // result * 33 + str_value;
    }
    return result;
}

static KEY_VALUE_MAPPING* store_key_value_item(const char* key, const void* value, size_t len)
{
    KEY_VALUE_MAPPING* result;
    if ((result = (KEY_VALUE_MAPPING*)malloc(sizeof(KEY_VALUE_MAPPING))) == NULL)
    {
        log_error("Failure allocating key value mapping");
    }
    else
    {
        memset(result, 0, sizeof(KEY_VALUE_MAPPING));
        if (clone_string(&result->key, key) != 0)
        {
            log_error("Failure cloning key info");
            free(result);
            result = NULL;
        }
        else if ((result->value = malloc(len)) == NULL)
        {
            free(result->key);
            log_error("Failure cloning key info");
            free(result);
            result = NULL;
        }
        else
        {
            memcpy(result->value, value, len);
            result->locally_alloc = true;
            result->len = len;
        }
    }
    return result;
}

static void free_map_value(ITEM_MAP_INFO* map_item, KEY_VALUE_MAPPING* key_value_item)
{
    if (!key_value_item->locally_alloc && map_item->destroy_cb != NULL)
    {
        map_item->destroy_cb(map_item->user_ctx, key_value_item->key, key_value_item->value);
    }
    else
    {
        free(key_value_item->value);
    }
}

static void clear_map(ITEM_MAP_INFO* map_item)
{
    for (size_t index = 0; index < map_item->max_slots; index++)
    {
        KEY_VALUE_MAPPING* kv_item = map_item->value_array[index];
        if (kv_item != NULL)
        {
            free_map_value(map_item, kv_item);
            free(kv_item->key);
            KEY_VALUE_MAPPING* iterator = kv_item->next;
            while (iterator != NULL)
            {
                KEY_VALUE_MAPPING* delete_item = iterator;
                iterator = iterator->next;
                free_map_value(map_item, delete_item);
                free(delete_item->key);
                free(delete_item);
            }
            free(kv_item);
        }
    }
}

ITEM_MAP_HANDLE item_map_create(size_t size, ITEM_MAP_DESTROY_ITEM destroy_cb, void* user_ctx, ITEM_MAP_HASH_FUNCTION hash_function)
{
    ITEM_MAP_INFO* result = (ITEM_MAP_INFO*)malloc(sizeof(ITEM_MAP_INFO));
    if (result == NULL)
    {
        log_error("Failure allocating item map item");
    }
    else
    {
        memset(result, 0, sizeof(ITEM_MAP_INFO));
        result->max_slots = size;
        result->destroy_cb = destroy_cb;
        result->user_ctx = user_ctx;
        if (result->max_slots < MIN_SLOT_SIZE)
        {
            result->max_slots = MIN_SLOT_SIZE;
        }
        if (hash_function == NULL)
        {
            result->hash_function = default_hash_function;
        }
        else
        {
            result->hash_function = hash_function;
        }

        if ((result->value_array = (KEY_VALUE_MAPPING**)malloc(sizeof(KEY_VALUE_MAPPING)*result->max_slots)) == NULL)
        {
            log_error("Failure allocating key value mapping");
            free(result);
            result = NULL;
        }
        else
        {
            memset(result->value_array, 0, sizeof(KEY_VALUE_MAPPING)*result->max_slots);
        }
    }
    return result;
}

void item_map_destroy(ITEM_MAP_HANDLE handle)
{
    if (handle != NULL)
    {
        // Free all items in the array
        clear_map(handle);
        free(handle->value_array);
        free(handle);
    }
}

int item_map_add_item(ITEM_MAP_HANDLE handle, const char* key, const void* value, size_t len)
{
    int result;
    if (handle == NULL || key == NULL || value == NULL || len == 0)
    {
        log_error("Invalid parameter specified handle: %p, key: %p", handle, key);
        result = __LINE__;
    }
    else
    {
        uint32_t hash_index = handle->hash_function(key);
        uint32_t index = hash_index % handle->max_slots;

        KEY_VALUE_MAPPING* kv_item = handle->value_array[index];
        if (kv_item == NULL)
        {
            if ((kv_item = store_key_value_item(key, value, len)) == NULL)
            {
                log_error("Failure cloning key info");
                result = __LINE__;
            }
            else
            {
                handle->value_array[index] = kv_item;
                handle->item_len++;
                result = 0;
            }
        }
        else
        {
            // Add to the end of the list
            KEY_VALUE_MAPPING* new_item;
            if ((new_item = store_key_value_item(key, value, len) ) == NULL)
            {
                log_error("Failure cloning key info");
                result = __LINE__;
            }
            else
            {
                // Add to the end of the link list
                KEY_VALUE_MAPPING** iterator = &(kv_item->next);
                while (*iterator != NULL)
                {
                    iterator = &(*iterator)->next;
                }
                *iterator = new_item;
                handle->item_len++;
                result = 0;
            }
        }
    }
    return result;
}

const void* item_map_get_item(ITEM_MAP_HANDLE handle, const char* key)
{
    const void* result;
    if (handle == NULL || key == NULL)
    {
        log_error("Invalid parameter specified handle: %p, key: %p", handle, key);
        result = NULL;
    }
    else
    {
        uint32_t hash_index = handle->hash_function(key);
        uint32_t index = hash_index % handle->max_slots;

        KEY_VALUE_MAPPING* kv_item = handle->value_array[index];
        KEY_VALUE_MAPPING* iterator = kv_item;
        result = kv_item->value;
        while (strcmp(iterator->key, key) != 0)
        {
            result = NULL;
            if (iterator->next == NULL)
            {
                // Didn't find an item
                break;
            }
            else
            {
                iterator = iterator->next;
                result = iterator->value;
            }
        }
    }
    return result;
}

int item_map_remove_item(ITEM_MAP_HANDLE handle, const char* key)
{
    int result;
    if (handle == NULL || key == NULL)
    {
        log_error("Invalid parameter specified handle: %p, key: %p", handle, key);
        result == __LINE__;
    }
    else
    {
        uint32_t hash_index = handle->hash_function(key);
        uint32_t index = hash_index % handle->max_slots;

        KEY_VALUE_MAPPING* kv_item = handle->value_array[index];
        if (kv_item != NULL)
        {
            if (strcmp(kv_item->key, key) == 0)
            {
                free_map_value(handle, kv_item);
                free(kv_item->key);
                if (kv_item->next != NULL)
                {
                    handle->value_array[index] = kv_item->next;
                }
                else
                {
                    handle->value_array[index] = NULL;
                }
                free(kv_item);
                handle->item_len--;
            }
            else
            {
                result = 0;
                KEY_VALUE_MAPPING* iterator = kv_item;
                while (iterator->next != NULL)
                {
                    if (strcmp(iterator->next->key, key) == 0)
                    {
                        KEY_VALUE_MAPPING* delete_item = iterator->next;
                        // The item is in the array
                        free_map_value(handle, delete_item);
                        free(delete_item->key);
                        if (delete_item->next != NULL)
                        {
                            iterator->next = delete_item->next;
                        }
                        else
                        {
                            iterator->next = NULL;
                        }
                        free(delete_item);
                        handle->item_len--;
                    }
                }
            }
        }
        else
        {
            // No Items are found at this index
            result = 0;
        }
    }
    return result;
}

int item_map_clear_all(ITEM_MAP_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified handle: NULL");
        result = __LINE__;
    }
    else
    {
        clear_map(handle);
        handle->item_len = 0;
    }
    return result;
}

size_t item_map_size(ITEM_MAP_HANDLE handle)
{
    size_t result;
    if (handle == NULL)
    {
        log_error("Invalid parameter specified handle: NULL");
        result = 0;
    }
    else
    {
        result = handle->item_len;
    }
    return result;
}
