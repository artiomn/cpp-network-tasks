cmake_minimum_required(VERSION 3.10)

project(OpenSSL_Client_Server_6)

find_package(OpenSSL)

if (OpenSSL_FOUND)
    add_subdirectory("openssl-client")
endif()

if (UNIX)
    add_subdirectory("openssl-socket-client")
    add_subdirectory("openssl-socket-server")
endif()
