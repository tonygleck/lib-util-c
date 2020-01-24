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
#include "testrunnerswitcher.h"
#include "azure_macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_charptr.h"
#include "umock_c/umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "lib-util-c/crt_extensions.h"

static const char* TEST_SOURCE_STRING = "source_string";
static size_t TEST_SOURCE_STRING_LEN = 7;

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

static TEST_MUTEX_HANDLE g_testByTest;


BEGIN_TEST_SUITE(crt_extensions_ut)

TEST_SUITE_INITIALIZE(a)
{
    int result;
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    (void)umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);
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

TEST_FUNCTION(clone_string_target_NULL_fail)
{
    // arrange

    // act
    int result = clone_string(NULL, TEST_SOURCE_STRING);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(clone_string_source_NULL_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string(&target, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(clone_string_succeed)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));

    // act
    int result = clone_string(&target, TEST_SOURCE_STRING);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, TEST_SOURCE_STRING, target);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(target);
}

TEST_FUNCTION(clone_string_fail)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG)).SetReturn(NULL);

    // act
    int result = clone_string(&target, TEST_SOURCE_STRING);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(clone_string_with_size_source_NULL_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string_with_size(&target, NULL, 1);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(clone_string_with_size_target_NULL_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string_with_size(NULL, TEST_SOURCE_STRING, TEST_SOURCE_STRING_LEN);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(clone_string_with_size_size_0_fail)
{
    // arrange
    char* target;

    // act
    int result = clone_string_with_size(&target, TEST_SOURCE_STRING, 0);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(clone_string_with_size_success)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));

    // act
    int result = clone_string_with_size(&target, TEST_SOURCE_STRING, TEST_SOURCE_STRING_LEN);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(target);
}

TEST_FUNCTION(clone_string_with_size_fail)
{
    // arrange
    char* target;

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG)).SetReturn(NULL);

    // act
    int result = clone_string_with_size(&target, TEST_SOURCE_STRING, TEST_SOURCE_STRING_LEN);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

END_TEST_SUITE(crt_extensions_ut)
