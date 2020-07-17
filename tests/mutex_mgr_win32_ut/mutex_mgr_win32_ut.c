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

#include <windows.h>

#define ENABLE_MOCKS
#include "lib-util-c/sys_debug_shim.h"

MOCKABLE_FUNCTION(, HANDLE, thread_shim_CreateMutex, LPSECURITY_ATTRIBUTES, lpMutexAttributes, BOOL, bInitialOwner, LPCSTR, lpName);
MOCKABLE_FUNCTION(, BOOL, thread_shim_CloseHandle, HANDLE, hObject);
MOCKABLE_FUNCTION(, DWORD, thread_shim_WaitForSingleObject, HANDLE, hHandle, DWORD, dwMilliseconds);
MOCKABLE_FUNCTION(, BOOL, thread_shim_ReleaseMutex, HANDLE, hMutex);
#undef ENABLE_MOCKS

#define CreateMutexW        thread_shim_CreateMutex
#define CloseHandle         thread_shim_CloseHandle
#define WaitForSingleObject thread_shim_WaitForSingleObject
#define ReleaseMutex        thread_shim_ReleaseMutex


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
    int result;

    (void)umock_c_init(on_umock_c_error);

    result = umocktypes_charptr_register_types();
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);
    result = umocktypes_bool_register_types();
    CTEST_ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_UMOCK_ALIAS_TYPE(LPSECURITY_ATTRIBUTES, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(BOOL, int);
    REGISTER_UMOCK_ALIAS_TYPE(LPCSTR, char*);

    REGISTER_GLOBAL_MOCK_RETURN(CreateMutexW, TEST_MUTEX_HANDLE);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(CreateMutexW, NULL);
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

CTEST_END_TEST_SUITE(mutex_mgr_win32_ut)
