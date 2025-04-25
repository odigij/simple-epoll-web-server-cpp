cmake_minimum_required(VERSION 3.10)
project(sews VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")

find_package(OpenSSL)
if(NOT OpenSSL_FOUND)
  message(WARNING "Package not found: OpenSSL")
else()
  message("Package found: OpenSSL")
endif()
find_package(fmt)
if(NOT fmt_FOUND)
  message(WARNING "Package not found: fmt")
else()
  message("Package found: fmt")
endif()
file(GLOB_RECURSE ALL_SOURCE ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE APP_SOURCE ${CMAKE_SOURCE_DIR}/app/*.cpp)
list(APPEND ALL_SOURCE ${APP_SOURCE})

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/assets")
  message(WARNING "Directory does not exists: ${CMAKE_SOURCE_DIR}/assets")
endif()

add_executable(${PROJECT_NAME} ${ALL_SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/external
)
if(OpenSSL_FOUND)
  target_link_libraries(${PROJECT_NAME}
    OpenSSL::SSL
    OpenSSL::Crypto
  )
endif()
if(fmt_FOUND)
  target_link_libraries(${PROJECT_NAME}
    fmt::fmt
  )
endif()

enable_testing()
add_subdirectory(${TEST_DIR})
