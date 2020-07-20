// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstddef>
#include <cstdlib>
#include <ctime>
#else
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#endif

#include <pthread.h>
#include <errno.h>

static void* my_mem_shim_malloc(size_t size)
{
    return malloc(size);
}

static void my_mem_shim_free(void* ptr)
{
    free(ptr);
}

// Include the test tools.
#include "ctest.h"
#include "azure_macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_charptr.h"
#include "umock_c/umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "lib-util-c/sys_debug_shim.h"

MOCKABLE_FUNCTION(, int, pthread_cond_init, pthread_cond_t*, cond, const pthread_condattr_t*, __cond_attr);
MOCKABLE_FUNCTION(, int, pthread_cond_destroy, pthread_cond_t*, cond);
MOCKABLE_FUNCTION(, int, pthread_cond_signal, pthread_cond_t*, cond);
MOCKABLE_FUNCTION(, int, pthread_cond_broadcast, pthread_cond_t*, cond);
MOCKABLE_FUNCTION(, int, pthread_cond_wait, pthread_cond_t*, cond, pthread_mutex_t*, __mutex);
MOCKABLE_FUNCTION(, int, pthread_cond_timedwait, pthread_cond_t*, cond, pthread_mutex_t*, __mutex, const struct timespec*, __abstime);

#undef ENABLE_MOCKS

#include "lib-util-c/condition_mgr.h"

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(condition_mgr_posix_ut)

CTEST_SUITE_INITIALIZE()
{
    int result;

    (void)umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);

    REGISTER_GLOBAL_MOCK_RETURN(pthread_cond_init, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_cond_init, EINVAL);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_cond_destroy, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_cond_destroy, EBUSY);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_cond_wait, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_cond_wait, EPERM);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_cond_timedwait, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_cond_timedwait, ETIMEDOUT);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_cond_wait, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_cond_wait, ETIMEDOUT);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_cond_signal, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_cond_signal, EINVAL);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_cond_broadcast, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_cond_broadcast, EINVAL);
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

CTEST_FUNCTION(condition_mgr_init_handle_NULL_fail)
{
    // arrange

    // act
    int result = condition_mgr_init(NULL);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(condition_mgr_init_success)
{
    SIGNAL_HANDLE handle;

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_init(IGNORED_ARG, IGNORED_ARG));

    // act
    int result = condition_mgr_init(&handle);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(condition_mgr_init_fail)
{
    SIGNAL_HANDLE handle;

    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_init(IGNORED_ARG, IGNORED_ARG));

    umock_c_negative_tests_snapshot();

    // act
    umock_c_negative_tests_reset();
    umock_c_negative_tests_fail_call(0);

    int result = condition_mgr_init(&handle);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    umock_c_negative_tests_deinit();
}

CTEST_FUNCTION(condition_mgr_deinit_success)
{
    SIGNAL_HANDLE handle;

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_destroy(IGNORED_ARG));

    // act
    condition_mgr_deinit(handle);

    // assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(condition_mgr_signal_success)
{
    SIGNAL_HANDLE handle;

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_signal(IGNORED_ARG));

    // act
    int result = condition_mgr_signal(handle);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(condition_mgr_signal_fail)
{
    SIGNAL_HANDLE handle;

    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_signal(IGNORED_ARG));

    umock_c_negative_tests_snapshot();

    // act
    umock_c_negative_tests_reset();
    umock_c_negative_tests_fail_call(0);

    int result = condition_mgr_signal(handle);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    umock_c_negative_tests_deinit();
}

CTEST_FUNCTION(condition_mgr_wait_success)
{
    SIGNAL_HANDLE handle;
    MUTEX_HANDLE mutex;

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_wait(IGNORED_ARG, IGNORED_ARG));

    // act
    int result = condition_mgr_wait(handle, mutex);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(condition_mgr_wait_fail)
{
    SIGNAL_HANDLE handle;
    MUTEX_HANDLE mutex;

    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_wait(IGNORED_ARG, IGNORED_ARG));

    umock_c_negative_tests_snapshot();

    // act
    umock_c_negative_tests_reset();
    umock_c_negative_tests_fail_call(0);

    int result = condition_mgr_wait(handle, mutex);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    umock_c_negative_tests_deinit();
}

CTEST_FUNCTION(condition_mgr_timed_wait_success)
{
    SIGNAL_HANDLE handle;
    MUTEX_HANDLE mutex;
    struct timespec abstime;

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_timedwait(IGNORED_ARG, IGNORED_ARG, IGNORED_ARG));

    // act
    int result = condition_mgr_timed_wait(handle, mutex, &abstime);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(condition_mgr_timed_wait_fail)
{
    SIGNAL_HANDLE handle;
    MUTEX_HANDLE mutex;
    struct timespec abstime;

    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_timedwait(IGNORED_ARG, IGNORED_ARG, IGNORED_ARG));

    umock_c_negative_tests_snapshot();

    // act
    umock_c_negative_tests_reset();
    umock_c_negative_tests_fail_call(0);

    int result = condition_mgr_timed_wait(handle, mutex, &abstime);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    umock_c_negative_tests_deinit();
}

CTEST_FUNCTION(condition_mgr_broadcast_success)
{
    SIGNAL_HANDLE handle;

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_broadcast(IGNORED_ARG));

    // act
    int result = condition_mgr_broadcast(handle);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(condition_mgr_broadcast_fail)
{
    SIGNAL_HANDLE handle;

    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    // arrange
    STRICT_EXPECTED_CALL(pthread_cond_broadcast(IGNORED_ARG));

    umock_c_negative_tests_snapshot();

    // act
    umock_c_negative_tests_reset();
    umock_c_negative_tests_fail_call(0);

    int result = condition_mgr_broadcast(handle);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    umock_c_negative_tests_deinit();
}

CTEST_END_TEST_SUITE(condition_mgr_posix_ut)
