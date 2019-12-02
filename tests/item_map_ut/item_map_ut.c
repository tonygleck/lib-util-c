// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstddef>
#else
#include <stdlib.h>
#include <stddef.h>
#endif

#include "testrunnerswitcher.h"
#include "azure_macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"
#include "umock_c/umock_c_negative_tests.h"
#include "umock_c/umocktypes_charptr.h"

static void* my_mem_shim_malloc(size_t size)
{
    return malloc(size);
}

static void my_mem_shim_free(void* ptr)
{
    free(ptr);
}

#define ENABLE_MOCKS
#include "umock_c/umock_c_prod.h"
#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/crt_extensions.h"

MOCKABLE_FUNCTION(, void, map_destroy_callback, void*, user_ctx, const char*, key, void*, remove_value);

#undef ENABLE_MOCKS

#include "lib-util-c/item_map.h"

static unsigned char TEST_ITEM_1[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
static unsigned char TEST_ITEM_2[] = { 0x02, 0x03, 0x04, 0x05, 0x06 };
static unsigned char TEST_ITEM_3[] = { 0x03, 0x04, 0x05, 0x06, 0x07 };
static unsigned char TEST_ITEM_4[] = { 0x04, 0x05, 0x06, 0x07, 0x08 };
static unsigned char TEST_ITEM_5[] = { 0x05, 0x06, 0x07, 0x08, 0x09 };

static unsigned char* TEST_ARRAY[] = { TEST_ITEM_1, TEST_ITEM_2, TEST_ITEM_3, TEST_ITEM_4, TEST_ITEM_5  };
static size_t TEST_ITEM_SIZE = 5;

static void my_map_destroy_cb(void* user_ctx, const char* key, void* remove_value)
{
}

static int my_clone_string(char** target, const char* source)
{
    size_t len = strlen(source);
    *target = my_mem_shim_malloc(len+1);
    strcpy(*target, source);
    return 0;
}

static TEST_MUTEX_HANDLE test_serialize_mutex;
MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

BEGIN_TEST_SUITE(item_map_ut)

TEST_SUITE_INITIALIZE(suite_init)
{
    test_serialize_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_serialize_mutex);

    umock_c_init(on_umock_c_error);

    REGISTER_UMOCK_ALIAS_TYPE(ITEM_MAP_HANDLE, void*);

    REGISTER_GLOBAL_MOCK_HOOK(map_destroy_callback, my_map_destroy_cb);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);

    REGISTER_GLOBAL_MOCK_HOOK(clone_string, my_clone_string);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(clone_string, __LINE__);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();
    TEST_MUTEX_DESTROY(test_serialize_mutex);
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

static void setup_item_map_add_item_mocks(void)
{
    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(clone_string(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));
}

TEST_FUNCTION(item_map_create_succeed)
{
    // arrange
    ITEM_MAP_HANDLE result;

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));

    // act
    result = item_map_create(10, map_destroy_callback, NULL, NULL);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(result);
}

TEST_FUNCTION(item_map_create_destroy_cb_NULL_succeed)
{
    // arrange
    ITEM_MAP_HANDLE result;

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));

    // act
    result = item_map_create(20, NULL, NULL, NULL);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(result);
}

TEST_FUNCTION(item_map_create_fail)
{
    // arrange
    ITEM_MAP_HANDLE result;

    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));

    umock_c_negative_tests_snapshot();

    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (umock_c_negative_tests_can_call_fail(index))
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(index);

            // act
            result = item_map_create(20, map_destroy_callback, NULL, NULL);

            // assert
            ASSERT_IS_NULL(result);
        }
    }

    // cleanup
    umock_c_negative_tests_deinit();
}

TEST_FUNCTION(item_map_add_item_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    umock_c_reset_all_calls();

    setup_item_map_add_item_mocks();

    // act
    int value = 22;
    int result = item_map_add_item(handle, "test_key", &value, sizeof(int));

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_add_item_fail)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);

    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);
    umock_c_reset_all_calls();

    setup_item_map_add_item_mocks();

    umock_c_negative_tests_snapshot();

    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (umock_c_negative_tests_can_call_fail(index))
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(index);

            // act
            int value = 22;
            int result = item_map_add_item(handle, "test_key", &value, sizeof(int));

            // assert
            ASSERT_ARE_NOT_EQUAL(int, 0, result);
        }
    }
    // cleanup
    item_map_destroy(handle);
    umock_c_negative_tests_deinit();
}

TEST_FUNCTION(item_map_add_item_collision_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    int value = 22;
    int result = item_map_add_item(handle, "aaaaaa", &value, sizeof(int));
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(clone_string(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));

    // act
    result = item_map_add_item(handle, "aaaba", &value, sizeof(int));

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_add_item_collision_fail)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    int value = 22;
    int result = item_map_add_item(handle, "aaaaaa", &value, sizeof(int));
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(clone_string(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG)).SetReturn(NULL);
    STRICT_EXPECTED_CALL(free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(free(IGNORED_PTR_ARG));

    // act
    result = item_map_add_item(handle, "aaaba", &value, sizeof(int));

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_get_item_handle_NULL_fail)
{
    // arrange

    // act
    const int* result = (const int*)item_map_get_item(NULL, "test_key2");

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_map_get_item_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    int value = 22;
    (void)item_map_add_item(handle, "test_key1", &value, sizeof(int));
    int value_2 = 77;
    (void)item_map_add_item(handle, "test_key2", &value_2, sizeof(int));
    umock_c_reset_all_calls();

    // act
    const int* result = (const int*)item_map_get_item(handle, "test_key2");

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(int, value_2, *result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_size_handle_NULL_fail)
{
    // arrange

    // act
    size_t result = item_map_size(NULL);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_map_size_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    int value = 22;
    (void)item_map_add_item(handle, "test_key1", &value, sizeof(int));
    int value_2 = 77;
    (void)item_map_add_item(handle, "test_key2", &value_2, sizeof(int));
    umock_c_reset_all_calls();

    // act
    size_t result = item_map_size(handle);

    // assert
    ASSERT_ARE_EQUAL(size_t, 2, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_size_no_item_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    umock_c_reset_all_calls();

    // act
    size_t result = item_map_size(handle);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_get_item_collision_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    int value = 22;
    (void)item_map_add_item(handle, "aaaaaa", &value, sizeof(int));
    int value_2 = 77;
    (void)item_map_add_item(handle, "aaaba", &value_2, sizeof(int));
    umock_c_reset_all_calls();

    // act
    const int* result = (const int*)item_map_get_item(handle, "aaaba");

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(int, value_2, *result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_remove_item_no_item_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    int value = 22;
    (void)item_map_add_item(handle, "aaaaaa", &value, sizeof(int));
    int value_2 = 77;
    (void)item_map_add_item(handle, "aaaba", &value_2, sizeof(int));
    umock_c_reset_all_calls();

    // act
    int result = item_map_remove_item(handle, "no_item");

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 2, item_map_size(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_remove_item_collision_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    int value = 22;
    (void)item_map_add_item(handle, "aaaaaa", &value, sizeof(int));
    int value_2 = 77;
    (void)item_map_add_item(handle, "aaaba", &value_2, sizeof(int));
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(free(IGNORED_PTR_ARG));

    // act
    int result = item_map_remove_item(handle, "aaaba");

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, item_map_size(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_clear_all_handle_NULL_fail)
{
    // arrange

    // act
    int result = item_map_clear_all(NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_map_clear_all_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    int value = 22;
    (void)item_map_add_item(handle, "aaaaaa", &value, sizeof(int));
    int value_2 = 77;
    (void)item_map_add_item(handle, "aaaba", &value_2, sizeof(int));
    umock_c_reset_all_calls();

    // act
    int result = item_map_clear_all(handle);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, item_map_size(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

TEST_FUNCTION(item_map_clear_all_no_items_succeed)
{
    // arrange
    ITEM_MAP_HANDLE handle = item_map_create(10, map_destroy_callback, NULL, NULL);
    umock_c_reset_all_calls();

    // act
    int result = item_map_clear_all(handle);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, item_map_size(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_map_destroy(handle);
}

END_TEST_SUITE(item_map_ut)
