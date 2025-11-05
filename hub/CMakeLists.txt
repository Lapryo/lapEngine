cmake_minimum_required(VERSION 4.1.0)
project(lapHub)

# Set output directories to match Makefile structure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/hub)

# Find all source files in the source directory
file(GLOB_RECURSE HUB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

# Source files
add_executable(lapHub 
    ${HUB_SOURCES}
)

# Include directories
target_include_directories(lapHub PRIVATE 
    include
    ../core/include
)

# Library directories and linking
target_link_directories(lapHub PRIVATE 
    ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(lapHub PRIVATE 
    lapCore
)