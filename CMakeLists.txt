set(P_NAME "sews")
cmake_minimum_required(VERSION 3.10)
project(${P_NAME})

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES src/*.cpp)

if(EXISTS "${CMAKE_SOURCE_DIR}/app/entry.cpp")
    message(STATUS "Using user-defined app: app/entry.cpp")
    list(APPEND SOURCES app/entry.cpp)
else()
  message( FATAL_ERROR "${CMAKE_SOURCE_DIR}/app/entry.cpp does not exists." )
endif()

add_executable(${P_NAME} ${SOURCES})
target_include_directories(${P_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
