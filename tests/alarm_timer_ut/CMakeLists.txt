#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.2)

set(theseTestsName alarm_timer_ut)

set(${theseTestsName}_test_files
    ${theseTestsName}.c
)

set(${theseTestsName}_c_files
    ../../src/alarm_timer.c
)

set(${theseTestsName}_h_files
)

build_test_project(${theseTestsName} "tests/lib_utils_tests")
