cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

################################################################
## Target

set(TARGET_NAME hook-dinput)

################################################################
## Dependencies
include(FetchContent)

# Detours
find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

# memory-reader
FetchContent_Declare(
  memory-reader
  GIT_REPOSITORY https://github.com/UnnamedOrange/memory-reader
  GIT_TAG v0.3.0
)
FetchContent_MakeAvailable(memory-reader)

################################################################

file(
  GLOB_RECURSE
  SOURCES
  CONFIGURE_DEPENDS
  "src/*"
)

add_library(${TARGET_NAME} SHARED)
target_compile_definitions(${TARGET_NAME} PRIVATE UNICODE _UNICODE)
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
target_compile_options(${TARGET_NAME} PRIVATE /W4 /permissive /WX)
# set_target_properties(${TARGET_NAME} PROPERTIES
#   MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
# )
target_sources(${TARGET_NAME} PRIVATE "${SOURCES}")

target_include_directories(${TARGET_NAME} PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PRIVATE ${DETOURS_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE memory-reader)
