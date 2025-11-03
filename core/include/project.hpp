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
        std::string path;
        Scene main_scene;
        std::vector<Scene> scenes;
    };

    void PackProject(Project& project);
    Project UnpackProject(const std::string& projectPath);
}

#endif