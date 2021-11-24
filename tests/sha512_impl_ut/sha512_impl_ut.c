// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstddef>
#else
#include <stdlib.h>
#include <stddef.h>
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

#define ENABLE_MOCKS
#include "umock_c/umock_c_prod.h"
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "lib-util-c/sha512_impl.h"

static const char* TEST_HASH_VALUE = "Enter The Wu-Tang: 36 Chambers";
static const size_t TEST_HASH_LEN = 30;
static const uint8_t TEST_HASH_RESULT[] = {
    0xA6, 0x02, 0x90, 0x73, 0xF1, 0x0B, 0x5B, 0x63,
    0x4B, 0x87, 0x53, 0x65, 0x09, 0x25, 0x33, 0x7B,
    0x39, 0x74, 0x03, 0xCA, 0x04, 0x71, 0x07, 0xC6,
    0x56, 0x35, 0x3A, 0x32, 0x75, 0xA7, 0x25, 0xF9,
    0xD3, 0xA5, 0x05, 0x61, 0x45, 0x46, 0x38, 0x2E,
    0x6C, 0xC5, 0x2A, 0x09, 0x22, 0xAB, 0xED, 0xC7,
    0x5F, 0x74, 0xE3, 0x63, 0xE5, 0x71, 0xB8, 0xB3,
    0x8C, 0x52, 0x11, 0x8C, 0x4A, 0x36, 0xF7, 0xD7 };

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}


CTEST_BEGIN_TEST_SUITE(sha512_impl_ut)

    CTEST_SUITE_INITIALIZE()
    {
        (void)umock_c_init(on_umock_c_error);

        //REGISTER_UMOCK_ALIAS_TYPE(SHA_HASH_INTERFACE, void*);

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

    CTEST_FUNCTION(sha512_get_interface_succeed)
    {
        //arrange

        //act
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();

        //assert
        CTEST_ASSERT_IS_NOT_NULL(sha_interface);
        CTEST_ASSERT_IS_NOT_NULL(sha_interface->initialize_fn);
        CTEST_ASSERT_IS_NOT_NULL(sha_interface->process_fn);
        CTEST_ASSERT_IS_NOT_NULL(sha_interface->retrieve_result_fn);
        CTEST_ASSERT_IS_NOT_NULL(sha_interface->deinitialize_fn);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    CTEST_FUNCTION(sha512_initialize_succeed)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();

        STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

        //act
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();

        //assert
        CTEST_ASSERT_IS_NOT_NULL(handle);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_initialize_fail)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(malloc(IGNORED_ARG)).SetReturn(NULL);

        //act
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();

        //assert
        CTEST_ASSERT_IS_NULL(handle);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    CTEST_FUNCTION(sha512_deinitialize_succeed)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(free(IGNORED_ARG));

        //act
        sha_interface->deinitialize_fn(handle);

        //assert
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    CTEST_FUNCTION(sha512_deinitialize_handle_NULL_succeed)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        umock_c_reset_all_calls();

        //act
        sha_interface->deinitialize_fn(NULL);

        //assert
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    CTEST_FUNCTION(sha512_process_handle_NULL_fail)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        umock_c_reset_all_calls();

        //act
        int result = sha_interface->process_fn(NULL, TEST_HASH_VALUE, TEST_HASH_LEN);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    CTEST_FUNCTION(sha512_process_msg_array_NULL_fail)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        umock_c_reset_all_calls();

        //act
        int result = sha_interface->process_fn(handle, NULL, TEST_HASH_LEN);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_process_array_len_0_fail)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        umock_c_reset_all_calls();

        //act
        int result = sha_interface->process_fn(handle, TEST_HASH_VALUE, 0);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_process_succeed)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        umock_c_reset_all_calls();

        //act
        int result = sha_interface->process_fn(handle, TEST_HASH_VALUE, TEST_HASH_LEN);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_retrieve_process_twice_fail)
    {
        //arrange
        uint8_t msg_digest[SHA512_HASH_SIZE];
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        int result = sha_interface->process_fn(handle, TEST_HASH_VALUE, TEST_HASH_LEN);
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        result = sha_interface->retrieve_result_fn(handle, msg_digest, SHA512_HASH_SIZE);
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        umock_c_reset_all_calls();

        //act
        result = sha_interface->retrieve_result_fn(handle, msg_digest, SHA512_HASH_SIZE);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(TEST_HASH_RESULT, msg_digest, SHA512_HASH_SIZE));
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_result_handle_NULL_fail)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        umock_c_reset_all_calls();

        //act
        uint8_t msg_digest[SHA512_HASH_SIZE];
        int result = sha_interface->retrieve_result_fn(NULL, msg_digest, SHA512_HASH_SIZE);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    CTEST_FUNCTION(sha512_result_msg_digest_NULL_fail)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        umock_c_reset_all_calls();

        //act
        int result = sha_interface->retrieve_result_fn(handle, NULL, SHA512_HASH_SIZE);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_result_digest_len_0_fail)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        umock_c_reset_all_calls();

        //act
        uint8_t msg_digest[SHA512_HASH_SIZE];
        int result = sha_interface->retrieve_result_fn(handle, msg_digest, 0);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_result_succeed)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        int result = sha_interface->process_fn(handle, TEST_HASH_VALUE, TEST_HASH_LEN);
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        umock_c_reset_all_calls();

        //act
        uint8_t msg_digest[SHA512_HASH_SIZE];
        result = sha_interface->retrieve_result_fn(handle, msg_digest, SHA512_HASH_SIZE);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(TEST_HASH_RESULT, msg_digest, SHA512_HASH_SIZE));
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_result_len_too_small_fail)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        int result = sha_interface->process_fn(handle, TEST_HASH_VALUE, TEST_HASH_LEN);
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        umock_c_reset_all_calls();

        //act
        uint8_t msg_digest[SHA512_HASH_SIZE];
        result = sha_interface->retrieve_result_fn(handle, msg_digest, 10);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

    CTEST_FUNCTION(sha512_result_without_process_succeed)
    {
        //arrange
        const SHA_HASH_INTERFACE* sha_interface = sha512_get_interface();
        SHA_IMPL_HANDLE handle = sha_interface->initialize_fn();
        umock_c_reset_all_calls();

        //act
        uint8_t msg_digest[SHA512_HASH_SIZE];
        int result = sha_interface->retrieve_result_fn(handle, msg_digest, SHA512_HASH_SIZE);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        sha_interface->deinitialize_fn(handle);
    }

CTEST_END_TEST_SUITE(sha512_impl_ut)
