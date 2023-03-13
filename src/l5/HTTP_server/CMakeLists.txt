cmake_minimum_required(VERSION 3.10)

project(HTTP_server_5 C CXX)

find_package(Boost REQUIRED COMPONENTS system) # asio

#set(${PROJECT_NAME}_SRC main.cpp)

#source_group(source FILES ${${PROJECT_NAME}_SRC}})

#add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")
set(SRC main.cpp connection.cpp connection_manager.cpp mime_types.cpp reply.cpp request_handler.cpp request_parser.cpp server.cpp)
set(HEADER connection.hpp connection_manager.hpp header.hpp mime_types.hpp reply.hpp request.hpp request_handler.hpp request_parser.hpp server.hpp)

add_executable("${PROJECT_NAME}" ${SRC} ${HEADER})
target_include_directories("${PROJECT_NAME}" PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries("${PROJECT_NAME}" PRIVATE ${Boost_LIBRARIES} pthread)

if(WIN32)
  target_link_libraries("${PROJECT_NAME}" PRIVATE wsock32 ws2_32)
endif()
