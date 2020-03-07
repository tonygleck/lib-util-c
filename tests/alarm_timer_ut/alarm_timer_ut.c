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

// Include the test tools.
#include "testrunnerswitcher.h"
#include "azure_macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_charptr.h"
#include "umock_c/umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "lib-util-c/alarm_timer.h"

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

static TEST_MUTEX_HANDLE g_testByTest;

static void sleep_for_now(unsigned int milliseconds)
{
#ifdef WIN32
    Sleep(milliseconds);
#else
    time_t seconds = milliseconds / 1000;
    long nsRemainder = (milliseconds % 1000) * 1000000;
    struct timespec timeToSleep = { seconds, nsRemainder };
    (void)nanosleep(&timeToSleep, NULL);
#endif
}

BEGIN_TEST_SUITE(alarm_timer_ut)

TEST_SUITE_INITIALIZE(a)
{
    int result;
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    (void)umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_UMOCK_ALIAS_TYPE(ALARM_TIMER_HANDLE, void*);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(g_testByTest);
}

TEST_FUNCTION_INITIALIZE(method_init)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest))
    {
        ASSERT_FAIL("Could not acquire test serialization mutex.");
    }

    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(method_cleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

TEST_FUNCTION(alarm_timer_init_succeed)
{
    // arrange
    ALARM_TIMER_INFO timer_info = {0};

    // act
    int result = alarm_timer_init(&timer_info);

    // assert
    ASSERT_ARE_EQUAL(int, result, 0);
    ASSERT_ARE_EQUAL(size_t, 0, timer_info.expire_sec);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(alarm_timer_init_time_info_NULL_fail)
{
    // arrange

    // act
    int result = alarm_timer_init(NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, result, 0);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(alarm_timer_start_handle_NULL_succeed)
{
    // arrange
    size_t expire_time = 10;
    umock_c_reset_all_calls();

    // act
    int result = alarm_timer_start(NULL, expire_time);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(alarm_timer_start_succeed)
{
    // arrange
    size_t expire_time = 10;
    ALARM_TIMER_HANDLE handle;
    ALARM_TIMER_INFO timer_info;
    alarm_timer_init(&timer_info);
    handle = &timer_info;
    umock_c_reset_all_calls();

    // act
    int result = alarm_timer_start(handle, expire_time);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(alarm_timer_reset_handle_NULL_succeed)
{
    // arrange

    // act
    alarm_timer_reset(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(alarm_timer_reset_succeed)
{
    // arrange
    size_t expire_time = 1;
    ALARM_TIMER_HANDLE handle;
    ALARM_TIMER_INFO timer_info;
    alarm_timer_init(&timer_info);
    handle = &timer_info;
    alarm_timer_start(handle, expire_time);
    umock_c_reset_all_calls();

    // act
    sleep_for_now((expire_time*2)*500);
    alarm_timer_reset(handle);
    bool result = alarm_timer_is_expired(handle);

    // assert
    ASSERT_IS_FALSE(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(alarm_timer_is_expired_false_succeed)
{
    // arrange
    size_t expire_time = 10;
    ALARM_TIMER_HANDLE handle;
    ALARM_TIMER_INFO timer_info;
    alarm_timer_init(&timer_info);
    handle = &timer_info;
    alarm_timer_start(handle, expire_time);
    umock_c_reset_all_calls();

    // act
    bool result = alarm_timer_is_expired(handle);

    // assert
    ASSERT_IS_FALSE(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(alarm_timer_is_expired_true_succeed)
{
    // arrange
    size_t expire_time = 1;
    ALARM_TIMER_HANDLE handle;
    ALARM_TIMER_INFO timer_info;
    (void)alarm_timer_init(&timer_info);
    handle = &timer_info;
    alarm_timer_start(handle, expire_time);
    umock_c_reset_all_calls();

    // act
    sleep_for_now((expire_time*3)*1000);
    bool result = alarm_timer_is_expired(handle);

    // assert
    ASSERT_IS_TRUE(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(alarm_timer_is_expired_NULL_fail)
{
    // arrange

    // act
    bool result = alarm_timer_is_expired(NULL);

    // assert
    ASSERT_IS_TRUE(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

END_TEST_SUITE(alarm_timer_ut)
