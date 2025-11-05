#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include "raylib/raylib.h"
#include <unordered_map>
#include <string>

namespace lapCore
{
    struct ResourceManager
    {
        std::unordered_map<std::string, Texture2D> textures;
        std::unordered_map<std::string, Shader> shaders;

        Texture2D AddTexture(const std::string &name, const std::string &filePath);
        void RemoveTexture(const std::string &name);

        Shader AddShader(const std::string &name, const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
        void RemoveShader(const std::string &name);
    };
}

#endif