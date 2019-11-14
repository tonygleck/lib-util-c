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

MOCKABLE_FUNCTION(, void, item_destroy_callback, void*, user_ctx, void*, item);
#undef ENABLE_MOCKS

#include "lib-util-c/item_list.h"

static unsigned char TEST_ITEM_1[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
static unsigned char TEST_ITEM_2[] = { 0x02, 0x03, 0x04, 0x05, 0x06 };
static unsigned char TEST_ITEM_3[] = { 0x03, 0x04, 0x05, 0x06, 0x07 };
static unsigned char TEST_ITEM_4[] = { 0x04, 0x05, 0x06, 0x07, 0x08 };
static unsigned char TEST_ITEM_5[] = { 0x05, 0x06, 0x07, 0x08, 0x09 };

static unsigned char* TEST_ARRAY[] = { TEST_ITEM_1, TEST_ITEM_2, TEST_ITEM_3, TEST_ITEM_4, TEST_ITEM_5  };
static size_t TEST_ITEM_SIZE = 5;

static void my_item_destroy_cb(void* user_ctx, void* item)
{
}

static TEST_MUTEX_HANDLE test_serialize_mutex;
MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

BEGIN_TEST_SUITE(item_list_ut)

TEST_SUITE_INITIALIZE(suite_init)
{
    test_serialize_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_serialize_mutex);

    umock_c_init(on_umock_c_error);

    REGISTER_UMOCK_ALIAS_TYPE(ITEM_LIST_HANDLE, void*);

    REGISTER_GLOBAL_MOCK_HOOK(item_destroy_callback, my_item_destroy_cb);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);
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

TEST_FUNCTION(item_list_create_succeed)
{
    // arrange
    ITEM_LIST_HANDLE result;

    // act
    result = item_list_create(item_destroy_callback, NULL);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(int, 0, item_list_item_count(result));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(result);
}

TEST_FUNCTION(item_list_create_destroy_cb_NULL_fail)
{
    // arrange
    ITEM_LIST_HANDLE result;

    // act
    result = item_list_create(NULL, NULL);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(result);
}

TEST_FUNCTION(item_list_destroy_handle_NULL_succeed)
{
    // arrange

    // act
    item_list_destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_list_destroy_no_items_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);

    // act
    item_list_destroy(handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_list_add_item_1_item_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);

    // act
    int result = item_list_add_item(handle, TEST_ITEM_1);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, item_list_item_count(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

TEST_FUNCTION(item_list_add_item_multiple_item_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    size_t item_count = sizeof(TEST_ARRAY)/sizeof(TEST_ARRAY[0]);

    // act
    for (size_t index = 0; index < item_count; index++)
    {
        int result = item_list_add_item(handle, TEST_ARRAY[index]);
        ASSERT_ARE_EQUAL(int, 0, result);
    }

    // assert
    ASSERT_ARE_EQUAL(int, item_count, item_list_item_count(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

TEST_FUNCTION(item_list_add_copy_handle_NULL_fail)
{
    // arrange
    int result = item_list_add_copy(NULL, TEST_ITEM_1, TEST_ITEM_SIZE);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_list_add_copy_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);

    int result = item_list_add_copy(handle, TEST_ITEM_1, TEST_ITEM_SIZE);
    ASSERT_ARE_EQUAL(int, 0, result);

    // assert
    ASSERT_ARE_EQUAL(int, 1, item_list_item_count(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

TEST_FUNCTION(item_list_remove_item_handle_NULL_succeed)
{
    // arrange
    int result = item_list_remove_item(NULL, 0);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_list_remove_item_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_item(handle, TEST_ITEM_1);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(item_destroy_callback(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    int result = item_list_remove_item(handle, 0);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, item_list_item_count(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

TEST_FUNCTION(item_list_remove_item_index_too_large_fail)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_item(handle, TEST_ITEM_1);
    umock_c_reset_all_calls();

    int result = item_list_remove_item(handle, 2);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 1, item_list_item_count(handle));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

TEST_FUNCTION(item_list_item_count_handle_NULL_fail)
{
    // arrange
    size_t result = item_list_item_count(NULL);

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_list_get_item_handle_NULL_fail)
{
    // arrange
    const void* result = item_list_get_item(NULL, 2);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_list_get_item_success)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    size_t item_count = sizeof(TEST_ARRAY)/sizeof(TEST_ARRAY[0]);
    for (size_t index = 0; index < item_count; index++)
    {
        item_list_add_item(handle, TEST_ARRAY[index]);
    }
    umock_c_reset_all_calls();

    const void* result = item_list_get_item(handle, 2);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(int, 0, memcmp(TEST_ARRAY[2], result, TEST_ITEM_SIZE) );
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

TEST_FUNCTION(item_list_get_item_index_out_of_range_fail)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    size_t item_count = sizeof(TEST_ARRAY)/sizeof(TEST_ARRAY[0]);
    for (size_t index = 0; index < item_count; index++)
    {
        item_list_add_item(handle, TEST_ARRAY[index]);
    }
    umock_c_reset_all_calls();

    const void* result = item_list_get_item(handle, 10);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

TEST_FUNCTION(item_list_clear_handle_NULL_fail)
{
    // arrange

    int result = item_list_clear(NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(item_list_clear_handle_success)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    size_t item_count = sizeof(TEST_ARRAY)/sizeof(TEST_ARRAY[0]);
    for (size_t index = 0; index < item_count; index++)
    {
        item_list_add_item(handle, TEST_ARRAY[index]);
    }
    umock_c_reset_all_calls();

    for (size_t index = 0; index < item_count; index++)
    {
        STRICT_EXPECTED_CALL(item_destroy_callback(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    }
    int result = item_list_clear(handle);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

END_TEST_SUITE(item_list_ut)
