#include "resource_manager.hpp"

using namespace lapCore;

Texture2D* ResourceManager::AddTexture(const std::string &name, const std::string &filePath)
{
    auto tex = LoadTexture(filePath.c_str());
    textures[name] = tex;
    return &textures[name];
}

void ResourceManager::RemoveTexture(const std::string &name)
{
    textures.erase(name);
}

Shader* ResourceManager::AddShader(const std::string &name, const std::string &vertexFilePath, const std::string &fragmentFilePath)
{
    Shader shader = LoadShader(vertexFilePath.c_str(), fragmentFilePath.c_str());
    shaders[name] = shader;
    return &shaders[name];
}

void ResourceManager::RemoveShader(const std::string &name)
{
    shaders.erase(name);
}