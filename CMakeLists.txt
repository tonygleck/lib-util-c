#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(lib-util-c)

option(lib_util_c_ut "Include unittest in build" OFF)
#option(refresh_submodules "set refresh_submodules to ON to update submodule files (default is OFF)" OFF)

# do not add or build any tests of the dependencies
set(skip_samples ON)

include("${CMAKE_CURRENT_LIST_DIR}/cmake_configs/proj_config.cmake")

# Add dependencies
if (NOT TARGET azure_macro_utils_c)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/azure-macro-utils-c)
endif()
include_directories(${MACRO_UTILS_INC_FOLDER})
if (NOT TARGET umock_c)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/deps/umock-c)
endif()
include_directories(${UMOCK_C_INC_FOLDER})

set(lib_src_files
    src/app_logging.c
    src/alarm_timer.c
    src/item_list.c
    src/sys_debug_shim.c
)
set(lib_header_files
    inc/lib-util-c/app_logging.h
    inc/lib-util-c/alarm_timer.h
    inc/lib-util-c/item_list.h
    inc/lib-util-c/sys_debug_shim.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_library(lib-util-c ${lib_src_files} ${lib_header_files})

compileTargetAsC99(lib-util-c)

# Add testing
if (${lib_util_c_ut})
    include("${CMAKE_CURRENT_LIST_DIR}/cmake_configs/proj_test.cmake")

    if ((NOT TARGET testrunnerswitcher) AND (EXISTS ${CMAKE_CURRENT_LIST_DIR}/deps/azure-c-testrunnerswitcher/CMakeLists.txt))
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/azure-c-testrunnerswitcher)
    endif()
    include_directories(${TESTRUNNERSWITCHER_INC_FOLDER})
    if ((NOT TARGET ctest) AND (EXISTS ${CMAKE_CURRENT_LIST_DIR}/deps/azure-ctest/CMakeLists.txt))
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/azure-ctest)
    endif()
    include_directories(${CTEST_INC_FOLDER})

    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()
