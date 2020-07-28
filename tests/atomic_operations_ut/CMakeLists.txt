#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.2)

set(theseTestsName atomic_operations_ut)

set(${theseTestsName}_test_files
    ${theseTestsName}.c
)

if (WIN32)
    set(${theseTestsName}_c_files
        ../../src/pal/win/atomic_operations_win.c
    )
else()
    set(${theseTestsName}_c_files
        ../../src/pal/linux/atomic_operations_linux.c
    )
endif()

set(${theseTestsName}_h_files
)

build_test_project(${theseTestsName} "tests/lib_utils_tests")
