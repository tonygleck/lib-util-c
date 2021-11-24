// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

#if defined(USE_MEMORY_DEBUG_SHIM)
MOCKABLE_FUNCTION(, int, mem_shim_init);
MOCKABLE_FUNCTION(, void, mem_shim_deinit);
MOCKABLE_FUNCTION(, void*, mem_shim_malloc, size_t, size);
MOCKABLE_FUNCTION(, void*, mem_shim_calloc, size_t, nmemb, size_t, size);
MOCKABLE_FUNCTION(, void*, mem_shim_realloc, void*, ptr, size_t, size);
MOCKABLE_FUNCTION(, void, mem_shim_free, void*, ptr);

MOCKABLE_FUNCTION(, size_t, mem_shim_get_maximum_memory);
MOCKABLE_FUNCTION(, size_t, mem_shim_get_current_memory);
MOCKABLE_FUNCTION(, size_t, mem_shim_get_allocations);
MOCKABLE_FUNCTION(, void, mem_shim_reset);

#ifdef WIN32

#include <windows.h>

MOCKABLE_FUNCTION(, HANDLE, sys_shim_CreateMutex, LPSECURITY_ATTRIBUTES, lpMutexAttributes, BOOL, bInitialOwner, LPCSTR, lpName);
MOCKABLE_FUNCTION(, BOOL, sys_shim_CloseHandle, HANDLE, hObject);
MOCKABLE_FUNCTION(, DWORD, sys_shim_WaitForSingleObject, HANDLE, hHandle, DWORD, dwMilliseconds);
MOCKABLE_FUNCTION(, BOOL, sys_shim_ReleaseMutex, HANDLE, hMutex);
#endif

//#ifdef USE_MEM_SHIM_ALLOCATOR
/* Unfortunately this is still needed here for things to still compile when using _CRTDBG_MAP_ALLOC.
That is because there is a rogue component (most likely CppUnitTest) including crtdbg. */
#if defined(_CRTDBG_MAP_ALLOC) && defined(_DEBUG)
#undef _malloc_dbg
#undef _calloc_dbg
#undef _realloc_dbg
#undef _free_dbg
#define _malloc_dbg(size, ...) mem_shim_malloc(size)
#define _calloc_dbg(nmemb, size, ...) mem_shim_calloc(nmemb, size)
#define _realloc_dbg(ptr, size, ...) mem_shim_realloc(ptr, size)
#define _free_dbg(ptr, ...) mem_shim_free(ptr)
#else
#define malloc mem_shim_malloc
#define calloc mem_shim_calloc
#define realloc mem_shim_realloc
#define free mem_shim_free
#endif
//#endif // USE_MEM_SHIM_ALLOCATOR

#ifdef WIN32
#define CreateMutexW        sys_shim_CreateMutex
#define CloseHandle         sys_shim_CloseHandle
#define WaitForSingleObject sys_shim_WaitForSingleObject
#define ReleaseMutex        sys_shim_ReleaseMutex
#endif

#else // USE_MEMORY_DEBUG_SHIM

#define mem_shim_init() 0
#define mem_shim_deinit() ((void)0)

#define mem_shim_getMaximumMemoryUsed() SIZE_MAX
#define mem_shim_getCurrentMemoryUsed() SIZE_MAX
#define mem_shim_getAllocationCount() SIZE_MAX
#define mem_shim_resetMetrics() ((void)0)

#endif  // USE_MEMORY_DEBUG_SHIM

#ifdef __cplusplus
}
#endif
