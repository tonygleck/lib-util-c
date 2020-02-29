
cmake_minimum_required(VERSION 3.3.0)

set(lib_util_sample_files
    lib_util_sample.c
)

add_executable(lib_util_sample ${lib_util_sample_files})

target_link_libraries(lib_util_sample lib-util-c)
target_link_libraries(lib_util_sample rt)
