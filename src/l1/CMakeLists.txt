cmake_minimum_required(VERSION 3.10)

project(cpp-network-lesson-1 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test_byte_order test_byte_order.cpp)

add_subdirectory(resolve)
add_subdirectory(udp_server)
add_subdirectory(ping)

