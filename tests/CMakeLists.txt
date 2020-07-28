#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.2)

add_unittest_directory(alarm_timer_ut)
add_unittest_directory(atomic_operations_ut)
add_unittest_directory(binary_tree_ut)
add_unittest_directory(binary_encoder_ut)
add_unittest_directory(buffer_alloc_ut)
add_unittest_directory(crt_extensions_ut)
add_unittest_directory(item_list_ut)
add_unittest_directory(item_map_ut)
add_unittest_directory(threadpool_ut)

if(WIN32)
    add_unittest_directory(mutex_mgr_win32_ut)
else()
    add_unittest_directory(condition_mgr_posix_ut)
    add_unittest_directory(mutex_mgr_posix_ut)
    add_unittest_directory(thread_mgr_posix_ut)
endif()