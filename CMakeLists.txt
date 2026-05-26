cmake_minimum_required(VERSION 4.1.2)
project(lapEngine)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    reactphysics3d
    GIT_REPOSITORY https://github.com/DanielChappuis/reactphysics3d.git
    GIT_TAG v0.10.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(reactphysics3d)

FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.16.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(entt)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

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
    reactphysics3d
    nlohmann_json::nlohmann_json
    EnTT::EnTT
)
