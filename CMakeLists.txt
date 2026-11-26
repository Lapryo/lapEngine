cmake_minimum_required(VERSION 3.28)
project(lapEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

option(INCLUDE_EXAMPLES "Build lapEngine examples" ${PROJECT_IS_TOP_LEVEL})

# ============================================================
# Dependency Fetching
# ============================================================

function(fetch_library name repository branch)
    string(TOUPPER "${name}" upper_name)

    # Always check for upstream updates when configuring.
    set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
    set(FETCHCONTENT_UPDATES_DISCONNECTED_${upper_name} OFF)
    set(FETCHCONTENT_FULLY_DISCONNECTED OFF)

    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${repository}
        GIT_TAG origin/${branch}
        GIT_SHALLOW TRUE
        GIT_SUBMODULES ""
    )

    FetchContent_MakeAvailable(${name})
endfunction()

function(fetch_dependencies)

    # ========================================================
    # Box2D
    # ========================================================

    set(BOX2D_SAMPLES OFF)
    set(BOX2D_UNIT_TESTS OFF)
    set(BOX2D_BENCHMARKS OFF)
    set(BOX2D_DOCS OFF)

    FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v3.2.0
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(box2d)

    if(NOT TARGET box2d)
        message(FATAL_ERROR "Failed to configure Box2D.")
    endif()

    # ========================================================
    # raylib
    # ========================================================

    set(BUILD_EXAMPLES OFF)
    set(BUILD_SHARED_LIBS OFF)

    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL OFF)

    fetch_library(
        raylib
        https://github.com/raysan5/raylib.git
        master
    )

    # ========================================================
    # cereal
    # ========================================================

    set(JUST_INSTALL_CEREAL ON)
    set(CEREAL_INSTALL OFF)
    set(BUILD_DOC OFF)
    set(BUILD_SANDBOX OFF)
    set(BUILD_TESTS OFF)
    set(SKIP_PORTABILITY_TEST ON)
    set(SKIP_PERFORMANCE_COMPARISON ON)

    fetch_library(
        cereal
        https://github.com/USCiLab/cereal.git
        master
    )

    # ========================================================
    # EnTT
    # ========================================================

    set(ENTT_BUILD_TESTING OFF)
    set(ENTT_BUILD_TESTBED OFF)
    set(ENTT_BUILD_EXAMPLE OFF)
    set(ENTT_BUILD_BENCHMARK OFF)
    set(ENTT_BUILD_LIB OFF)
    set(ENTT_BUILD_SNAPSHOT OFF)
    set(ENTT_BUILD_DOCS OFF)
    set(ENTT_INSTALL OFF)

    fetch_library(
        entt
        https://github.com/skypjack/entt.git
        main
    )

    # ========================================================
    # nlohmann JSON
    # ========================================================

    set(JSON_BuildTests OFF)
    set(JSON_Install OFF)
    set(JSON_CI OFF)
    set(NLOHMANN_JSON_BUILD_MODULES OFF)

    fetch_library(
        nlohmann_json
        https://github.com/nlohmann/json.git
        develop
    )

    # ========================================================
    # Dear ImGui
    # ========================================================

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")

        fetch_library(
            imgui
            https://github.com/ocornut/imgui.git
            docking
        )

        FetchContent_GetProperties(imgui)

        add_library(imgui STATIC
            "${imgui_SOURCE_DIR}/imgui.cpp"
            "${imgui_SOURCE_DIR}/imgui_draw.cpp"
            "${imgui_SOURCE_DIR}/imgui_tables.cpp"
            "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        )

        target_include_directories(imgui PUBLIC
            "${imgui_SOURCE_DIR}"
        )

        # ====================================================
        # rlImGui
        # ====================================================

        fetch_library(
            rlimgui
            https://github.com/raylib-extras/rlImGui.git
            main
        )

        FetchContent_GetProperties(rlimgui)

        add_library(rlimgui STATIC
            "${rlimgui_SOURCE_DIR}/rlImGui.cpp"
        )

        target_include_directories(rlimgui PUBLIC
            "${rlimgui_SOURCE_DIR}"
        )

        target_link_libraries(rlimgui PUBLIC
            raylib
            imgui
        )

    endif()

endfunction()

fetch_dependencies()

# ============================================================
# lapEngine
# ============================================================

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

add_library(lapEngine STATIC ${CORE_SOURCES})

target_compile_features(lapEngine PUBLIC cxx_std_20)

target_include_directories(lapEngine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(lapEngine PUBLIC
    raylib
    box2d
    nlohmann_json::nlohmann_json
    EnTT::EnTT
    cereal::cereal
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(lapEngine PUBLIC rlimgui)
    target_compile_definitions(lapEngine PUBLIC LAP_DEBUG)
endif()

# macOS libc++ compatibility
if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(lapEngine PRIVATE
        "SHELL:-include nlohmann/json.hpp"
    )
endif()

# ============================================================
# lapEngine Examples
# ============================================================

if(INCLUDE_EXAMPLES)
    add_subdirectory(examples/blank_window)
    add_subdirectory(examples/player_with_physics)
    add_subdirectory(examples/stress_test)
    add_subdirectory(examples/prefab_children)
endif()