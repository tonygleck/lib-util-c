#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(lib-util-c)

option(include_ut "Include unittest in build" OFF)
#option(refresh_submodules "set refresh_submodules to ON to update submodule files (default is OFF)" OFF)

# do not add or build any tests of the dependencies
set(original_include_ut ${include_ut})

set(run_e2e_tests OFF)
set(run_unittests OFF)
set(skip_samples ON)

include("${CMAKE_CURRENT_LIST_DIR}/cmake_configs/proj_test.cmake")

# Add dependencies
if (NOT TARGET umock_c)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/deps/umock-c)
endif()
include_directories(${UMOCK_C_INC_FOLDER})

if ((NOT TARGET azure_macro_utils_c) AND (EXISTS ${CMAKE_CURRENT_LIST_DIR}/deps/azure-macro-utils-c/CMakeLists.txt))
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/azure-macro-utils-c)
endif()
include_directories(${MACRO_UTILS_INC_FOLDER})

set(include_ut ${original_include_ut})

# Add testing
if (${include_ut})
    enable_testing()

    set(logging_files ${CMAKE_CURRENT_LIST_DIR}/src/app_logging.c)

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/testrunnerswitcher/CMakeLists.txt")
        execute_process(COMMAND git clone https://github.com/Azure/azure-c-testrunnerswitcher.git -q ${CMAKE_CURRENT_SOURCE_DIR}/deps/testrunnerswitcher)
    endif()
    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/ctest/CMakeLists.txt")
        execute_process(COMMAND git clone https://github.com/Azure/azure-ctest.git -q ${CMAKE_CURRENT_SOURCE_DIR}/deps/ctest)
    endif()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/testrunnerswitcher)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/ctest)
endif()

set(lib_src_files
    src/app_logging.c
    src/alarm_timer.c
    src/item_list.c
)

set(lib_header_files
    inc/lib-util-c/app_logging.h
    inc/lib-util-c/alarm_timer.h
    inc/lib-util-c/item_list.h
)

include_directories(./inc)

add_library(lib-util-c ${lib_src_files} ${lib_header_files})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
