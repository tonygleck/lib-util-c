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

// Include the test tools.
#include "ctest.h"
#include "azure_macro_utils/macro_utils.h"

#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_charptr.h"
#include "umock_c/umock_c_negative_tests.h"
#include "umock_c/umocktypes_bool.h"

#define ENABLE_MOCKS
#include "lib-util-c/sys_debug_shim.h"

#undef ENABLE_MOCKS

//#include <windows.h>

#include "lib-util-c/mutex_mgr.h"

const HANDLE TEST_MUTEX_HANDLE = (HANDLE)0x123456;

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(mutex_mgr_win32_ut)

CTEST_SUITE_INITIALIZE()
{
    (void)umock_c_init(on_umock_c_error);

    CTEST_ASSERT_ARE_EQUAL(int, 0, umocktypes_charptr_register_types());
    CTEST_ASSERT_ARE_EQUAL(int, 0, umocktypes_bool_register_types());

    REGISTER_UMOCK_ALIAS_TYPE(LPSECURITY_ATTRIBUTES, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(BOOL, int);
    REGISTER_UMOCK_ALIAS_TYPE(LPCSTR, char*);
    REGISTER_UMOCK_ALIAS_TYPE(DWORD, unsigned int);

    REGISTER_GLOBAL_MOCK_RETURN(CreateMutexW, TEST_MUTEX_HANDLE);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(CreateMutexW, NULL);
    REGISTER_GLOBAL_MOCK_RETURN(WaitForSingleObject, WAIT_OBJECT_0);
    REGISTER_GLOBAL_MOCK_RETURN(ReleaseMutex, TRUE);
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
    STRICT_EXPECTED_CALL(CreateMutex(IGNORED_ARG, TRUE, IGNORED_ARG));

    //act
    int result = mutex_mgr_create(&handle);

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
    STRICT_EXPECTED_CALL(CreateMutex(IGNORED_ARG, TRUE, IGNORED_ARG)).SetReturn(NULL);

    //act
    int result = mutex_mgr_create(&handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(mutex_mgr_create_handle_NULL_fail)
{
    //arrange

    //act
    int result = mutex_mgr_create(NULL);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(mutex_mgr_destroy_success)
{
    MUTEX_HANDLE handle;
    CTEST_ASSERT_ARE_EQUAL(int, 0, mutex_mgr_create(&handle));
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(CloseHandle(handle));

    //act
    mutex_mgr_destroy(handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(mutex_mgr_destroy_handle_NULL_fail)
{
    //arrange

    //act
    mutex_mgr_destroy(NULL);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(mutex_mgr_lock_success)
{
    MUTEX_HANDLE handle;
    CTEST_ASSERT_ARE_EQUAL(int, 0, mutex_mgr_create(&handle));
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(WaitForSingleObject(handle, IGNORED_ARG));

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
    CTEST_ASSERT_ARE_EQUAL(int, 0, mutex_mgr_create(&handle));
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(WaitForSingleObject(handle, IGNORED_ARG)).SetReturn(WAIT_FAILED);

    //act
    int result = mutex_mgr_lock(handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_lock_handle_NULL_fail)
{
    //arrange

    //act
    int result = mutex_mgr_lock(NULL);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(mutex_mgr_trylock_success)
{
    MUTEX_HANDLE handle;
    CTEST_ASSERT_ARE_EQUAL(int, 0, mutex_mgr_create(&handle));
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(WaitForSingleObject(handle, IGNORED_ARG));

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
    CTEST_ASSERT_ARE_EQUAL(int, 0, mutex_mgr_create(&handle));
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(WaitForSingleObject(handle, IGNORED_ARG)).SetReturn(WAIT_FAILED);

    //act
    int result = mutex_mgr_trylock(handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_trylock_handle_NULL_fail)
{
    //arrange

    //act
    int result = mutex_mgr_trylock(NULL);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(mutex_mgr_unlock_success)
{
    MUTEX_HANDLE handle;
    CTEST_ASSERT_ARE_EQUAL(int, 0, mutex_mgr_create(&handle));
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(ReleaseMutex(handle));

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
    CTEST_ASSERT_ARE_EQUAL(int, 0, mutex_mgr_create(&handle));
    umock_c_reset_all_calls();

    //arrange
    STRICT_EXPECTED_CALL(ReleaseMutex(handle)).SetReturn(FALSE);

    //act
    int result = mutex_mgr_unlock(handle);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    mutex_mgr_destroy(handle);
}

CTEST_FUNCTION(mutex_mgr_unlock_handle_NULL_fail)
{
    //arrange

    //act
    int result = mutex_mgr_unlock(NULL);

    //assert
    CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_END_TEST_SUITE(mutex_mgr_win32_ut)
