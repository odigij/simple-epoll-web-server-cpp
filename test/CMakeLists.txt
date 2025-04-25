project(sews_test VERSION 1.0)

file(GLOB_RECURSE TEST_SOURCE CONFIGURE_DEPENDS ${TEST_DIR}/*.cpp)
file(GLOB_RECURSE CORE_SOURCE CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

if(NOT TEST_SOURCE)
  message(FATAL_ERROR "No test sources found in ${TEST_DIR}")
endif()

add_executable(${PROJECT_NAME} ${TEST_SOURCE} ${CORE_SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_ROOT_DIR}/include
  ${TEST_DIR}/include
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
