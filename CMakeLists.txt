cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(Flame)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(BUILD_SHARED_LIBS OFF)
set(GLM_BUILD_TESTS OFF)
# Assimp
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_INSTALL OFF)
# DirectXTex
set(BUILD_DX12 OFF)
set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)

add_definitions(
  -DUNICODE
  -DWIN32
  -DWIN32_LEAN_AND_MEAN
  -DNOMINMAX
  -DGLM_ENABLE_EXPERIMENTAL
)

add_subdirectory(vendor/glm)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/directxtex)
add_subdirectory(src/ConsoleLib)
add_subdirectory(src/Engine)
add_subdirectory(src/Project)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  message(SEND_ERROR "CMAKE_BUILD_TYPE is empty - can't copy resources.")
  message(SEND_ERROR "Set it manually or if you use VSCode try to set the generator to Ninja in Extensions -> CMakeTools -> Extension Settings -> Generator")
endif()

add_custom_target(copy_resources ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${PROJECT_SOURCE_DIR}/Assets" "${PROJECT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/Assets"
  COMMENT "Copying resources to the build directory..."
)
add_dependencies(Project copy_resources)
