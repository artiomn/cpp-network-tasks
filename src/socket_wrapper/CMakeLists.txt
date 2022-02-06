cmake_minimum_required(VERSION 3.10)

project(socket-wrapper C CXX)

file(GLOB ${PROJECT_NAME}_SRC "src/*.*")
file(GLOB ${PROJECT_NAME}_HEADERS} "include/*.h")

source_group(source FILES ${${PROJECT_NAME}_SRC}})
source_group(headers FILES ${${PROJECT_NAME}_HEADERS}})

add_library("${PROJECT_NAME}" ${${PROJECT_NAME}_SRC})
target_include_directories("${PROJECT_NAME}" PUBLIC "include")

if(WIN32)
  target_link_libraries("${PROJECT_NAME}" PUBLIC wsock32 ws2_32)
endif()

