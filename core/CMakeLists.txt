cmake_minimum_required(VERSION 4.2.0)
project(lapCore)

# Find all source files in the source directory
file(GLOB_RECURSE CORE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/core")

# Source files
add_library(lapCore STATIC ${CORE_SOURCES})

# Include directories
target_include_directories(lapCore PRIVATE 
    include
)

# Library directories and linking
target_link_directories(lapCore PRIVATE 
    ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(lapCore PRIVATE 
    raylib
    winmm
)