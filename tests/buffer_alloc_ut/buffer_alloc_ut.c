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

static void* my_mem_shim_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
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

#include "lib-util-c/buffer_alloc.h"

#define DEFAULT_ALLOC_SIZE       64

static TEST_MUTEX_HANDLE test_serialize_mutex;
MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

BEGIN_TEST_SUITE(buffer_alloc_ut)

TEST_SUITE_INITIALIZE(suite_init)
{
    test_serialize_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_serialize_mutex);

    umock_c_init(on_umock_c_error);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_realloc, my_mem_shim_realloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_realloc, NULL);
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

TEST_FUNCTION(string_buffer_construct_buffer_NULL_fail)
{
    // arrange
    const char* src_string = "test_string";

    // act
    int result = string_buffer_construct(NULL, src_string);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(string_buffer_construct_value_NULL_fail)
{
    // arrange
    STRING_BUFFER buffer = { 0 };

    // act
    int result = string_buffer_construct(&buffer, NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(string_buffer_construct_succeed)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* src_string = "test_string";

    STRICT_EXPECTED_CALL(malloc(11+DEFAULT_ALLOC_SIZE+1));

    // act
    int result = string_buffer_construct(&buffer, src_string);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, src_string, buffer.payload);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(buffer.payload);
}

TEST_FUNCTION(string_buffer_construct_append_string_succeed)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* src_string = "test";
    const char* second_string = "_string";
    const char* total_string = "test_string";

    (void)string_buffer_construct(&buffer, src_string);
    umock_c_reset_all_calls();

    // act
    int result = string_buffer_construct(&buffer, second_string);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, total_string, buffer.payload);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(buffer.payload);
}

TEST_FUNCTION(string_buffer_construct_alloc_append_succeed)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* src_string = "Two things are infinite: ";
    const char* second_string = "the universe and human stupidity; and I'm not sure about the universe";
    const char* total_string = "Two things are infinite: the universe and human stupidity; and I'm not sure about the universe";

    (void)string_buffer_construct(&buffer, src_string);
    umock_c_reset_all_calls();

    //size_t realloc_len = strlen(second_string);
    STRICT_EXPECTED_CALL(realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));

    // act
    int result = string_buffer_construct(&buffer, second_string);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, total_string, buffer.payload);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(buffer.payload);
}

TEST_FUNCTION(string_buffer_construct_malloc_fail)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* src_string = "test_string";

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG)).SetReturn(NULL);

    // act
    int result = string_buffer_construct(&buffer, src_string);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(string_buffer_construct_sprintf_buffer_NULL_fail)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* fmt_string = "test_string_%d";
    const char* total_string = "test_string_123";
    int value = 123;

    // act
    int result = string_buffer_construct_sprintf(NULL, fmt_string, value);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(string_buffer_construct_sprintf_format_NULL_fail)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* total_string = "test_string_123";
    int value = 123;

    // act
    int result = string_buffer_construct_sprintf(&buffer, NULL, value);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(string_buffer_construct_sprintf_succeed)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* fmt_string = "test_string_%d";
    const char* total_string = "test_string_123";
    int value = 123;

    STRICT_EXPECTED_CALL(malloc(15+DEFAULT_ALLOC_SIZE+1));

    // act
    int result = string_buffer_construct_sprintf(&buffer, fmt_string, value);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, total_string, buffer.payload);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(buffer.payload);
}

TEST_FUNCTION(string_buffer_construct_sprintf_malloc_fail)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* fmt_string = "test_string_%d";
    const char* total_string = "test_string_123";
    int value = 123;

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG)).SetReturn(NULL);

    // act
    int result = string_buffer_construct_sprintf(&buffer, fmt_string, value);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(string_buffer_construct_sprintf_append_string_succeed)
{
    // arrange
    STRING_BUFFER buffer = { 0 };
    const char* src_string = "test";
    const char* second_string = "_string_%d";
    const char* total_string = "test_string_123";
    int value = 123;

    (void)string_buffer_construct(&buffer, src_string);
    umock_c_reset_all_calls();

    // act
    int result = string_buffer_construct_sprintf(&buffer, second_string, value);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, total_string, buffer.payload);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(buffer.payload);
}

TEST_FUNCTION(byte_buffer_construct_buffer_NULL_fail)
{
    // arrange
    const unsigned char binary_string[] = { 0x21, 0x22, 0x23, 0x24, 0x25, 0x26 };
    size_t bin_length = 5;

    // act
    int result = byte_buffer_construct(NULL, binary_string, bin_length);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(byte_buffer_construct_value_NULL_succeed)
{
    // arrange
    BYTE_BUFFER buffer = { 0 };
    size_t bin_length = 5;

    // act
    int result = byte_buffer_construct(&buffer, NULL, bin_length);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(byte_buffer_construct_size_0_succeed)
{
    // arrange
    BYTE_BUFFER buffer = { 0 };
    const unsigned char binary_buff[] = { 0x21, 0x22, 0x23, 0x24, 0x25, 0x26 };
    size_t bin_length = 5;

    // act
    int result = byte_buffer_construct(&buffer, binary_buff, 0);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(byte_buffer_construct_succeed)
{
    // arrange
    BYTE_BUFFER buffer = { 0 };
    const unsigned char binary_buff[] = { 0x21, 0x22, 0x23, 0x24, 0x25, 0x26 };
    size_t bin_length = 5;

    STRICT_EXPECTED_CALL(malloc(bin_length+DEFAULT_ALLOC_SIZE+1));

    // act
    int result = byte_buffer_construct(&buffer, binary_buff, bin_length);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, memcmp(buffer.payload, binary_buff, buffer.payload_size));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(buffer.payload);
}

TEST_FUNCTION(byte_buffer_construct_malloc_fail)
{
    // arrange
    BYTE_BUFFER buffer = { 0 };
    const unsigned char binary_buff[] = { 0x21, 0x22, 0x23, 0x24, 0x25, 0x26 };
    size_t bin_length = 5;

    STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG)).SetReturn(NULL);

    // act
    int result = byte_buffer_construct(&buffer, binary_buff, bin_length);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

TEST_FUNCTION(byte_buffer_construct_append_binary_succeed)
{
    // arrange
    size_t bin_length = 64;
    size_t init_length = 4;
    BYTE_BUFFER buffer = { 0 };
    unsigned char initial_buff[68];
    for (size_t index = 0; index < bin_length+init_length; index++)
    {
        initial_buff[index] = 0x25 + index;
    }

    (void)byte_buffer_construct(&buffer, initial_buff, init_length);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(realloc(IGNORED_PTR_ARG, bin_length+bin_length+init_length+DEFAULT_ALLOC_SIZE+1));

    // act
    int result = byte_buffer_construct(&buffer, initial_buff+init_length, bin_length);

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(int, 0, memcmp(buffer.payload, initial_buff, buffer.payload_size));
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(buffer.payload);
}

TEST_FUNCTION(byte_buffer_construct_append_binary_realloc_fail)
{
    // arrange
    size_t bin_length = 64;
    size_t init_length = 4;
    BYTE_BUFFER buffer = { 0 };
    unsigned char initial_buff[68];
    for (size_t index = 0; index < bin_length+init_length; index++)
    {
        initial_buff[index] = 0x25 + index;
    }

    (void)byte_buffer_construct(&buffer, initial_buff, init_length);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG)).SetReturn(NULL);

    // act
    int result = byte_buffer_construct(&buffer, initial_buff+init_length, bin_length);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    my_mem_shim_free(buffer.payload);
}

END_TEST_SUITE(buffer_alloc_ut)
