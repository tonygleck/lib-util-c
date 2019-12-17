#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.2)

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(lib-util-c)

option(lib_util_c_ut "Include unittest in build" OFF)

# do not add or build any tests of the dependencies
set(skip_samples ON)

include("${CMAKE_CURRENT_LIST_DIR}/cmake_configs/proj_config.cmake")

# Add dependencies
if (NOT TARGET azure_macro_utils_c)
    add_subdirectory(${PROJECT_SOURCE_DIR}/deps/azure-macro-utils-c)
endif()
include_directories(${MACRO_UTILS_INC_FOLDER})
if (NOT TARGET umock_c)
    add_subdirectory(${PROJECT_SOURCE_DIR}/deps/umock-c)
endif()
include_directories(${UMOCK_C_INC_FOLDER})

set(lib_src_files
    ${PROJECT_SOURCE_DIR}/src/app_logging.c
    ${PROJECT_SOURCE_DIR}/src/alarm_timer.c
    ${PROJECT_SOURCE_DIR}/src/binary_encoder.c
    ${PROJECT_SOURCE_DIR}/src/binary_tree.c
    ${PROJECT_SOURCE_DIR}/src/buffer_alloc.c
    ${PROJECT_SOURCE_DIR}/src/crt_extensions.c
    ${PROJECT_SOURCE_DIR}/src/item_list.c
    ${PROJECT_SOURCE_DIR}/src/item_map.c
    ${PROJECT_SOURCE_DIR}/src/sys_debug_shim.c
)
set(lib_header_files
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/app_logging.h
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/alarm_timer.h
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/binary_encoder.h
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/binary_tree.h
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/buffer_alloc.h
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/crt_extensions.h
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/item_list.h
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/item_map.h
    ${PROJECT_SOURCE_DIR}/inc/lib-util-c/sys_debug_shim.h
)

include_directories(${PROJECT_SOURCE_DIR}/inc)

add_library(lib-util-c ${lib_src_files} ${lib_header_files})
target_include_directories(lib-util-c PUBLIC ${PROJECT_SOURCE_DIR}/inc/lib-util-c)
#target_link_libraries(my_library PUBLIC other_library)

addCompileSettings(lib-util-c)
compileTargetAsC99(lib-util-c)

# Add testing
if (${lib_util_c_ut})
    include("${CMAKE_CURRENT_LIST_DIR}/cmake_configs/proj_test.cmake")

    if ((NOT TARGET testrunnerswitcher) AND (EXISTS ${CMAKE_CURRENT_LIST_DIR}/deps/azure-c-testrunnerswitcher/CMakeLists.txt))
        add_subdirectory(${PROJECT_SOURCE_DIR}/deps/azure-c-testrunnerswitcher)
    endif()
    include_directories(${TESTRUNNERSWITCHER_INC_FOLDER})
    if ((NOT TARGET ctest) AND (EXISTS ${CMAKE_CURRENT_LIST_DIR}/deps/azure-ctest/CMakeLists.txt))
        add_subdirectory(${PROJECT_SOURCE_DIR}/deps/azure-ctest)
    endif()
    include_directories(${CTEST_INC_FOLDER})

    enable_testing()
    #enable_coverage_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/samples)
