cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(Engine)

file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB SRC_FILES_IMGUI "${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp")
list(APPEND SRC_FILES
  ${CMAKE_SOURCE_DIR}/vendor/directxtex/DDSTextureLoader/DDSTextureLoader11.cpp
  ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_dx11.cpp
  ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_win32.cpp
  ${SRC_FILES_IMGUI}
)

include_directories(
  ${CMAKE_SOURCE_DIR}/src/Engine
  ${CMAKE_SOURCE_DIR}/vendor/glm
  ${CMAKE_SOURCE_DIR}/vendor/assimp/include
  ${CMAKE_BINARY_DIR}/vendor/assimp/include
  ${CMAKE_SOURCE_DIR}/vendor/directxtex
  ${CMAKE_SOURCE_DIR}/vendor/imgui
)

add_library(${PROJECT_NAME} ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)
