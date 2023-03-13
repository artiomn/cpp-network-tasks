cmake_minimum_required(VERSION 3.10)

project(chat_as_server C CXX)

find_package(Boost REQUIRED COMPONENTS system) # asio

set(SRC main.cpp)
set(HEADER chat_message.hpp)


#source_group(source FILES ${${PROJECT_NAME}_SRC}})

add_executable(chat_as_server ${SRC} ${HEADER})
target_include_directories("${PROJECT_NAME}" PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries("${PROJECT_NAME}" PRIVATE ${Boost_LIBRARIES} pthread)

if(WIN32)
  target_link_libraries("${PROJECT_NAME}" PRIVATE wsock32 ws2_32)
endif()
