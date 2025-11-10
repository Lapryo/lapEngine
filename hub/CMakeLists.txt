cmake_minimum_required(VERSION 4.2.0)
project(lapHub)

# Set output directories to match Makefile structure
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building with debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/hub/debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Building with release")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/hub/release)
endif()

# Find all source files in the source directory
file(GLOB_RECURSE HUB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

# Source files
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(lapHub 
        ${HUB_SOURCES}
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(lapHub WIN32 
        ${HUB_SOURCES}
    )
endif()

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