cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(Project)

file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/*.cpp")

include_directories(
  ${CMAKE_SOURCE_DIR}/src/ConsoleLib
  ${CMAKE_SOURCE_DIR}/src/Engine
  ${CMAKE_SOURCE_DIR}/vendor/glm
  ${CMAKE_SOURCE_DIR}/vendor/assimp/include
  ${CMAKE_BINARY_DIR}/vendor/assimp/include
  ${CMAKE_SOURCE_DIR}/vendor/directxtex
  ${CMAKE_SOURCE_DIR}/vendor/imgui
)

add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE ConsoleLib
  PRIVATE Engine

  PRIVATE user32
  PRIVATE dxgi
  PRIVATE d3d11
  PRIVATE d3dcompiler

  PRIVATE glm
  PRIVATE assimp
  PRIVATE DirectXTex
)

