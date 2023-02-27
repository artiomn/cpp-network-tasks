cmake_minimum_required(VERSION 3.5)

project(udp_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(udp_client udp_client.cpp)
