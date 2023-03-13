cmake_minimum_required(VERSION 3.10)

project(home_work_5 C CXX)



if (UNIX)
    add_subdirectory(asio_server)
    add_subdirectory(asio_client)
    add_subdirectory(chat_as_client)
    add_subdirectory(chat_as_server)
    add_subdirectory(HTTP_server)
endif()

