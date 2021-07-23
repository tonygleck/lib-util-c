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
#include "ctest.h"
#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_bool.h"

#define ENABLE_MOCKS
#include "umock_c/umock_c_prod.h"
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "lib-util-c/dllist.h"

static size_t g_id_value = 1;

typedef struct TEST_DLLIST_ITEM_TAG
{
    size_t id;
    const char* tag;
    DLLIST_ENTRY dllist_entry;
} TEST_DLLIST_ITEM;

static size_t get_next_id(void)
{
    return g_id_value++;
}

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(dllist_ut)

CTEST_SUITE_INITIALIZE()
{
    CTEST_ASSERT_ARE_EQUAL(int, 0, umock_c_init(on_umock_c_error));
    CTEST_ASSERT_ARE_EQUAL(int, 0, umocktypes_bool_register_types());

    REGISTER_UMOCK_ALIAS_TYPE(PDLLIST_ENTRY, void*);

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
    g_id_value = 1;
}

CTEST_FUNCTION_CLEANUP()
{
}

CTEST_FUNCTION(dllist_init_list_head_succeed)
{
    //arrange
    DLLIST_ENTRY list_entry = {0};

    //act
    dllist_init_list_head(&list_entry);

    //assert
    CTEST_ASSERT_ARE_EQUAL(void_ptr, &list_entry, list_entry.fwd_link);
    CTEST_ASSERT_ARE_EQUAL(void_ptr, &list_entry, list_entry.bk_link);

    //cleanup
}

CTEST_FUNCTION(dllist_is_empty_on_emtpy_list_succeed)
{
    //arrange
    DLLIST_ENTRY list_entry;
    dllist_init_list_head(&list_entry);

    //act
    bool result = dllist_is_empty(&list_entry);

    //assert
    CTEST_ASSERT_IS_TRUE(result);

    //cleanup
}

CTEST_FUNCTION(dllist_is_empty_on_non_emtpy_list_succeed)
{
    //arrange
    DLLIST_ENTRY list_entry;
    TEST_DLLIST_ITEM test_item = { get_next_id(), "Test Item 1" };

    dllist_init_list_head(&list_entry);
    dllist_insert_tail(&list_entry, &test_item.dllist_entry);

    //act
    bool result = dllist_is_empty(&list_entry);

    //assert
    CTEST_ASSERT_IS_FALSE(result);

    //cleanup
}

CTEST_FUNCTION(dllist_insert_tail_succeed)
{
    //arrange
    DLLIST_ENTRY list_entry;
    TEST_DLLIST_ITEM test_item_1 = { get_next_id(), "Test Item 1" };
    TEST_DLLIST_ITEM test_item_2 = { get_next_id(), "Test Item 2" };

    dllist_init_list_head(&list_entry);

    //act
    dllist_insert_tail(&list_entry, &test_item_1.dllist_entry);
    dllist_insert_tail(&list_entry, &test_item_2.dllist_entry);

    //assert
    CTEST_ASSERT_ARE_EQUAL(void_ptr, list_entry.fwd_link, &(test_item_1.dllist_entry));
    CTEST_ASSERT_ARE_EQUAL(void_ptr, test_item_1.dllist_entry.fwd_link, &(test_item_2.dllist_entry));
    CTEST_ASSERT_ARE_EQUAL(void_ptr, test_item_2.dllist_entry.fwd_link, &list_entry);
    CTEST_ASSERT_ARE_EQUAL(void_ptr, list_entry.bk_link, &(test_item_2.dllist_entry));
    CTEST_ASSERT_ARE_EQUAL(void_ptr, test_item_2.dllist_entry.bk_link, &(test_item_1.dllist_entry));
    CTEST_ASSERT_ARE_EQUAL(void_ptr, test_item_1.dllist_entry.bk_link, &list_entry);

    //cleanup
}

CTEST_END_TEST_SUITE(dllist_ut)
