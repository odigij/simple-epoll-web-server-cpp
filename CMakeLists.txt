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
    message(STATUS "Including: app/entry.cpp")
    list(APPEND SOURCES app/entry.cpp)

    if(EXISTS "${CMAKE_SOURCE_DIR}/app/handlers.cpp")
        message(STATUS "Including: app/handlers.cpp")
        list(APPEND SOURCES app/handlers.cpp)
    else()
        message(WARNING "app/handlers.cpp is missing. Your app may not function correctly.")
    endif()
else()
    message(FATAL_ERROR "Missing required: app/entry.cpp")
endif()

add_executable(${P_NAME} ${SOURCES})
target_include_directories(${P_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/app)
