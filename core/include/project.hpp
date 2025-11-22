#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "scene.hpp"
#include <vector>
#include <string>

namespace lapCore
{
    struct Project
    {
        std::string name;
        std::string version;
        std::string path;
        int main_scene_index;
        Scene *main_scene;
        std::vector<std::unique_ptr<Scene>> scenes;

        Vector2 logicalResolution;
        RenderTexture target;

        Scene *GetMainScene();
        void Clear();

        void LoadSettings(const std::string &settingsFilePath);
    };

    void PackProject(Project &project);
    Project UnpackProject(const char projJson[]);
}

#endif