# CMakeLists.txt for Flappy Bird example using lapEngine

# Setup CMake project
cmake_minimum_required(VERSION 4.1.2)
project(stress_test)

set(CMAKE_CXX_STANDARD 20)

# Source files
file(GLOB_RECURSE GAME_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp" # Edit game source path as needed
)

# Executable
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(stress_test 
        ${GAME_SOURCES}
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(WIN32)
        add_executable(stress_test # WIN32 # Hide console window
            ${GAME_SOURCES}
        )
    else()
        add_executable(stress_test 
            ${GAME_SOURCES}
        )
    endif()
endif()

# Include directories
target_include_directories(stress_test PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include" # Edit game include path as needed
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include" # Edit lapEngine include path as needed
)

if(WIN32)
    target_link_libraries(stress_test PRIVATE 
        lapEngine
    )
else()
    target_link_libraries(stress_test PRIVATE 
        lapEngine
    )
endif()