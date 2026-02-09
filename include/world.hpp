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
    struct World
    {
        World(Project project) : project(project) {}

        Scene main_scene;

        rl::Vector2 logicalResolution;
        rl::RenderTexture target;

        ResourceManager resources;

        void LoadSettings(const std::string &settingsFilePath);
        void SetScene(ProjectSceneData &scene_data);

    private:
        Project project;
    };
}

#endif