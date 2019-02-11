// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

void* my_gballoc_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#ifdef __cplusplus
#include <cstddef>
#include <ctime>
#else
#include <stddef.h>
#include <time.h>
#endif

/**
 * Include the test tools.
 */
#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umock_c_negative_tests.h"

#define ENABLE_MOCKS
//#include "azure_c_shared_utility/gballoc.h"

MOCKABLE_FUNCTION(, void, item_destroy_callback, void*, user_ctx, void*, item);
#undef ENABLE_MOCKS

#include "lib-util-c/item_list.h"

static unsigned char TEST_ITEM_1[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
static size_t TEST_ITEM_1_SIZE = 5;

static void my_item_destroy_cb(void* user_ctx, void* item)
{

}

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

static TEST_MUTEX_HANDLE g_testByTest;

BEGIN_TEST_SUITE(item_list_ut)

    TEST_SUITE_INITIALIZE(a)
    {
        int result;
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        (void)umock_c_init(on_umock_c_error);

        result = umocktypes_charptr_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);

        REGISTER_UMOCK_ALIAS_TYPE(ITEM_LIST_HANDLE, void*);

        //REGISTER_GLOBAL_MOCK_HOOK(item_destroy_callback, my_item_destroy_cb);
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

    TEST_FUNCTION(item_list_create_succeed)
    {
        // arrange
        ITEM_LIST_HANDLE result;

        // act
        result = item_list_create(my_item_destroy_cb, NULL);

        // assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(int, 0, item_list_item_count(result));
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
        ITEM_LIST_HANDLE handle = item_list_create(my_item_destroy_cb, NULL);

        // act
        item_list_destroy(handle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
    }

    TEST_FUNCTION(item_list_add_item_1_item_succeed)
    {
        // arrange
        ITEM_LIST_HANDLE handle = item_list_create(my_item_destroy_cb, NULL);

        // act
        int result = item_list_add_item(handle, TEST_ITEM_1);

        // assert
        ASSERT_ARE_EQUAL(int, 0, result);
        ASSERT_ARE_EQUAL(int, 1, item_list_item_count(handle));
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        item_list_destroy(handle);
    }

    TEST_FUNCTION(item_list_add_item_10_item_succeed)
    {
        // arrange
        ITEM_LIST_HANDLE handle = item_list_create(my_item_destroy_cb, NULL);
        size_t item_count = 10;

        // act
        for (size_t index = 0; index < item_count; index++)
        {
            int result = item_list_add_item(handle, TEST_ITEM_1);
            ASSERT_ARE_EQUAL(int, 0, result);
        }

        // assert
        ASSERT_ARE_EQUAL(int, item_count, item_list_item_count(handle));
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        item_list_destroy(handle);
    }

END_TEST_SUITE(item_list_ut)
