cmake_minimum_required(VERSION 4.1.0)
project(lapEditor)

# Set output directories to match Makefile structure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/editor)

# Find all source files in the source directory
file(GLOB_RECURSE EDITOR_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

# Source files
add_executable(lapEditor 
    ${EDITOR_SOURCES}
)

# Include directories
target_include_directories(lapEditor PRIVATE 
    include
    ../core/include
)

# Library directories and linking
target_link_directories(lapEditor PRIVATE 
    ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(lapEditor PRIVATE 
    lapCore
)