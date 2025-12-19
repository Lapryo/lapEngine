#include "resource_manager.hpp"

using namespace lapCore;

rl::Texture2D *ResourceManager::AddTexture(const std::string &name, const std::string &filePath)
{
    auto tex = rl::LoadTexture(filePath.c_str());
    textures[name] = tex;
    return &textures[name];
}

void ResourceManager::RemoveTexture(const std::string &name)
{
    textures.erase(name);
}

rl::Shader *ResourceManager::AddShader(const std::string &name, const std::string &vertexFilePath, const std::string &fragmentFilePath)
{
    auto shader = rl::LoadShader(vertexFilePath.c_str(), fragmentFilePath.c_str());
    shaders[name] = shader;
    return &shaders[name];
}

void ResourceManager::RemoveShader(const std::string &name)
{
    shaders.erase(name);
}