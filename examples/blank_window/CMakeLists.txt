# Setup CMake project
cmake_minimum_required(VERSION 4.1.2)
project(blank_window)

set(CMAKE_CXX_STANDARD 23)

# Source files
file(GLOB_RECURSE GAME_SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp" # Edit game source path as needed
)

# Executable
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_executable(blank_window 
		${GAME_SOURCES}
	)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	if(WIN32)
		add_executable(blank_window WIN32 # Hide console window
			${GAME_SOURCES}
		)
	else()
		add_executable(blank_window 
			${GAME_SOURCES}
		)
	endif()
endif()

# Include directories
target_include_directories(blank_window PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/include" # Edit game include path as needed

	# YOU MAY NEED TO ADJUST THIS PATH TO POINT TO THE LAPENGINE INCLUDE DIRECTORY
	"${CMAKE_CURRENT_SOURCE_DIR}/../../include" # Edit lapEngine include path as needed
)

if(WIN32)
	target_link_libraries(blank_window PRIVATE 
		lapEngine
		box2d
		raylib
		winmm
	)
else()
	target_link_libraries(blank_window PRIVATE 
		lapEngine
		box2d
		raylib
	)
endif()
