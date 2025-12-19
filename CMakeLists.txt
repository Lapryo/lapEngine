cmake_minimum_required(VERSION 4.1.2)
project(lapCore)

set(CMAKE_CXX_STANDARD 23)

# Find all source files in the source directory
file(GLOB_RECURSE CORE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

# Source files
add_library(lapCore STATIC ${CORE_SOURCES})

# Include directories
target_include_directories(lapCore PRIVATE 
    include
)