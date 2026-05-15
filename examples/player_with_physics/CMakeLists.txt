# CMakeLists.txt for Flappy Bird example using lapEngine

# Setup CMake project
cmake_minimum_required(VERSION 4.1.2)
project(player_physics)

set(CMAKE_CXX_STANDARD 23)

# Source files
file(GLOB_RECURSE GAME_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp" # Edit game source path as needed
)

# Executable
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(player_physics 
        ${GAME_SOURCES}
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(WIN32)
        add_executable(player_physics # WIN32 # Hide console window
            ${GAME_SOURCES}
        )
    else()
        add_executable(player_physics 
            ${GAME_SOURCES}
        )
    endif()
endif()

# Include directories
target_include_directories(player_physics PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include" # Edit game include path as needed
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include" # Edit lapEngine include path as needed
)

# Library directories and linking
target_link_directories(player_physics PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/lib" # Edit library path as needed
)

if(WIN32)
    target_link_libraries(player_physics PRIVATE 
        lapEngine
    )
else()
    target_link_libraries(player_physics PRIVATE 
        lapEngine
    )
endif()