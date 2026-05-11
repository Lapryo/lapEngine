cmake_minimum_required(VERSION 4.1.2)
project(lapEngine)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG main
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
)
FetchContent_MakeAvailable(raylib)

# Find all source files in the source directory
file(GLOB_RECURSE CORE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

# Source files
add_library(lapEngine STATIC ${CORE_SOURCES})

# Include directories
target_include_directories(lapEngine PUBLIC 
    include
)

target_link_libraries(lapEngine PUBLIC
    raylib
    box2d
)

add_subdirectory(examples/flappy_bird)
add_subdirectory(examples/blank_window)