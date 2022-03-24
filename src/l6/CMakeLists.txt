cmake_minimum_required(VERSION 3.10)

project(cpp-network-lesson-6)

find_package(OpenSSL)

if (OpenSSL_FOUND)
    add_subdirectory("openssl-client")
endif()

if (UNIX)
    add_subdirectory("openssl-socket-client")
    add_subdirectory("jwt-cpp-example")
endif()
