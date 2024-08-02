cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(ConsoleLib)

file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/*.cpp")

include_directories(
  ${CMAKE_SOURCE_DIR}/src/ConsoleLib
)

add_library(${PROJECT_NAME} ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>
)
