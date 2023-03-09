make_minimum_required(VERSION 3.10)
      2 
      3 project(mt-server-with-shift C CXX)
      4 
      5 set(${PROJECT_NAME}_SRC main.cpp)
      6 
      7 source_group(source FILES ${${PROJECT_NAME}_SRC}})
      8 
      9 add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")
     10 target_link_libraries("${PROJECT_NAME}" socket-wrapper)
     11 
     12 if(WIN32)
     13   target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
     14 else()
     15   target_link_libraries("${PROJECT_NAME}" pthread)
     16 endif()
