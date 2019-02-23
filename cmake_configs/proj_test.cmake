#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

function(add_unittest_directory test_directory)
    if (${run_unittests})
        add_subdirectory(${test_directory})
    endif()
endfunction()

function(build_test_project whatIsBuilding folder)

    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)

    set(test_include_dir ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} ${UMOCK_C_INC_FOLDER})
    include_directories(${test_include_dir})

    if (WIN32)
        add_definitions(-DUNICODE)
        add_definitions(-D_UNICODE)
        #windows needs this define
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)

        #add_executable(${whatIsBuilding}
        #    ${${whatIsBuilding}_cpp_files}
        #    ${${whatIsBuilding}_h_files}
        #)
        set_target_properties(${whatIsBuilding} PROPERTIES LINKER_LANGUAGE CXX)
        set_target_properties(${whatIsBuilding} PROPERTIES FOLDER ${folder})
    endif()

    add_executable(${whatIsBuilding}_exe
        ${${whatIsBuilding}_test_files}
        ${${whatIsBuilding}_cpp_files}
        ${${whatIsBuilding}_h_files}
        ${${whatIsBuilding}_c_files}
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${logging_files}
    )

    set_target_properties(${whatIsBuilding}_exe
               PROPERTIES
               FOLDER ${folder})

    target_compile_definitions(${whatIsBuilding}_exe PUBLIC -DUSE_CTEST)
    target_include_directories(${whatIsBuilding}_exe PUBLIC ${test_include_dir})

    target_link_libraries(${whatIsBuilding}_exe umock_c ctest testrunnerswitcher m)
    add_test(NAME ${whatIsBuilding} COMMAND $<TARGET_FILE:${whatIsBuilding}_exe>)

endfunction()
