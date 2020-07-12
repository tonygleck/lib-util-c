// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#ifdef __cplusplus
#include <cstddef>
#include <ctime>
#else
#include <stddef.h>
#include <time.h>
#endif

#include <pthread.h>
#include <errno.h>

// Include the test tools.
#include "ctest.h"
#include "azure_macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_charptr.h"
#include "umock_c/umock_c_negative_tests.h"

static void* my_mem_shim_malloc(size_t size)
{
    return malloc(size);
}

static void* my_mem_shim_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

static void my_mem_shim_free(void* ptr)
{
    free(ptr);
}

#define ENABLE_MOCKS
#include "lib-util-c/sys_debug_shim.h"

typedef void*(*start_routine)(void*);

MOCKABLE_FUNCTION(, int, pthread_create, pthread_t*, thread, const pthread_attr_t*, attr, start_routine, start_func, void*, arg);
MOCKABLE_FUNCTION(, int, pthread_join, pthread_t, thread, void**, value_ptr);
MOCKABLE_FUNCTION(, int, pthread_detach, pthread_t, thread);
#undef ENABLE_MOCKS

#include "lib-util-c/thread_mgr.h"

static start_routine g_test_start_func = NULL;
static void* start_parameter = NULL;

static int test_thread_start_func(void* parameter)
{
    return 0;
}

static int my_pthread_create(pthread_t* thread, const pthread_attr_t* attr, start_routine start_func, void* arg)
{
    (void)attr;
    g_test_start_func = start_func;
    start_parameter = arg;
    thread = (pthread_t*)my_mem_shim_malloc(1);
    return 0;
}

static int my_pthread_join(pthread_t thread, void** value_ptr)
{
    (void)value_ptr;
    my_mem_shim_free(&thread);
    return 0;
}

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(thread_mgr_posix_ut)

CTEST_SUITE_INITIALIZE()
{
    int result;

    (void)umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_realloc, my_mem_shim_realloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_realloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);

    REGISTER_GLOBAL_MOCK_HOOK(pthread_create, my_pthread_create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_create, EAGAIN);
    REGISTER_GLOBAL_MOCK_HOOK(pthread_join, my_pthread_join);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_join, __LINE__);

    //REGISTER_UMOCK_ALIAS_TYPE(ALARM_TIMER_HANDLE, void*);
}

CTEST_SUITE_CLEANUP()
{
    umock_c_deinit();
}

CTEST_FUNCTION_INITIALIZE()
{
    umock_c_reset_all_calls();
}

CTEST_FUNCTION_CLEANUP()
{
}

CTEST_FUNCTION(thread_mgr_init_succeed)
{
    //arrange
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(pthread_create(IGNORED_ARG, IGNORED_ARG, IGNORED_ARG, IGNORED_ARG));

    //act
    THREAD_MGR_HANDLE result = thread_mgr_init(test_thread_start_func, NULL);

    //assert
    CTEST_ASSERT_IS_NOT_NULL(result);

    //cleanup
    thread_mgr_join(result);
}

CTEST_END_TEST_SUITE(thread_mgr_posix_ut)
