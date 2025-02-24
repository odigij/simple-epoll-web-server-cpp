cmake_minimum_required(VERSION 3.10)
project(sews)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# export MY_INCLUDE_DIR="/path/to/extra/includes"
# export MY_COMPILE_DEFINES="-DCUSTOM_FLAG"

if(DEFINED ENV{MY_INCLUDE_DIR})
    message(STATUS "Using extra include directory from MY_INCLUDE_DIR: $ENV{MY_INCLUDE_DIR}")
    include_directories($ENV{MY_INCLUDE_DIR})
endif()

if(DEFINED ENV{MY_COMPILE_DEFINES})
    message(STATUS "Using compile definitions from MY_COMPILE_DEFINES: $ENV{MY_COMPILE_DEFINES}")
    add_definitions($ENV{MY_COMPILE_DEFINES})
endif()

set(SOURCES src/main.cpp)

add_executable(sews ${SOURCES})

if(MSVC)
    target_compile_options(sews PRIVATE /W4)
else()
    target_compile_options(sews PRIVATE -Wall -Wextra -pedantic)
endif()
