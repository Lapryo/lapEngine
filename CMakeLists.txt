cmake_minimum_required(VERSION 4.1.2)
project(lapEngine)

set(CMAKE_CXX_STANDARD 23)          # or 17 / 23
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Require it (no fallback to older)
set(CMAKE_CXX_EXTENSIONS OFF)       # Turn off compiler-specific extensions like GNU++20

add_subdirectory(editor)
add_subdirectory(core)