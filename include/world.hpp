#pragma once

#include "project/project.hpp"
#include "resourcemanager.hpp"
#include "scene.hpp"
#include "actionmap.hpp"

// This file will completely refactor the project structure
// "Worlds" will take in a project, and they will own the ECS, systems, and runtime state
// Projects will just be data containers that can be loaded into worlds (JSONs converted to code (vectors, unordered_maps, etc))

// App (container) -> Project (data) -> World (runtime) -> Scene (data) -> ECS (data + runtime)

// EACH SCENE NEEDS A REFERENCE TO THE WORLD IN THE FORM OF A POINTER

namespace lapCore
{
    class App;
    struct ResourceManager;

    struct WindowProperties
    {
        string title = "Default Window";

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
            prefabs = new Scene(this, ProjectSceneData(), Scene::LoadMode::ACTIVE);
            for (const auto& prefabData : project.prefabs)
            {
                prefabs->AddObject(prefabData);
            }
        }

        Scene* mainScene = nullptr;
        Scene* prefabs = nullptr;

        unordered_map<id_type, Scene*> staleScenes;

        WindowProperties window;
        ResourceManager resources;

        void LoadAssets();
        void LoadSettings(const string &settingsFilePath);

        void LoadWindow();
        void ResetWindowProperties();

        void LoadScenes(id_type mainID);
        void SetScene(ProjectSceneData &scene_data);

        ProjectSceneData* GetSceneData(id_type sceneID);
        void SwitchScene(Scene* scene, function<void()> preloadCallback);
        void SwitchScene(const ProjectSceneData &scene_data, function<void()> preloadCallback);

        Scene* GetScene(id_type sceneID);
        void SwitchScene(id_type sceneID, function<void()> preloadCallback);

        const Project &GetProject() const {
            return project;
        }

        bool switchingScene = false;
        ProjectSceneData nextSceneData;

        function<void()> preloadSceneCallback = nullptr;

        unordered_map<string, ActionMap> actionMaps;
        ActionMap* currentActionMap = nullptr;

    private:
        Project project;

        bool debugMenu = false;
    };
}