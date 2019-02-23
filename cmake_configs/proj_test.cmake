#Licensed under the MIT license. See LICENSE file in the project root for full license information.

function(build_test_project whatIsBuilding folder)
    set(include_dir ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} ${UMOCK_C_INC_FOLDER})
    include_directories(${include_dir})

    if (WIN32)
        add_definitions(-DUNICODE)
        add_definitions(-D_UNICODE)
        #windows needs this define
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)

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
    target_include_directories(${whatIsBuilding}_exe PUBLIC ${include_dir})

    target_link_libraries(${whatIsBuilding}_exe micromock_ctest umock_c ctest m)
    add_test(NAME ${whatIsBuilding} COMMAND $<TARGET_FILE:${whatIsBuilding}_exe>)

    #if(${run_valgrind})
    #    find_program(VALGRIND_FOUND NAMES valgrind)
    #    if(${VALGRIND_FOUND} STREQUAL VALGRIND_FOUND-NOTFOUND)
    #        message(WARNING "run_valgrind was TRUE, but valgrind was not found - there will be no tests run under valgrind")
    #    else()
    #       add_test(NAME ${whatIsBuilding}_valgrind COMMAND valgrind                 --num-callers=100 --error-exitcode=1 --leak-check=full --track-origins=yes ${VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER} $<TARGET_FILE:${whatIsBuilding}_exe>)
    #       add_test(NAME ${whatIsBuilding}_helgrind COMMAND valgrind --tool=helgrind --num-callers=100 --error-exitcode=1 ${VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER} $<TARGET_FILE:${whatIsBuilding}_exe>)
    #       add_test(NAME ${whatIsBuilding}_drd      COMMAND valgrind --tool=drd      --num-callers=100 --error-exitcode=1 ${VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER} $<TARGET_FILE:${whatIsBuilding}_exe>)
    #   endif()
    #endif()
endfunction()

function(add_unittest_directory whatIsBuilding)
    add_subdirectory(${whatIsBuilding})
    add_test(NAME ${whatIsBuilding} COMMAND ${whatIsBuilding})

    if (WIN32)
    else()
        #add_test(NAME ${whatIsBuilding}_valgrind COMMAND valgrind --num-callers=10 --error-exitcode=1 --leak-check=full --track-origins=yes $TARGET_FILE:${whatIsBuilding})
    endif()
endfunction(add_unittest_directory)
