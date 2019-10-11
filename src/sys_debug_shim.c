// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>

#ifndef MEMORY_DEBUG_SHIM

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)~(size_t)0)
#endif

typedef struct ALLOCATION_TAG
{
    size_t size;
    void* ptr;
    void* next;
} ALLOCATION;

typedef enum GBALLOC_STATE_TAG
{
    GBALLOC_STATE_INIT,
    GBALLOC_STATE_NOT_INIT
} GBALLOC_STATE;

static ALLOCATION* head = NULL;
static size_t totalSize = 0;
static size_t maxSize = 0;
static size_t g_allocations = 0;
static GBALLOC_STATE gballocState = GBALLOC_STATE_NOT_INIT;
//static LOCK_HANDLE gballocThreadSafeLock = NULL;

int mem_shim_init(void)
{
    int result;
    if (gballocState != GBALLOC_STATE_NOT_INIT)
    {
        result = __LINE__;
    }
    /*else if ((gballocThreadSafeLock = Lock_Init()) == NULL)
    {
        result = __LINE__;
    }*/
    else
    {
        gballocState = GBALLOC_STATE_INIT;

        totalSize = 0;
        maxSize = 0;
        g_allocations = 0;
        result = 0;
    }
    return result;
}

void mem_shim_deinit(void)
{
    gballocState = GBALLOC_STATE_NOT_INIT;
}

void* mem_shim_malloc(size_t size)
{
    void* result;

    if (gballocState != GBALLOC_STATE_INIT)
    {
        result = malloc(size);
    }
    /*else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = NULL;
    }*/
    else
    {
        ALLOCATION* allocation = (ALLOCATION*)malloc(sizeof(ALLOCATION));
        if (allocation == NULL)
        {
            result = NULL;
        }
        else
        {
            if ((result = malloc(size)) == NULL)
            {
                free(allocation);
            }
            else
            {
                /* Codes_SRS_GBALLOC_01_004: [If the underlying malloc call is successful, gb_malloc shall increment the total memory used with the amount indicated by size.] */
                allocation->ptr = result;
                allocation->size = size;
                allocation->next = head;
                head = allocation;

                g_allocations++;
                totalSize += size;
                /* Codes_SRS_GBALLOC_01_011: [The maximum total memory used shall be the maximum of the total memory used at any point.] */
                if (maxSize < totalSize)
                {
                    maxSize = totalSize;
                }
            }
        }
        //(void)Unlock(gballocThreadSafeLock);
    }

    return result;

}

void* mem_shim_calloc(size_t nmemb, size_t size)
{
    void* result;
    if (gballocState != GBALLOC_STATE_INIT)
    {
        result = calloc(nmemb, size);
    }
    /*else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = NULL;
    }*/
    else
    {
        ALLOCATION* allocation = (ALLOCATION*)malloc(sizeof(ALLOCATION));
        if (allocation == NULL)
        {
            result = NULL;
        }
        else
        {
            if ((result = calloc(nmemb, size)) == NULL)
            {
                free(allocation);
            }
            else
            {
                allocation->ptr = result;
                allocation->size = nmemb * size;
                allocation->next = head;
                head = allocation;
                g_allocations++;

                totalSize += allocation->size;
                if (maxSize < totalSize)
                {
                    maxSize = totalSize;
                }
            }
        }
        //(void)Unlock(gballocThreadSafeLock);
    }
    return result;
}

void* mem_shim_realloc(void* ptr, size_t size)
{
    ALLOCATION* curr;
    void* result;
    ALLOCATION* allocation = NULL;

    if (gballocState != GBALLOC_STATE_INIT)
    {
        result = realloc(ptr, size);
    }
    /*else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = NULL;
    }*/
    else
    {
        if (ptr == NULL)
        {
            allocation = (ALLOCATION*)malloc(sizeof(ALLOCATION));
        }
        else
        {
            curr = head;
            while (curr != NULL)
            {
                if (curr->ptr == ptr)
                {
                    allocation = curr;
                    break;
                }
                else
                {
                    curr = (ALLOCATION*)curr->next;
                }
            }
        }

        if (allocation == NULL)
        {
            result = NULL;
        }
        else
        {
            result = realloc(ptr, size);
            if (result == NULL)
            {
                if (ptr == NULL)
                {
                    free(allocation);
                }
            }
            else
            {
                if (ptr != NULL)
                {
                    allocation->ptr = result;
                    totalSize -= allocation->size;
                    allocation->size = size;
                }
                else
                {
                    // add block
                    allocation->ptr = result;
                    allocation->size = size;
                    allocation->next = head;
                    head = allocation;
                }
                totalSize += size;
                g_allocations++;

                if (maxSize < totalSize)
                {
                    maxSize = totalSize;
                }
            }
        }
        //(void)Unlock(gballocThreadSafeLock);
    }

    return result;
}

void mem_shim_free(void* ptr)
{
    if (gballocState != GBALLOC_STATE_INIT)
    {
        free(ptr);
    }
    /*else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
    }*/
    else
    {
        ALLOCATION* curr = head;
        ALLOCATION* prev = NULL;
        while (curr != NULL)
        {
            if (curr->ptr == ptr)
            {
                free(ptr);
                totalSize -= curr->size;
                if (prev != NULL)
                {
                    prev->next = curr->next;
                }
                else
                {
                    head = (ALLOCATION*)curr->next;
                }

                free(curr);
                break;
            }

            prev = curr;
            curr = (ALLOCATION*)curr->next;
        }

        if ((curr == NULL) && (ptr != NULL))
        {
            /* could not find the allocation */
            //LogError("Could not free allocation for address %p (not found)", ptr);
        }
        //(void)Unlock(gballocThreadSafeLock);
    }
}

size_t mem_shim_get_maximum_memory(void)
{
    size_t result;
    if (gballocState != GBALLOC_STATE_INIT)
    {
        //LogError("gballoc is not initialized.");
        result = SIZE_MAX;
    }
    /*else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = SIZE_MAX;
    }*/
    else
    {
        result = maxSize;
        //(void)Unlock(gballocThreadSafeLock);
    }
    return result;
}

size_t mem_shim_get_current_memory(void)
{
    size_t result;
    if (gballocState != GBALLOC_STATE_INIT)
    {
        //LogError("gballoc is not initialized.");
        result = SIZE_MAX;
    }
    /*else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = SIZE_MAX;
    }*/
    else
    {
        result = totalSize;
        //(void)Unlock(gballocThreadSafeLock);
    }
    return result;
}

size_t mem_shim_get_allocations(void)
{
    size_t result;
    if (gballocState != GBALLOC_STATE_INIT)
    {
        //LogError("gballoc is not initialized.");
        result = 0;
    }
    /*else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
        result = 0;
    }*/
    else
    {
        result = g_allocations;
        //(void)Unlock(gballocThreadSafeLock);
    }
    return result;
}

void mem_shim_reset(void)
{
    if (gballocState != GBALLOC_STATE_INIT)
    {
        //LogError("gballoc is not initialized.");
    }
    /*else if (LOCK_OK != Lock(gballocThreadSafeLock))
    {
        LogError("Failed to get the Lock.");
    }*/
    else
    {
        totalSize = 0;
        maxSize = 0;
        g_allocations = 0;
        //(void)Unlock(gballocThreadSafeLock);
    }
}

#endif // MEMORY_DEBUG_SHIM
