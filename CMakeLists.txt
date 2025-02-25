cmake_minimum_required(VERSION 3.10)
project(sews)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

include_directories(src/includes)

file(GLOB SOURCES src/*.cpp)

add_executable(sews ${SOURCES})

if(MSVC)
    target_compile_options(sews PRIVATE /W4)
else()
    target_compile_options(sews PRIVATE -Wall -Wextra -pedantic)
endif()

