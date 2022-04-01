cmake_minimum_required(VERSION 3.10)

project(cpp-network-lesson-7)

if (UNIX)
    add_subdirectory("lwip_example")
    add_subdirectory("cpp-httplib-example")
    add_subdirectory("curl-example")
    add_subdirectory("restbed-example")
    add_subdirectory("tg-arm-bot")
endif()