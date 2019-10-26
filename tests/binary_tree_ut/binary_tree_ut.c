// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstddef>
#else
#include <stdlib.h>
#endif

static void* my_mem_shim_malloc(size_t size)
{
    return malloc(size);
}

static void my_mem_shim_free(void* ptr)
{
    free(ptr);
}

#ifdef __cplusplus
#else
#include <stddef.h>
#endif

// Include the test tools.
#include "testrunnerswitcher.h"
#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c.h"

#define ENABLE_MOCKS
#include "umock_c/umock_c_prod.h"
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "lib-util-c/binary_tree.h"

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

static const NODE_KEY INSERT_FOR_NO_ROTATION[] = { 0xa, 0xb, 0x5, 0x7, 0xc, 0x3 };
static const char* VISUAL_NO_ROTATION = "a(5(3)(7))(b(c))";

static const NODE_KEY INSERT_FOR_NO_ROTATION_2[] = { 0xa, 0xc, 0x5, 0x7, 0xb, 0x3 };
static const char* VISUAL_NO_ROTATION_2 = "a(5(3)(7))(c(b))";

static const size_t INSERT_NO_ROTATION_HEIGHT = 3;
static const NODE_KEY INSERT_FOR_RIGHT_ROTATION[] = { 0xa, 0xb, 0x7, 0x5, 0x3 };
static const char* VISUAL_RIGHT_ROTATION = "a(5(3)(7))(b)";

static const NODE_KEY INSERT_FOR_RIGHT_LEFT_ROTATION[] = { 0x10, 0x14, 0xe, 0xa, 0xc };
static const char* VISUAL_RIGHT_LEFT_ROTATION = "10(c(a)(e))(14)";

static const NODE_KEY INSERT_FOR_LEFT_RIGHT_ROTATION[] = { 0xa, 0x6, 0xd, 0x12, 0xe };
static const char* VISUAL_LEFT_RIGHT_ROTATION = "a(6)(e(d)(12))";
static const NODE_KEY INVALID_ITEM = 0x01;

static void* DATA_VALUE = (void*)0x11;

static TEST_MUTEX_HANDLE g_testByTest;

BEGIN_TEST_SUITE(binary_tree_ut)

TEST_SUITE_INITIALIZE(suite_init)
{
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    (void)umock_c_init(on_umock_c_error);

    REGISTER_UMOCK_ALIAS_TYPE(BINARY_TREE_HANDLE, void*);

    //REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    //REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    //REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);
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

static void remove_callback(void* data)
{
    //DATA_VALUE
    (void)data;
}

static void assert_visual_check(BINARY_TREE_HANDLE handle, const char* expected)
{
    char* visual_check = binary_tree_construct_visual(handle);
    ASSERT_ARE_EQUAL(char_ptr, expected, visual_check);
    free(visual_check);
}

    TEST_FUNCTION(binary_tree_create_succeed)
    {
        //arrange

        //act
        BINARY_TREE_HANDLE result = binary_tree_create();


        //assert
        ASSERT_IS_NOT_NULL(result);

        //cleanup
        binary_tree_destroy(result);
    }

    TEST_FUNCTION(binary_tree_destroy_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE result = binary_tree_create();

        //act
        binary_tree_destroy(result);

        //assert

        //cleanup
    }

    TEST_FUNCTION(binary_tree_destroy_handle_NULL_succeed)
    {
        //arrange

        //act
        binary_tree_destroy(NULL);

        //assert

        //cleanup
    }

    TEST_FUNCTION(binary_tree_insert_handle_NULL_fail)
    {
        //arrange
        NODE_KEY insert_key = 0x4;

        //act
        int result = binary_tree_insert(NULL, insert_key, NULL);

        //assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //cleanup
    }

    TEST_FUNCTION(binary_tree_insert_succeed)
    {
        //arrange
        NODE_KEY insert_key = 0x4;
        BINARY_TREE_HANDLE handle = binary_tree_create();

        //act
        int result = binary_tree_insert(handle, insert_key, NULL);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);
        assert_visual_check(handle, "4");


        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_insert_right_rotate_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();

        //act
        size_t count = sizeof(INSERT_FOR_RIGHT_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            int result = binary_tree_insert(handle, INSERT_FOR_RIGHT_ROTATION[index], DATA_VALUE);

            //assert
            ASSERT_ARE_EQUAL(int, 0, result);
        }
        assert_visual_check(handle, VISUAL_RIGHT_ROTATION);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_insert_right_left_rotate_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();

        //act
        size_t count = sizeof(INSERT_FOR_RIGHT_LEFT_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            int result = binary_tree_insert(handle, INSERT_FOR_RIGHT_LEFT_ROTATION[index], DATA_VALUE);

            //assert
            ASSERT_ARE_EQUAL(int, 0, result);
        }
        assert_visual_check(handle, VISUAL_RIGHT_LEFT_ROTATION);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_insert_left_right_rotate_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();

        //act
        size_t count = sizeof(INSERT_FOR_LEFT_RIGHT_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            int result = binary_tree_insert(handle, INSERT_FOR_LEFT_RIGHT_ROTATION[index], DATA_VALUE);

            //assert
            ASSERT_ARE_EQUAL(int, 0, result);
        }
        assert_visual_check(handle, VISUAL_LEFT_RIGHT_ROTATION);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_insert_left_rotate_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();

        //act
        const NODE_KEY INSERT_FOR_LEFT_ROTATION[] = { 0x7, 0x5, 0xa, 0xb, 0xd };
        const char* VISUAL_LEFT_ROTATION = "7(5)(b(a)(d))";

        size_t count = sizeof(INSERT_FOR_LEFT_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            int result = binary_tree_insert(handle, INSERT_FOR_LEFT_ROTATION[index], DATA_VALUE);

            //assert
            ASSERT_ARE_EQUAL(int, 0, result);
        }
        assert_visual_check(handle, VISUAL_LEFT_ROTATION);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_insert_left_rotate_2_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();

        //act
        static const NODE_KEY INSERT_FOR_LEFT_ROTATION[] = { 0x7, 0x5, 0xa, 0xd, 0xb };
        const char* VISUAL_LEFT_ROTATION = "7(5)(b(a)(d))";

        size_t count = sizeof(INSERT_FOR_LEFT_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            int result = binary_tree_insert(handle, INSERT_FOR_LEFT_ROTATION[index], DATA_VALUE);

            //assert
            ASSERT_ARE_EQUAL(int, 0, result);
        }
        assert_visual_check(handle, VISUAL_LEFT_ROTATION);

        //cleanup
        binary_tree_destroy(handle);
    }


    TEST_FUNCTION(binary_tree_find_handle_NULL_fail)
    {
        //arrange

        //act
        void* found_item = binary_tree_find(NULL, INSERT_FOR_NO_ROTATION[0]);

        ASSERT_IS_NULL(found_item);

        //cleanup
    }

    TEST_FUNCTION(binary_tree_find_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        void* found_item = binary_tree_find(handle, INSERT_FOR_NO_ROTATION[count-1]);

        //assert
        ASSERT_IS_NOT_NULL(found_item);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_find_no_items_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();

        //act
        void* found_item = binary_tree_find(handle, INSERT_FOR_NO_ROTATION[0]);

        //assert
        ASSERT_IS_NULL(found_item);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_find_invalid_item_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        void* found_item = binary_tree_find(handle, INVALID_ITEM);

        //assert
        ASSERT_IS_NULL(found_item);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_find_invalid_item_2_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        void* found_item = binary_tree_find(handle, 0x20);

        //assert
        ASSERT_IS_NULL(found_item);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_item_count_handle_NULL_fail)
    {
        //arrange

        //act
        size_t item_count = binary_tree_item_count(NULL);

        //assert
        ASSERT_ARE_NOT_EQUAL(size_t, 0, item_count);

        //cleanup
    }

    TEST_FUNCTION(binary_tree_item_count_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        size_t item_count = binary_tree_item_count(handle);

        //assert
        ASSERT_ARE_EQUAL(size_t, count, item_count);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_height_handle_NULL_fail)
    {
        //arrange

        //act
        size_t item_count = binary_tree_height(NULL);

        //assert
        ASSERT_ARE_NOT_EQUAL(size_t, 0, item_count);

        //cleanup
    }

    TEST_FUNCTION(binary_tree_height_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        size_t item_count = binary_tree_height(handle);

        //assert
        ASSERT_ARE_EQUAL(size_t, INSERT_NO_ROTATION_HEIGHT, item_count);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_handle_NULL_fail)
    {
        //arrange

        //act
        int result = binary_tree_remove(NULL, INSERT_FOR_NO_ROTATION[0], remove_callback);

        //assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //cleanup
    }

    TEST_FUNCTION(binary_tree_remove_two_children_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, INSERT_FOR_NO_ROTATION[2], remove_callback);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);

        // Use as verification since it touches every node
        binary_tree_print(handle);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_two_children_2_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        const NODE_KEY REMOVE_TWO_CHILDREN_2[] = { 0xa, 0xf, 0x6, 0x3, 0xc, 0x12, 0x10 };

        size_t count = sizeof(REMOVE_TWO_CHILDREN_2);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, REMOVE_TWO_CHILDREN_2[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, REMOVE_TWO_CHILDREN_2[1], remove_callback);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);

        // Use as verification since it touches every node
        binary_tree_print(handle);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_one_child_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, INSERT_FOR_NO_ROTATION[1], remove_callback);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);

        // Use as verification since it touches every node
        binary_tree_print(handle);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_one_child_2_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION_2[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, INSERT_FOR_NO_ROTATION_2[1], remove_callback);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);

        // Use as verification since it touches every node
        binary_tree_print(handle);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_node_no_children_2_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        const char* VISUAL_NO_ROTATION_AFTER_REMOVE = "a(5(7))(b(c))";

        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, INSERT_FOR_NO_ROTATION[count-1], remove_callback);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);
        assert_visual_check(handle, VISUAL_NO_ROTATION_AFTER_REMOVE);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_node_no_children_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        const char* VISUAL_NO_ROTATION_AFTER_REMOVE = "a(5(3))(b(c))";

        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, INSERT_FOR_NO_ROTATION[3], remove_callback);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);
        assert_visual_check(handle, VISUAL_NO_ROTATION_AFTER_REMOVE);


        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_root_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        static const char* VISUAL_NO_ROTATION_AFTER_REMOVE = "b(5(3)(7))(c)";

        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, INSERT_FOR_NO_ROTATION[0], remove_callback);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);

        assert_visual_check(handle, VISUAL_NO_ROTATION_AFTER_REMOVE);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_root_2_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        static const char* VISUAL_NO_ROTATION_AFTER_REMOVE = "b(5(3)(7))(c)";

        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, INSERT_FOR_NO_ROTATION[0], remove_callback);

        //assert
        ASSERT_ARE_EQUAL(int, 0, result);

        assert_visual_check(handle, VISUAL_NO_ROTATION_AFTER_REMOVE);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_remove_item_not_found_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        int result = binary_tree_remove(handle, INVALID_ITEM, remove_callback);

        //assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //cleanup
        binary_tree_destroy(handle);
    }

    TEST_FUNCTION(binary_tree_construct_visual_handle_NULL_fail)
    {
        //arrange
        //act
        char* result = binary_tree_construct_visual(NULL);

        //assert
        ASSERT_IS_NULL(result);

        //cleanup
    }

    TEST_FUNCTION(binary_tree_construct_visual_no_items_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();

        //act
        char* result = binary_tree_construct_visual(handle);

        //assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, "", result);

        //cleanup
        free(result);
        binary_tree_destroy(handle);
    }


    TEST_FUNCTION(binary_tree_construct_visual_succeed)
    {
        //arrange
        BINARY_TREE_HANDLE handle = binary_tree_create();
        size_t count = sizeof(INSERT_FOR_NO_ROTATION);
        for (size_t index = 0; index < count; index++)
        {
            (void)binary_tree_insert(handle, INSERT_FOR_NO_ROTATION[index], DATA_VALUE);
        }

        //act
        char* result = binary_tree_construct_visual(handle);

        //assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, VISUAL_NO_ROTATION, result);

        //cleanup
        free(result);
        binary_tree_destroy(handle);
    }

END_TEST_SUITE(binary_tree_ut)
