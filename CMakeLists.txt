set(P_NAME "sews")
cmake_minimum_required(VERSION 3.10)
project(${P_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Install it with: sudo apt install libssl-dev")
endif()

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

if(EXISTS "${CMAKE_SOURCE_DIR}/app/entry.cpp")
    message(STATUS "Including: app/entry.cpp")
    file(GLOB APP_SOURCES ${CMAKE_SOURCE_DIR}/app/*.cpp)
    list(APPEND SOURCES ${APP_SOURCES})
else()
    message(WARNING "app/entry.cpp is missing. Your app may not function correctly.")
endif()

add_executable(${P_NAME} ${SOURCES})

target_include_directories(${P_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${P_NAME} OpenSSL::SSL OpenSSL::Crypto)
