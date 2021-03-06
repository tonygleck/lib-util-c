# Licensed under the MIT license. See LICENSE file in the project root for full license information.

function(addCompileSettings theTarget)
    if (MSVC)
        target_compile_options(${theTarget} PRIVATE -W4 /WX -D_CRT_SECURE_NO_WARNINGS)
        target_compile_definitions(${theTarget} PRIVATE -D_CRT_SECURE_NO_WARNINGS)
        # Make warning as error
        #add_definitions(/WX)
    else()
        target_compile_options(${theTarget} PRIVATE -Wall -Werror -Wextra -Wshadow -fPIC)

        if (${DEBUG_CONFIG})
            target_compile_options(${theTarget} PRIVATE -O0)
        else()
            target_compile_options(${theTarget} PRIVATE -O3)
        endif()
    endif()
endfunction()

include(CheckSymbolExists)
function(detect_architecture symbol arch)
    if (NOT DEFINED ARCHITECTURE OR ARCHITECTURE STREQUAL "")
        set(CMAKE_REQUIRED_QUIET 1)
        check_symbol_exists("${symbol}" "" ARCHITECTURE_${arch})
        unset(CMAKE_REQUIRED_QUIET)

        # The output variable needs to be unique across invocations otherwise
        # CMake's crazy scope rules will keep it defined
        if (ARCHITECTURE_${arch})
            set(ARCHITECTURE "${arch}" PARENT_SCOPE)
            set(ARCHITECTURE_${arch} 1 PARENT_SCOPE)
            add_definitions(-DARCHITECTURE_${arch}=1)
        endif()
    endif()
endfunction()

function(compileTargetAsC99 theTarget)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set_target_properties(${theTarget} PROPERTIES COMPILE_FLAGS "--std=c99")
    endif()
  else()
    set_target_properties(${theTarget} PROPERTIES C_STANDARD 99)
    set_target_properties(${theTarget} PROPERTIES CXX_STANDARD 11)
  endif()
endfunction()

function(compileTargetAsC11 theTarget)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      if (CXX_FLAG_CXX11)
        set_target_properties(${theTarget} PROPERTIES COMPILE_FLAGS "--std=c11 -D_POSIX_C_SOURCE=200112L")
      else()
        set_target_properties(${theTarget} PROPERTIES COMPILE_FLAGS "--std=c99 -D_POSIX_C_SOURCE=200112L")
      endif()
    endif()
  else()
    set_target_properties(${theTarget} PROPERTIES C_STANDARD 11)
    set_target_properties(${theTarget} PROPERTIES CXX_STANDARD 11)
  endif()
endfunction()
