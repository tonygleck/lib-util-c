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
#include "lib-util-c/mutex_mgr.h"
#include "lib-util-c/condition_mgr.h"
#include "lib-util-c/thread_mgr.h"

MOCKABLE_FUNCTION(, void, test_thread_start_func, void*, parameter);

#undef ENABLE_MOCKS

#include "lib-util-c/threadpool.h"

#define THREADPOOL_INIT_NUM             5
#define THREADPOOL_MIN_INIT_NUM         2
#define THREADPOOL_DEFAULT_INIT_NUM     3

static void* g_thread_start_param = (void*)0x123456;
static THREAD_START_FUNC g_thread_function;
static void* g_thread_param;
static MUTEX_HANDLE g_mutex_handle;
static SIGNAL_HANDLE g_signal_handle;

static int my_mutex_mgr_create(MUTEX_HANDLE* handle)
{
#ifdef WIN32
#else
    g_mutex_handle.__align = 0;
    *handle = g_mutex_handle;
#endif
    return 0;
}

static void my_mutex_mgr_destroy(MUTEX_HANDLE handle)
{
#ifdef WIN32
#else
    CTEST_ASSERT_ARE_EQUAL(long, 0, handle.__align);
#endif
}

static int my_mutex_mgr_lock(MUTEX_HANDLE handle)
{
#ifdef WIN32
#else
    handle.__align++;
#endif
}

static int my_mutex_mgr_unlock(MUTEX_HANDLE handle)
{
#ifdef WIN32
#else
    handle.__align--;
#endif
}

static int my_condition_mgr_init(SIGNAL_HANDLE* handle)
{
    return 0;
}

static void my_condition_mgr_deinit(SIGNAL_HANDLE handle)
{
}

static THREAD_MGR_HANDLE my_thread_mgr_init(THREAD_START_FUNC start_func, void* parameter)
{
    g_thread_function = start_func;
    g_thread_param = parameter;
    return my_mem_shim_malloc(1);
}

static int my_thread_mgr_detach(THREAD_MGR_HANDLE handle)
{
    my_mem_shim_free(handle);
    return 0;
}

#ifndef WIN32
static char* umocktypes_stringify_MUTEX_HANDLE(MUTEX_HANDLE* mutex)
{
    char* result;
    result = my_mem_shim_malloc(32);
    sprintf(result, "mutex: %d", (int)mutex->__align);
    return result;
}

static int umocktypes_are_equal_MUTEX_HANDLE(MUTEX_HANDLE* left, MUTEX_HANDLE* right)
{
    int result;
    if (left == NULL || right == NULL)
    {
        result = -1;
    }
    else
    {
        result = (left->__align == right->__align);
    }
    return result;
}

static int umocktypes_copy_MUTEX_HANDLE(MUTEX_HANDLE* destination, MUTEX_HANDLE* source)
{
    destination->__align = source->__align;
}

static void umocktypes_free_MUTEX_HANDLE(MUTEX_HANDLE* value)
{
    //my_mem_shim_free(value);
}

static char* umocktypes_stringify_SIGNAL_HANDLE(SIGNAL_HANDLE* signal)
{
    char* result;
    result = my_mem_shim_malloc(32);
    sprintf(result, "signal: %d", (int)signal->__align);
    return result;
}

static int umocktypes_are_equal_SIGNAL_HANDLE(SIGNAL_HANDLE* left, SIGNAL_HANDLE* right)
{
    int result;
    if (left == NULL || right == NULL)
    {
        result = -1;
    }
    else
    {
        result = (left->__align == right->__align);
    }
    return result;
}

static int umocktypes_copy_SIGNAL_HANDLE(SIGNAL_HANDLE* destination, SIGNAL_HANDLE* source)
{
    destination->__align = source->__align;
}

static void umocktypes_free_SIGNAL_HANDLE(SIGNAL_HANDLE* value)
{
    //my_mem_shim_free(value);
}
#endif

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(threadpool_ut)

CTEST_SUITE_INITIALIZE()
{
    int result;

    (void)umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_UMOCK_ALIAS_TYPE(THREAD_START_FUNC, void*);
    REGISTER_UMOCK_ALIAS_TYPE(THREAD_MGR_HANDLE, void*);
#ifdef WIN32
    REGISTER_UMOCK_ALIAS_TYPE(MUTEX_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(SIGNAL_HANDLE, void*);
#else
    REGISTER_TYPE(MUTEX_HANDLE, MUTEX_HANDLE);
    REGISTER_TYPE(SIGNAL_HANDLE, SIGNAL_HANDLE);
#endif

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_realloc, my_mem_shim_realloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_realloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);

    REGISTER_GLOBAL_MOCK_HOOK(mutex_mgr_create, my_mutex_mgr_create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mutex_mgr_create, __LINE__);
    REGISTER_GLOBAL_MOCK_HOOK(mutex_mgr_destroy, my_mutex_mgr_destroy);
    REGISTER_GLOBAL_MOCK_HOOK(mutex_mgr_lock, my_mutex_mgr_lock);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mutex_mgr_lock, __LINE__);
    REGISTER_GLOBAL_MOCK_HOOK(mutex_mgr_unlock, my_mutex_mgr_unlock);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mutex_mgr_unlock, __LINE__);

    REGISTER_GLOBAL_MOCK_HOOK(condition_mgr_init, my_condition_mgr_init);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(condition_mgr_init, __LINE__);
    REGISTER_GLOBAL_MOCK_HOOK(condition_mgr_deinit, my_condition_mgr_deinit);

    REGISTER_GLOBAL_MOCK_HOOK(thread_mgr_init, my_thread_mgr_init);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(thread_mgr_init, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(thread_mgr_detach, my_thread_mgr_detach);
    REGISTER_GLOBAL_MOCK_RETURN(thread_mgr_get_num_proc, THREADPOOL_DEFAULT_INIT_NUM);
}

CTEST_SUITE_CLEANUP()
{
    umock_c_deinit();
}

CTEST_FUNCTION_INITIALIZE()
{
    umock_c_reset_all_calls();
    g_thread_function = NULL;
    g_thread_param = NULL;

}

CTEST_FUNCTION_CLEANUP()
{
}

CTEST_FUNCTION(threadpool_create_succeed)
{
    //arrange
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(mutex_mgr_create(IGNORED_ARG));
    STRICT_EXPECTED_CALL(condition_mgr_init(IGNORED_ARG));
    for (size_t index = 0; index < THREADPOOL_INIT_NUM; index++)
    {
        STRICT_EXPECTED_CALL(thread_mgr_init(IGNORED_ARG, IGNORED_ARG));
        STRICT_EXPECTED_CALL(thread_mgr_detach(IGNORED_ARG));
    }

    //act
    THREADPOOL_HANDLE result = threadpool_create(THREADPOOL_INIT_NUM);

    //assert
    CTEST_ASSERT_IS_NOT_NULL(result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    threadpool_destroy(result);
}

CTEST_FUNCTION(threadpool_create_fail)
{
    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    //arrange
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(mutex_mgr_create(IGNORED_ARG));
    STRICT_EXPECTED_CALL(condition_mgr_init(IGNORED_ARG));

    STRICT_EXPECTED_CALL(thread_mgr_init(IGNORED_ARG, IGNORED_ARG));
    STRICT_EXPECTED_CALL(thread_mgr_detach(IGNORED_ARG)).CallCannotFail();

    umock_c_negative_tests_snapshot();

    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (umock_c_negative_tests_can_call_fail(index))
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(index);

            //act
            THREADPOOL_HANDLE result = threadpool_create(THREADPOOL_INIT_NUM);

            //assert
            CTEST_ASSERT_IS_NULL(result);
        }
    }

    //cleanup
    umock_c_negative_tests_deinit();
}

CTEST_FUNCTION(threadpool_create_min_succeed)
{
    //arrange
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(thread_mgr_get_num_proc());
    STRICT_EXPECTED_CALL(mutex_mgr_create(IGNORED_ARG));
    STRICT_EXPECTED_CALL(condition_mgr_init(IGNORED_ARG));
    for (size_t index = 0; index < THREADPOOL_DEFAULT_INIT_NUM; index++)
    {
        STRICT_EXPECTED_CALL(thread_mgr_init(IGNORED_ARG, IGNORED_ARG));
        STRICT_EXPECTED_CALL(thread_mgr_detach(IGNORED_ARG));
    }

    //act
    THREADPOOL_HANDLE result = threadpool_create(THREADPOOL_MIN_INIT_NUM);

    //assert
    CTEST_ASSERT_IS_NOT_NULL(result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    threadpool_destroy(result);
}

CTEST_FUNCTION(threadpool_destroy_succeed)
{
    //arrange
    THREADPOOL_HANDLE result = threadpool_create(THREADPOOL_MIN_INIT_NUM);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(mutex_mgr_destroy(g_mutex_handle)).IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(condition_mgr_deinit(g_signal_handle)).IgnoreArgument_signal_item();
    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    //act
    threadpool_destroy(result);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(threadpool_destroy_handle_NULL_succeed)
{
    //arrange

    //act
    threadpool_destroy(NULL);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(threadpool_initiate_work_succeed)
{
    //arrange
    THREADPOOL_HANDLE handle = threadpool_create(THREADPOOL_MIN_INIT_NUM);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(mutex_mgr_lock(g_mutex_handle)).IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(condition_mgr_broadcast(g_signal_handle)).IgnoreArgument_signal_item();
    STRICT_EXPECTED_CALL(mutex_mgr_unlock(g_mutex_handle)).IgnoreArgument_handle();

    //act
    int result = threadpool_initiate_work(handle, test_thread_start_func, g_thread_start_param);

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    threadpool_destroy(handle);
}

CTEST_FUNCTION(threadpool_initiate_work_start_func_NULL_fail)
{
    //arrange
    THREADPOOL_HANDLE handle = threadpool_create(THREADPOOL_MIN_INIT_NUM);
    umock_c_reset_all_calls();

    //act
    int result = threadpool_initiate_work(handle, NULL, g_thread_start_param);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    threadpool_destroy(handle);
}

CTEST_FUNCTION(threadpool_initiate_work_handle_NULL_fail)
{
    //arrange

    //act
    int result = threadpool_initiate_work(NULL, test_thread_start_func, g_thread_start_param);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(threadpool_initiate_work_fail)
{
    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    //arrange
    THREADPOOL_HANDLE handle = threadpool_create(THREADPOOL_MIN_INIT_NUM);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(mutex_mgr_lock(g_mutex_handle)).IgnoreArgument_handle().CallCannotFail();
    STRICT_EXPECTED_CALL(condition_mgr_broadcast(g_signal_handle)).IgnoreArgument_signal_item().CallCannotFail();
    STRICT_EXPECTED_CALL(mutex_mgr_unlock(g_mutex_handle)).IgnoreArgument_handle().CallCannotFail();

    umock_c_negative_tests_snapshot();

    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (umock_c_negative_tests_can_call_fail(index))
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(index);

            //act
            int result = threadpool_initiate_work(handle, test_thread_start_func, g_thread_start_param);

            //assert
            CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        }
    }

    //cleanup
    threadpool_destroy(handle);
    umock_c_negative_tests_deinit();
}

CTEST_END_TEST_SUITE(threadpool_ut)
