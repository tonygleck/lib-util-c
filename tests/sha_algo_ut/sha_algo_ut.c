// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstddef>
#else
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
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
#include "umock_c/umock_c_prod.h"
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "lib-util-c/sha_algorithms.h"

#undef ENABLE_MOCKS

#define ENABLE_MOCKS
// MOCKABLE_FUNCTION(, SHA_IMPL_HANDLE, test_init_hash);
// MOCKABLE_FUNCTION(, void, test_deinit_hash, SHA_IMPL_HANDLE, handle);
// MOCKABLE_FUNCTION(, int, test_process_hash, SHA_IMPL_HANDLE, handle, const uint8_t*, msg_array, size_t, array_len);
// MOCKABLE_FUNCTION(, int, test_retrieve_hash, SHA_IMPL_HANDLE, handle, uint8_t, msg_digest[], size_t, digest_len);
#undef ENABLE_MOCKS

static bool negative_test_run;

SHA_IMPL_HANDLE test_init_hash(void)
{
    return (SHA_IMPL_HANDLE)my_mem_shim_malloc(1);
}

void test_deinit_hash(SHA_IMPL_HANDLE handle)
{
    my_mem_shim_free(handle);
}

int test_process_hash(SHA_IMPL_HANDLE handle, const uint8_t* msg_array, size_t array_len)
{
    (void)handle;
    (void)msg_array;
    (void)array_len;
    return negative_test_run ? __LINE__ : 0;
}

int test_retrieve_hash(SHA_IMPL_HANDLE handle, uint8_t msg_digest[], size_t digest_len)
{
    (void)handle;
    (void)msg_digest;
    (void)digest_len;
    return negative_test_run ? __LINE__ : 0;
}

const SHA_HASH_INTERFACE test_hash_interface =
{
    test_init_hash,
    test_deinit_hash,
    test_process_hash,
    test_retrieve_hash
};

const SHA_HASH_INTERFACE test_iface_init_NULL =
{
    NULL,
    test_deinit_hash,
    test_process_hash,
    test_retrieve_hash
};

const SHA_HASH_INTERFACE test_iface_deinit_NULL =
{
    test_init_hash,
    NULL,
    test_process_hash,
    test_retrieve_hash
};

const SHA_HASH_INTERFACE test_iface_process_NULL =
{
    test_init_hash,
    test_deinit_hash,
    NULL,
    test_retrieve_hash
};

const SHA_HASH_INTERFACE test_iface_retrieve_NULL =
{
    test_init_hash,
    test_deinit_hash,
    test_process_hash,
    NULL
};

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(sha_algo_ut)

CTEST_SUITE_INITIALIZE()
{
    int result;

    (void)umock_c_init(on_umock_c_error);

    REGISTER_UMOCK_ALIAS_TYPE(SHA_CTX_HANDLE, void*);

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
    negative_test_run = false;
}

CTEST_FUNCTION_CLEANUP()
{
}

CTEST_FUNCTION(sha_algorithms_init_interface_NULL_fail)
{
    // arrange

    // act
    SHA_CTX_HANDLE handle = sha_algorithms_init(NULL);

    // assert
    CTEST_ASSERT_IS_NULL(handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(sha_algorithms_init_interface_field_NULL_fail)
{
    // arrange
    const SHA_HASH_INTERFACE* fail_interface;

    // act
    for (size_t index = 0; index < 4; index++)
    {
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
        STRICT_EXPECTED_CALL(free(IGNORED_ARG));

        switch (index)
        {
            case 0:
                fail_interface = &test_iface_init_NULL;
                break;
            case 1:
                fail_interface = &test_iface_deinit_NULL;
                break;
            case 2:
                fail_interface = &test_iface_process_NULL;
                break;
            case 3:
                fail_interface = &test_iface_retrieve_NULL;
                break;
        }

        SHA_CTX_HANDLE handle = sha_algorithms_init(fail_interface);

        // assert
        CTEST_ASSERT_IS_NULL(handle, "sha_algorithms_init failure in test %lu", index);
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    // cleanup
}

CTEST_FUNCTION(sha_algorithms_init_fail)
{
    // arrange

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG)).SetReturn(NULL);

    // act
    SHA_CTX_HANDLE handle = sha_algorithms_init(&test_hash_interface);

    // assert
    CTEST_ASSERT_IS_NULL(handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(sha_algorithms_init_succeed)
{
    // arrange

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    // act
    SHA_CTX_HANDLE handle = sha_algorithms_init(&test_hash_interface);

    // assert
    CTEST_ASSERT_IS_NOT_NULL(handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    sha_algorithms_deinit(handle);
}

CTEST_FUNCTION(sha_algorithms_deinit_handle_NULL_succeed)
{
    // arrange

    // act

    // assert
    sha_algorithms_deinit(NULL);

    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(sha_algorithms_deinit_succeed)
{
    // arrange
    SHA_CTX_HANDLE handle = sha_algorithms_init(&test_hash_interface);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    // assert
    sha_algorithms_deinit(handle);

    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(sha_algorithms_process_handle_NULL_fail)
{
    // arrange
    uint8_t msg_array[] = {'b', 'y', 'e'};
    size_t array_len = sizeof(msg_array)/sizeof(msg_array[0]);
    uint8_t msg_digest[10];
    size_t digest_len = 10;

    // act

    // assert
    int result = sha_algorithms_process(NULL, msg_array, array_len, msg_digest, digest_len);

    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

CTEST_FUNCTION(sha_algorithms_process_msg_array_NULL_fail)
{
    // arrange
    size_t array_len = 10;
    uint8_t msg_digest[10];
    size_t digest_len = 10;

    SHA_CTX_HANDLE handle = sha_algorithms_init(&test_hash_interface);
    umock_c_reset_all_calls();

    // act

    // assert
    int result = sha_algorithms_process(handle, NULL, array_len, msg_digest, digest_len);

    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    sha_algorithms_deinit(handle);
}

CTEST_FUNCTION(sha_algorithms_process_array_len_0_fail)
{
    // arrange
    uint8_t msg_array[] = {'b', 'y', 'e'};
    uint8_t msg_digest[10];
    size_t digest_len = 10;

    SHA_CTX_HANDLE handle = sha_algorithms_init(&test_hash_interface);
    umock_c_reset_all_calls();

    // act

    // assert
    int result = sha_algorithms_process(handle, msg_array, 0, msg_digest, digest_len);

    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    sha_algorithms_deinit(handle);
}

CTEST_FUNCTION(sha_algorithms_process_msg_digest_NULL_fail)
{
    // arrange
    uint8_t msg_array[] = {'b', 'y', 'e'};
    size_t array_len = sizeof(msg_array)/sizeof(msg_array[0]);
    size_t digest_len = 10;

    SHA_CTX_HANDLE handle = sha_algorithms_init(&test_hash_interface);
    umock_c_reset_all_calls();

    // act

    // assert
    int result = sha_algorithms_process(handle, msg_array, array_len, NULL, digest_len);

    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    sha_algorithms_deinit(handle);
}

CTEST_FUNCTION(sha_algorithms_process_succeed)
{
    // arrange
    uint8_t msg_array[] = {'b', 'y', 'e'};
    size_t array_len = sizeof(msg_array)/sizeof(msg_array[0]);
    uint8_t msg_digest[10];
    size_t digest_len = 10;

    SHA_CTX_HANDLE handle = sha_algorithms_init(&test_hash_interface);
    umock_c_reset_all_calls();

    // act
    int result = sha_algorithms_process(handle, msg_array, array_len, msg_digest, digest_len);

    // assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    sha_algorithms_deinit(handle);
}

CTEST_FUNCTION(sha_algorithms_process_fail)
{
    // arrange
    uint8_t msg_array[] = {'b', 'y', 'e'};
    size_t array_len = sizeof(msg_array)/sizeof(msg_array[0]);
    uint8_t msg_digest[10];
    size_t digest_len = 10;

    SHA_CTX_HANDLE handle = sha_algorithms_init(&test_hash_interface);
    umock_c_reset_all_calls();

    //int negativeTestsInitResult = umock_c_negative_tests_init();
    //CTEST_ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    //umock_c_negative_tests_snapshot();
    negative_test_run = true;

    size_t count = 2;
    for (size_t index = 0; index < count; index++)
    {
        //umock_c_negative_tests_reset();
        //umock_c_negative_tests_fail_call(index);

        // act
        int result = sha_algorithms_process(handle, msg_array, array_len, msg_digest, digest_len);

        // assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    }

    // cleanup
    //umock_c_negative_tests_deinit();
    sha_algorithms_deinit(handle);
}

CTEST_END_TEST_SUITE(sha_algo_ut)
