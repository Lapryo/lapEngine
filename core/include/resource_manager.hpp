#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include "raylib/raylib_namespace.h"

#include <unordered_map>
#include <string>

namespace lapCore
{
    struct ResourceManager
    {
        std::unordered_map<std::string, rl::Texture2D> textures;
        std::unordered_map<std::string, rl::Shader> shaders;

        rl::Texture2D *AddTexture(const std::string &name, const std::string &filePath);
        void RemoveTexture(const std::string &name);

        rl::Shader *AddShader(const std::string &name, const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
        void RemoveShader(const std::string &name);
    };
}

#endif