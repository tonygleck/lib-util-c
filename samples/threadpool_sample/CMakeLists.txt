
cmake_minimum_required(VERSION 3.3.0)

set(threadpool_sample_files
    threadpool_sample.c
)

add_executable(threadpool_sample ${threadpool_sample_files})

target_link_libraries(threadpool_sample lib-util-c)
target_link_libraries(threadpool_sample rt)
