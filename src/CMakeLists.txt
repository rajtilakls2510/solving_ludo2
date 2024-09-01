cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(solving_ludo2)

set(CMAKE_PREFIX_PATH "/usr/local/bin")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_CURRENT_LIST_DIR}/../extern/libtorch")

add_subdirectory(proto)
add_subdirectory(backend)