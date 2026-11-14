#pragma once

#include "project/project.hpp"
#include "resourcemanager.hpp"
#include "scene.hpp"
#include "actionmap.hpp"

// World owns its scene pointers. `mainScene` always refers to one element of
// `scenes`; switching scenes does not reconstruct entities or systems.

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

        World(App* app, Project project) : app(app), project(std::move(project))
        {
            prefabs = new Scene(this);
            for (const auto& prefabData : this->project.prefabs)
                prefabs->AddObject(prefabData);
        }

        ~World();
        World(const World&) = delete;
        World& operator=(const World&) = delete;

        // Owned scene pointers. Custom scenes must be registered here too.
        vector<Scene*> scenes;
        Scene* mainScene = nullptr;
        Scene* prefabs = nullptr;

        WindowProperties window;
        ResourceManager resources;

        void LoadAssets();
        void LoadSettings(const string &settingsFilePath);

        void LoadWindow();
        void ResetWindowProperties();

        // Load scenes exactly once. All scene systems are initialized up front.
        void LoadScenes(id_type mainID);
        void ClearScenes();
        Scene* AddScene(Scene* scene);

        // Apply a pending scene switch (normally called by App after Update).
        void SetScene(Scene* scene);
        Scene* GetScene(id_type sceneID);

        void SwitchScene(Scene* scene, function<void()> preloadCallback = nullptr);
        void SwitchScene(id_type sceneID, function<void()> preloadCallback = nullptr);
        void SwitchScene(const ProjectSceneData& scene_data,
                         function<void()> preloadCallback = nullptr);

        const Project &GetProject() const {
            return project;
        }

        bool switchingScene = false;
        Scene* pendingScene = nullptr;
        function<void()> preloadSceneCallback = nullptr;

        unordered_map<string, ActionMap> actionMaps;
        ActionMap* currentActionMap = nullptr;

    private:
        Project project;

        bool debugMenu = false;
    };
}