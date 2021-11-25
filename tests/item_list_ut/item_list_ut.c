// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstddef>
#else
#include <stdlib.h>
#include <stddef.h>
#endif

#include "ctest.h"
#include "macro_utils/macro_utils.h"

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
    (void)user_ctx;
    (void)item;
}

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(item_list_ut)

CTEST_SUITE_INITIALIZE()
{
    umock_c_init(on_umock_c_error);

    REGISTER_UMOCK_ALIAS_TYPE(ITEM_LIST_HANDLE, void*);

    REGISTER_GLOBAL_MOCK_HOOK(item_destroy_callback, my_item_destroy_cb);

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

CTEST_FUNCTION(item_list_create_succeed)
{
    // arrange
    ITEM_LIST_HANDLE result;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    // act
    result = item_list_create(item_destroy_callback, NULL);

    // assert
    CTEST_ASSERT_IS_NOT_NULL(result);
    CTEST_ASSERT_ARE_EQUAL(int, 0, item_list_item_count(result));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(result);
}

CTEST_FUNCTION(item_list_create_fail)
{
    // arrange
    ITEM_LIST_HANDLE result;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG)).SetReturn(NULL);

    // act
    result = item_list_create(item_destroy_callback, NULL);

    // assert
    CTEST_ASSERT_IS_NULL(result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_destroy_handle_NULL_succeed)
{
    // arrange

    // act
    item_list_destroy(NULL);

    // assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_destroy_no_items_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    // act
    item_list_destroy(handle);

    // assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_add_item_handle_NULL_fail)
{
    // arrange

    // act
    int result = item_list_add_item(NULL, TEST_ITEM_1);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_add_item_1_item_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    // act
    int result = item_list_add_item(handle, TEST_ITEM_1);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(int, 1, item_list_item_count(handle));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_add_item_multiple_item_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    size_t item_count = sizeof(TEST_ARRAY)/sizeof(TEST_ARRAY[0]);

    // act
    for (size_t index = 0; index < item_count; index++)
    {
        STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

        int result = item_list_add_item(handle, TEST_ARRAY[index]);
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    }

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, item_count, item_list_item_count(handle));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_add_copy_handle_NULL_fail)
{
    // arrange
    int result = item_list_add_copy(NULL, TEST_ITEM_1, TEST_ITEM_SIZE);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_add_copy_fail)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    int negativeTestsInitResult = umock_c_negative_tests_init();
    CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    umock_c_negative_tests_snapshot();

    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(index);

        int result = item_list_add_copy(handle, TEST_ITEM_1, TEST_ITEM_SIZE);

        // assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    }

    // cleanup
    umock_c_negative_tests_deinit();
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_add_copy_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    int result = item_list_add_copy(handle, TEST_ITEM_1, TEST_ITEM_SIZE);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(int, 1, item_list_item_count(handle));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_remove_item_handle_NULL_succeed)
{
    // arrange
    int result = item_list_remove_item(NULL, 0);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_remove_item_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_item(handle, TEST_ITEM_1);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(item_destroy_callback(IGNORED_ARG, IGNORED_ARG));
    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    int result = item_list_remove_item(handle, 0);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(int, 0, item_list_item_count(handle));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_remove_item_2_items_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_copy(handle, TEST_ITEM_1, TEST_ITEM_SIZE);
    item_list_add_copy(handle, TEST_ITEM_2, TEST_ITEM_SIZE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(free(IGNORED_ARG));
    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    int result = item_list_remove_item(handle, 1);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(int, 1, item_list_item_count(handle));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_remove_item_3_items_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_copy(handle, TEST_ITEM_1, TEST_ITEM_SIZE);
    item_list_add_copy(handle, TEST_ITEM_2, TEST_ITEM_SIZE);
    item_list_add_copy(handle, TEST_ITEM_3, TEST_ITEM_SIZE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(free(IGNORED_ARG));
    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    int result = item_list_remove_item(handle, 2);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(int, 2, item_list_item_count(handle));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_remove_item_index_too_large_fail)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_item(handle, TEST_ITEM_1);
    umock_c_reset_all_calls();

    int result = item_list_remove_item(handle, 2);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(int, 1, item_list_item_count(handle));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_item_count_handle_NULL_fail)
{
    // arrange
    size_t result = item_list_item_count(NULL);

    // assert
    CTEST_ASSERT_ARE_EQUAL(size_t, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_get_item_handle_NULL_fail)
{
    // arrange
    const void* result = item_list_get_item(NULL, 2);

    // assert
    CTEST_ASSERT_IS_NULL(result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_get_item_success)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    size_t item_count = sizeof(TEST_ARRAY)/sizeof(TEST_ARRAY[0]);
    for (size_t index = 0; index < item_count; index++)
    {
        item_list_add_item(handle, TEST_ARRAY[index]);
    }
    umock_c_reset_all_calls();

    const void* result = item_list_get_item(handle, 2);

    // assert
    CTEST_ASSERT_IS_NOT_NULL(result);
    CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(TEST_ARRAY[2], result, TEST_ITEM_SIZE) );
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_get_item_index_out_of_range_fail)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    size_t item_count = sizeof(TEST_ARRAY)/sizeof(TEST_ARRAY[0]);
    for (size_t index = 0; index < item_count; index++)
    {
        item_list_add_item(handle, TEST_ARRAY[index]);
    }
    umock_c_reset_all_calls();

    const void* result = item_list_get_item(handle, 10);

    // assert
    CTEST_ASSERT_IS_NULL(result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_clear_handle_NULL_fail)
{
    // arrange

    int result = item_list_clear(NULL);

    // assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_clear_handle_success)
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
        STRICT_EXPECTED_CALL(item_destroy_callback(IGNORED_ARG, IGNORED_ARG));
        STRICT_EXPECTED_CALL(free(IGNORED_ARG));
    }
    int result = item_list_clear(handle);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_get_front_handle_NULL_fail)
{
    // arrange

    const unsigned char* front_item = item_list_get_front(NULL);

    // assert
    CTEST_ASSERT_IS_NULL(front_item);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_get_front_no_items_fail)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    const unsigned char* front_item = item_list_get_front(handle);

    // assert
    CTEST_ASSERT_IS_NULL(front_item);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_get_front_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_item(handle, TEST_ITEM_1);
    item_list_add_item(handle, TEST_ITEM_2);
    umock_c_reset_all_calls();

    const unsigned char* front_item = item_list_get_front(handle);

    // assert
    CTEST_ASSERT_IS_NOT_NULL(front_item);
    CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(front_item, TEST_ITEM_1, TEST_ITEM_SIZE));
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_iterator_handle_NULL_fail)
{
    // arrange

    ITERATOR_HANDLE iterator = item_list_iterator(NULL);
    CTEST_ASSERT_IS_NULL(iterator);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_iterator_no_items_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    umock_c_reset_all_calls();

    ITERATOR_HANDLE iterator = item_list_iterator(handle);
    CTEST_ASSERT_IS_NULL(iterator);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_iterator_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_item(handle, TEST_ITEM_1);
    item_list_add_item(handle, TEST_ITEM_2);
    umock_c_reset_all_calls();

    ITERATOR_HANDLE iterator = item_list_iterator(handle);

    // assert
    CTEST_ASSERT_IS_NOT_NULL(iterator);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_FUNCTION(item_list_get_next_handle_NULL_succeed)
{
    // arrange
    ITERATOR_HANDLE iterator;

    const unsigned char* item = item_list_get_next(NULL, &iterator);

    // assert
    CTEST_ASSERT_IS_NULL(item);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(item_list_get_next_succeed)
{
    // arrange
    ITEM_LIST_HANDLE handle = item_list_create(item_destroy_callback, NULL);
    item_list_add_item(handle, TEST_ITEM_1);
    item_list_add_item(handle, TEST_ITEM_2);
    ITERATOR_HANDLE iterator = item_list_iterator(handle);
    umock_c_reset_all_calls();

    const unsigned char* item = item_list_get_next(handle, &iterator);
    CTEST_ASSERT_IS_NOT_NULL(item);
    CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(item, TEST_ITEM_1, TEST_ITEM_SIZE));

    item = item_list_get_next(handle, &iterator);
    CTEST_ASSERT_IS_NOT_NULL(item);
    CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(item, TEST_ITEM_2, TEST_ITEM_SIZE));

    item = item_list_get_next(handle, &iterator);
    CTEST_ASSERT_IS_NULL(item);

    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    item_list_destroy(handle);
}

CTEST_END_TEST_SUITE(item_list_ut)
