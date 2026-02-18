#ifndef WORLD_HPP
#define WORLD_HPP

#include "new_project.hpp"
#include "resource_manager.hpp"
#include "scene.hpp"

#include <memory>

// This file will completely refactor the project structure
// "Worlds" will take in a project, and they will own the ECS, systems, and runtime state
// Projects will just be data containers that can be loaded into worlds (JSONs converted to code (vectors, unordered_maps, etc))

// App (container) -> Project (data) -> World (runtime) -> Scene (data) -> ECS (data + runtime)

// EACH SCENE NEEDS A REFERENCE TO THE WORLD IN THE FORM OF A POINTER

namespace lapCore
{
    struct WindowProperties
    {
        std::string title = "Default Window";

        rl::Vector2 resolution = {1200, 900};
        rl::Vector2 logical_resolution = {800, 600};

        bool fullscreen = false;
        bool borderless = false;
        bool decorated = true;
        bool resizable = true;

        bool vsync = false;
        bool infinite_fps = true;
        unsigned int max_fps = 60;

        rl::RenderTexture target;
    };

    struct World
    {
        World(Project project) : project(project) {}

        Scene main_scene;

        WindowProperties window;
        ResourceManager resources;

        void LoadSettings(const std::string &settingsFilePath);

        void LoadWindow();
        void ResetWindowProperties();

        void SetScene(ProjectSceneData &scene_data);

        const Project &GetProject() const {
            return project;
        }

    private:
        Project project;
    };
}

#endif