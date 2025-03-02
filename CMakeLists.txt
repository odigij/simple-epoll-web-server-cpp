set(P_NAME "sews")
cmake_minimum_required(VERSION 3.10)
project(${P_NAME})

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/args.cpp
    src/signal.cpp
    src/request.cpp
    src/response.cpp
    src/router.cpp
    src/server.cpp
    src/main.cpp
)

if(EXISTS "${CMAKE_SOURCE_DIR}/app/entry.cpp")
    message(STATUS "Using user-defined app: app/entry.cpp")
    list(APPEND SOURCES app/entry.cpp)
    add_compile_definitions(APP_EXISTS)
else()
    message(STATUS "Using default app: src/predefined_entry.cpp")
    list(APPEND SOURCES src/predefined_entry.cpp)
endif()

add_executable(${P_NAME} ${SOURCES})
target_include_directories(${P_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
