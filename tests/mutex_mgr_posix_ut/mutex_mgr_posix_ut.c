// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#ifdef __cplusplus
#include <cstddef>
#include <ctime>
#else
#include <stddef.h>
#include <time.h>
#endif

#include <pthread.h>
#include <errno.h>

// Include the test tools.
#include "ctest.h"
#include "azure_macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_charptr.h"
#include "umock_c/umock_c_negative_tests.h"

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
#include "lib-util-c/sys_debug_shim.h"

MOCKABLE_FUNCTION(, int, pthread_mutex_init, pthread_mutex_t*, mutex, const pthread_mutexattr_t*, attr);
MOCKABLE_FUNCTION(, int, pthread_mutex_destroy, pthread_mutex_t*, mutex);
MOCKABLE_FUNCTION(, int, pthread_mutex_lock, pthread_mutex_t*, mutex);
MOCKABLE_FUNCTION(, int, pthread_mutex_trylock, pthread_mutex_t*, mutex);
MOCKABLE_FUNCTION(, int, pthread_mutex_unlock, pthread_mutex_t*, mutex);
#undef ENABLE_MOCKS

#include "lib-util-c/mutex_mgr.h"

static int my_pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* mutexattr)
{
    (void)mutexattr;
    memset(mutex, 0, sizeof(pthread_mutex_t));
    return 0;
}

static int my_pthread_mutex_destroy(pthread_mutex_t* mutex)
{
    return 0;
}


MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(mutex_mgr_posix_ut)

CTEST_SUITE_INITIALIZE()
{
    int result;

    (void)umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_realloc, my_mem_shim_realloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_realloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);

    REGISTER_GLOBAL_MOCK_HOOK(pthread_mutex_init, my_pthread_mutex_init);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_mutex_init, -1);
    REGISTER_GLOBAL_MOCK_HOOK(pthread_mutex_destroy, my_pthread_mutex_destroy);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_mutex_destroy, -1);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_mutex_lock, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_mutex_lock, -1);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_mutex_trylock, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_mutex_trylock, -1);
    REGISTER_GLOBAL_MOCK_RETURN(pthread_mutex_unlock, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(pthread_mutex_unlock, -1);
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

CTEST_FUNCTION(mutex_mgr_create_succeed)
{
    MUTEX_HANDLE handle;

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_init(IGNORED_ARG, IGNORED_ARG));

    //act
    int result = mutex_mgr_create(handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_create_fail)
{
    MUTEX_HANDLE handle;

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_init(IGNORED_ARG, IGNORED_ARG)).SetReturn(-1);

    //act
    int result = mutex_mgr_create(handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_destroy_succeed)
{
    MUTEX_HANDLE handle;
    mutex_mgr_create(handle);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_destroy(IGNORED_ARG));

    //act
    mutex_mgr_destroy(handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(mutex_mgr_lock_succeed)
{
    MUTEX_HANDLE handle;
    mutex_mgr_create(handle);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_lock(IGNORED_ARG));

    //act
    int result = mutex_mgr_lock(handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_lock_fail)
{
    MUTEX_HANDLE handle;
    mutex_mgr_create(handle);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_lock(IGNORED_ARG)).SetReturn(-1);

    //act
    int result = mutex_mgr_lock(handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_trylock_succeed)
{
    MUTEX_HANDLE handle;
    mutex_mgr_create(handle);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_trylock(IGNORED_ARG));

    //act
    int result = mutex_mgr_trylock(handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_trylock_fail)
{
    MUTEX_HANDLE handle;
    mutex_mgr_create(handle);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_trylock(IGNORED_ARG)).SetReturn(-1);

    //act
    int result = mutex_mgr_trylock(handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_unlock_succeed)
{
    MUTEX_HANDLE handle;
    mutex_mgr_create(handle);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_unlock(IGNORED_ARG));

    //act
    int result = mutex_mgr_unlock(handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_unlock_fail)
{
    MUTEX_HANDLE handle;
    mutex_mgr_create(handle);
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(pthread_mutex_unlock(IGNORED_ARG)).SetReturn(-1);

    //act
    int result = mutex_mgr_unlock(handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_END_TEST_SUITE(mutex_mgr_posix_ut)
