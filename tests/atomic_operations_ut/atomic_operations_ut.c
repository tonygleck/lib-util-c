// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstddef>
#include <ctime>
#else
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#endif

// Include the test tools.
#include "ctest.h"
#include "macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"

#define ENABLE_MOCKS
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "lib-util-c/atomic_operations.h"

#define TEST_PREINCREMENT_VALUE     25
#define TEST_INC_DEC_VALUE          10

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(atomic_operations_ut)

CTEST_SUITE_INITIALIZE()
{
    (void)umock_c_init(on_umock_c_error);
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

CTEST_FUNCTION(atomic_increment_succeed)
{
    // arrange
    long initial_value = TEST_PREINCREMENT_VALUE;

    // act
    long result = atomic_increment(&initial_value);

    // assert
    CTEST_ASSERT_ARE_EQUAL(long, result, TEST_PREINCREMENT_VALUE+1);

    // cleanup
}

CTEST_FUNCTION(atomic_increment_value_NULL_fail)
{
    // arrange

    // act
    long result = atomic_increment(NULL);

    // assert
    CTEST_ASSERT_ARE_EQUAL(long, result, 0);

    // cleanup
}

CTEST_FUNCTION(atomic_increment64_succeed)
{
    // arrange
    int64_t initial_value = TEST_PREINCREMENT_VALUE;

    // act
    int64_t result = atomic_increment64(&initial_value);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int64_t, result, TEST_PREINCREMENT_VALUE+1);

    // cleanup
}

CTEST_FUNCTION(atomic_increment64_value_NULL_fail)
{
    // arrange

    // act
    int64_t result = atomic_increment64(NULL);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int64_t, result, 0);

    // cleanup
}

CTEST_FUNCTION(atomic_decrement_succeed)
{
    // arrange
    long initial_value = TEST_PREINCREMENT_VALUE;

    // act
    long result = atomic_decrement(&initial_value);

    // assert
    CTEST_ASSERT_ARE_EQUAL(long, result, TEST_PREINCREMENT_VALUE-1);

    // cleanup
}

CTEST_FUNCTION(atomic_decrement_value_NULL_fail)
{
    // arrange

    // act
    long result = atomic_decrement(NULL);

    // assert
    CTEST_ASSERT_ARE_EQUAL(long, result, 0);

    // cleanup
}

CTEST_FUNCTION(atomic_decrement64_succeed)
{
    // arrange
    int64_t initial_value = TEST_PREINCREMENT_VALUE;

    // act
    int64_t result = atomic_decrement64(&initial_value);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int64_t, result, TEST_PREINCREMENT_VALUE-1);

    // cleanup
}

CTEST_FUNCTION(atomic_decrement64_value_NULL_fail)
{
    // arrange

    // act
    int64_t result = atomic_decrement64(NULL);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int64_t, result, 0);

    // cleanup
}

CTEST_FUNCTION(atomic_add_succeed)
{
    // arrange
    long initial_value = TEST_PREINCREMENT_VALUE;

    // act
    long result = atomic_add(&initial_value, TEST_INC_DEC_VALUE);

    // assert
    CTEST_ASSERT_ARE_EQUAL(long, result, TEST_PREINCREMENT_VALUE+TEST_INC_DEC_VALUE);

    // cleanup
}

CTEST_FUNCTION(atomic_add_operand_NULL_fail)
{
    // arrange

    // act
    long result = atomic_add(NULL, TEST_INC_DEC_VALUE);

    // assert
    CTEST_ASSERT_ARE_EQUAL(long, result, 0);

    // cleanup
}

CTEST_FUNCTION(atomic_subtract_succeed)
{
    // arrange
    long initial_value = TEST_PREINCREMENT_VALUE;

    // act
    long result = atomic_subtract(&initial_value, TEST_INC_DEC_VALUE);

    // assert
    CTEST_ASSERT_ARE_EQUAL(long, result, TEST_PREINCREMENT_VALUE-TEST_INC_DEC_VALUE);

    // cleanup
}

CTEST_FUNCTION(atomic_subtract_operand_NULL_fail)
{
    // arrange

    // act
    long result = atomic_subtract(NULL, TEST_INC_DEC_VALUE);

    // assert
    CTEST_ASSERT_ARE_EQUAL(long, result, 0);

    // cleanup
}

CTEST_END_TEST_SUITE(atomic_operations_ut)
