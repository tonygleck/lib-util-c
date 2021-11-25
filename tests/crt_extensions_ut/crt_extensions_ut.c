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
#include "macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_charptr.h"
#include "umock_c/umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "lib-util-c/crt_extensions.h"

static const char* TEST_SOURCE_STRING = "source_string";
static const char* TEST_SOURCE_STRING_FMT = "clone_string_with_format%d";
static size_t TEST_SOURCE_STRING_LEN = 7;

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(crt_extensions_ut)

CTEST_SUITE_INITIALIZE()
{
    int result;

    (void)umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);
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

CTEST_FUNCTION(clone_string_target_NULL_fail)
{
    // arrange

    // act
    int result = clone_string(NULL, TEST_SOURCE_STRING);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_source_NULL_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string(&target, NULL);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_succeed)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    // act
    int result = clone_string(&target, TEST_SOURCE_STRING);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, TEST_SOURCE_STRING, target);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(target);
}

CTEST_FUNCTION(clone_string_fail)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG)).SetReturn(NULL);

    // act
    int result = clone_string(&target, TEST_SOURCE_STRING);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_size_source_NULL_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string_with_size(&target, NULL, 1);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_size_target_NULL_fail)
{
    // arrange

    // act
    int result = clone_string_with_size(NULL, TEST_SOURCE_STRING, TEST_SOURCE_STRING_LEN);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_size_size_0_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string_with_size(&target, TEST_SOURCE_STRING, 0);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_size_success)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    // act
    int result = clone_string_with_size(&target, TEST_SOURCE_STRING, TEST_SOURCE_STRING_LEN);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(target);
}

CTEST_FUNCTION(clone_string_with_size_fail)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG)).SetReturn(NULL);

    // act
    int result = clone_string_with_size(&target, TEST_SOURCE_STRING, TEST_SOURCE_STRING_LEN);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_format_target_NULL_fail)
{
    // arrange

    // act
    int result = clone_string_with_format(NULL, TEST_SOURCE_STRING_FMT, 42);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_format_format_NULL_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string_with_format(&target, NULL, 42);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_format_no_len_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string_with_format(&target, "", 42);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_format_success)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    // act
    int result = clone_string_with_format(&target, TEST_SOURCE_STRING_FMT, 42);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, "clone_string_with_format42", target);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(target);
}

CTEST_FUNCTION(clone_string_with_format_fail)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG)).SetReturn(NULL);

    // act
    int result = clone_string_with_format(&target, TEST_SOURCE_STRING_FMT, 42);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(target);
}

CTEST_FUNCTION(clone_string_with_size_format_target_NULL_fail)
{
    // arrange

    // act
    int result = clone_string_with_size_format(NULL, TEST_SOURCE_STRING, TEST_SOURCE_STRING_LEN, TEST_SOURCE_STRING_FMT, 7);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_size_format_source_NULL_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string_with_size_format(&target, NULL, TEST_SOURCE_STRING_LEN, TEST_SOURCE_STRING_FMT, 7);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(clone_string_with_size_format_success)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    // act
    int result = clone_string_with_size_format(&target, TEST_SOURCE_STRING, TEST_SOURCE_STRING_LEN, TEST_SOURCE_STRING_FMT, 7);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, "source_clone_string_with_format7", target);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(target);
}

// CTEST_FUNCTION(get_time_value_success)
// {
//     // arrange

//     // act
//     struct tm* result = get_time_value();

//     // assert
//     CTEST_ASSERT_IS_NOT_NULL(result);
//     CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

//     // cleanup
// }

CTEST_END_TEST_SUITE(crt_extensions_ut)
