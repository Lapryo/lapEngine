#ifndef WORLD_HPP
#define WORLD_HPP

#include "project.hpp"
#include "resourcemanager.hpp"
#include "scene.hpp"
#include "reflection.hpp"

#include <memory>

// This file will completely refactor the project structure
// "Worlds" will take in a project, and they will own the ECS, systems, and runtime state
// Projects will just be data containers that can be loaded into worlds (JSONs converted to code (vectors, unordered_maps, etc))

// App (container) -> Project (data) -> World (runtime) -> Scene (data) -> ECS (data + runtime)

// EACH SCENE NEEDS A REFERENCE TO THE WORLD IN THE FORM OF A POINTER

namespace lapCore
{
    class App;

    struct WindowProperties
    {
        std::string title = "Default Window";

        Vector2 resolution = {1200, 900};
        Vector2 logical_resolution = {800, 600};

        bool fullscreen = false;
        bool borderless = false;
        bool decorated = true;
        bool resizable = true;

        bool vsync = false;
        bool infinite_fps = true;
        unsigned int max_fps = 60;

        RenderTexture target;
    };

    struct World
    {
        App* app;

        World(App* app, Project project) : project(project), app(app) {
            prefabs = new ObjectContainer();
            prefabs->AddObjectsFromProjectData(project.prefabs);
        }

        Scene* mainScene = nullptr;
        ObjectContainer* prefabs = nullptr;

        WindowProperties window;
        ResourceManager resources;

        void LoadAssets();
        void LoadSettings(const std::string &settingsFilePath);

        void LoadWindow();
        void ResetWindowProperties();

        void SetScene(ProjectSceneData &scene_data);
        ProjectSceneData &GetMainSceneData();

        const Project &GetProject() const {
            return project;
        }

    private:
        Project project;
    };
}

#endif