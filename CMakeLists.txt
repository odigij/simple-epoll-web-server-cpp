cmake_minimum_required(VERSION 3.10)
project(sews VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
  message(FATAL_ERROR "OpenSSL not found. Install it by a suitable package manager.")
endif()

find_package(fmt REQUIRED)
if(NOT fmt_FOUND)
  message(FATAL_ERROR "fmt not found. Install it by a suitable package manager.")
endif()

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

if(EXISTS "${CMAKE_SOURCE_DIR}/app/entry.cpp")
  file(GLOB APP_SOURCES ${CMAKE_SOURCE_DIR}/app/*.cpp)
  list(APPEND SOURCES ${APP_SOURCES})
else()
  message(FATAL_ERROR "app/entry.cpp is missing.")
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/assets")
  message(FATAL_ERROR "assets directory is missing!")
endif()

set(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/external
)

target_link_libraries(${PROJECT_NAME}
    OpenSSL::SSL
    OpenSSL::Crypto
    fmt::fmt
)
