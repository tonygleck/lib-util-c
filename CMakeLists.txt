#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(lib-util-c)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
set(PROJECT_INC_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/inc)
#set(TEST_INC_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/inc)

option(include_ut "Include unittest in build" OFF)

enable_testing()

#include("cmake_configs/proj_config.cmake")
#if (MSVC)
#    detect_architecture("_M_AMD64" x86_64)
#    detect_architecture("_M_IX86" x86)
#    detect_architecture("_M_ARM" ARM)
#else()
#    detect_architecture("__x86_64__" x86_64)
#    detect_architecture("__i386__" x86)
#    detect_architecture("__arm__" ARM)
#endif()
#if (NOT DEFINED ARCHITECTURE OR ARCHITECTURE STREQUAL "")
#    set(ARCHITECTURE "GENERIC")
#endif()
#message(STATUS "Current architecture: ${ARCHITECTURE}")

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4201")

    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    add_definitions(/WX)

    #set_source_files_properties(bandit/bandit/external/optionparser.h PROPERTIES COMPILE_FLAGS "/wd4701 /wd4703")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Wno-variadic-macros")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Wno-variadic-macros")
    # Make warning as error
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
endif()

# Add dependencies
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/deps/umock-c)
include_directories(${UMOCK_C_INC_FOLDER})

set(lib_src_files
    src/alarm_timer.c
    src/item_list.c
)

set(lib_header_files
    inc/lib-util-c/alarm_timer.h
    inc/lib-util-c/item_list.h
)

include_directories(./inc)

add_library(lib-util-c ${lib_src_files} ${lib_header_files})

# Add testing
if (${include_ut})
    include("${CMAKE_CURRENT_LIST_DIR}/cmake_configs/proj_test.cmake")

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/testrunnerswitcher/CMakeLists.txt")
        execute_process(COMMAND git clone https://github.com/Azure/azure-c-testrunnerswitcher.git -q --no-tags ${CMAKE_CURRENT_SOURCE_DIR}/deps/testrunnerswitcher)
    endif()
    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/ctest/CMakeLists.txt")
        execute_process(COMMAND git clone https://github.com/Azure/azure-ctest.git -q --no-tags ${CMAKE_CURRENT_SOURCE_DIR}/deps/ctest)
    endif()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/testrunnerswitcher)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/ctest)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()
