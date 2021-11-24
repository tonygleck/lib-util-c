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
#include "macro_utils/macro_utils.h"

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

MOCKABLE_FUNCTION(, int, test_thread_start_func, void*, parameter);
#undef ENABLE_MOCKS

#include "lib-util-c/thread_mgr.h"

static void* g_test_thread_start_param = (void*)0x543210;
static start_routine g_test_start_func = NULL;
static void* g_start_parameter = NULL;

static int my_pthread_create(pthread_t* thread, const pthread_attr_t* attr, start_routine start_func, void* arg)
{
    (void)attr;
    g_test_start_func = start_func;
    g_start_parameter = arg;
    return 0;
}

static int my_pthread_join(pthread_t thread, void** value_ptr)
{
    (void)value_ptr;
    (void)thread;
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

    REGISTER_UMOCK_ALIAS_TYPE(start_routine, void*);
    REGISTER_UMOCK_ALIAS_TYPE(THREAD_START_FUNC, void*);
    REGISTER_UMOCK_ALIAS_TYPE(pthread_t, unsigned long);

    //REGISTER_TYPE(pthread_t, pthread_t);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_realloc, my_mem_shim_realloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_realloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);

    REGISTER_GLOBAL_MOCK_HOOK(pthread_create, my_pthread_create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_create, EAGAIN);
    REGISTER_GLOBAL_MOCK_HOOK(pthread_join, my_pthread_join);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_join, __LINE__);

    REGISTER_GLOBAL_MOCK_RETURN(test_thread_start_func, 0);
}

CTEST_SUITE_CLEANUP()
{
    umock_c_deinit();
}

CTEST_FUNCTION_INITIALIZE()
{
    umock_c_reset_all_calls();
    g_test_start_func = NULL;
    g_start_parameter = NULL;
}

CTEST_FUNCTION_CLEANUP()
{
}

CTEST_FUNCTION(thread_mgr_init_start_func_NULL_fail)
{
    //arrange

    //act
    THREAD_MGR_HANDLE result = thread_mgr_init(NULL, NULL);

    //assert
    CTEST_ASSERT_IS_NULL(result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
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
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    thread_mgr_join(result);
}

CTEST_FUNCTION(thread_mgr_init_fail)
{
    //arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(pthread_create(IGNORED_ARG, IGNORED_ARG, IGNORED_ARG, IGNORED_ARG));

    umock_c_negative_tests_snapshot();

    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (umock_c_negative_tests_can_call_fail(index))
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(index);

            // act
            THREAD_MGR_HANDLE result = thread_mgr_init(test_thread_start_func, NULL);
            // assert

            //assert
            CTEST_ASSERT_IS_NULL(result);
        }
    }

    //cleanup
        umock_c_negative_tests_deinit();
}

CTEST_FUNCTION(thread_mgr_join_succeed)
{
    THREAD_MGR_HANDLE handle = thread_mgr_init(test_thread_start_func, NULL);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_join(IGNORED_ARG, IGNORED_ARG));
    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    //act
    int result = thread_mgr_join(handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(thread_mgr_join_handle_NULL_succeed)
{
    //arrange

    //act
    int result = thread_mgr_join(NULL);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(thread_mgr_detach_succeed)
{
    THREAD_MGR_HANDLE handle = thread_mgr_init(test_thread_start_func, g_test_thread_start_param);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_detach(IGNORED_ARG));
    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    //act
    int result = thread_mgr_detach(handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(thread_mgr_detach_fail)
{
    THREAD_MGR_HANDLE handle = thread_mgr_init(test_thread_start_func, g_test_thread_start_param);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_detach(IGNORED_ARG)).SetReturn(EINVAL);

    //act
    int result = thread_mgr_detach(handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    (void)thread_mgr_join(handle);
}

CTEST_FUNCTION(thread_mgr_detach_handle_NULL_succeed)
{
    //arrange

    //act
    int result = thread_mgr_detach(NULL);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(thread_worker_func_succeed)
{
    THREAD_MGR_HANDLE handle = thread_mgr_init(test_thread_start_func, g_test_thread_start_param);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(test_thread_start_func(g_test_thread_start_param)).SetReturn(EINVAL);

    //act
    g_test_start_func(g_start_parameter);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    (void)thread_mgr_join(handle);
}

CTEST_END_TEST_SUITE(thread_mgr_posix_ut)
