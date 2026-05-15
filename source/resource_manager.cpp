#include "resource_manager.hpp"
#include <algorithm>

using namespace lapCore;

Texture2D *ResourceManager::AddTexture(const std::string &name, const std::string &filePath, std::vector<std::string> data)
{
    auto tex = LoadTexture(filePath.c_str());

    if (std::find(data.begin(), data.end(), "gen-mipmaps") != data.end())
        GenTextureMipmaps(&tex);

    if (std::find(data.begin(), data.end(), "point-filter") != data.end())
        SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    else if (std::find(data.begin(), data.end(), "bilinear-filter") != data.end())
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
    else if (std::find(data.begin(), data.end(), "trilinear-filter") != data.end())
        SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
    else if (std::find(data.begin(), data.end(), "anisotropic-filter-4x") != data.end())
        SetTextureFilter(tex, TEXTURE_FILTER_ANISOTROPIC_4X);
    else if (std::find(data.begin(), data.end(), "anisotropic-filter-8x") != data.end())
        SetTextureFilter(tex, TEXTURE_FILTER_ANISOTROPIC_8X);
    else if (std::find(data.begin(), data.end(), "anisotropic-filter-16x") != data.end())
        SetTextureFilter(tex, TEXTURE_FILTER_ANISOTROPIC_16X);

    textures[name] = tex;
    return &textures[name];
}

void ResourceManager::RemoveTexture(const std::string &name)
{
    UnloadTexture(textures[name]);
    textures.erase(name);
}

void lapCore::ResourceManager::ClearTextures()
{
    for (auto &pair : textures)
        UnloadTexture(pair.second);
    textures.clear();
}

Shader *ResourceManager::AddShader(const std::string &name, const std::string &vertexFilePath, const std::string &fragmentFilePath, std::vector<std::string> data)
{
    auto shader = LoadShader(vertexFilePath.c_str(), fragmentFilePath.c_str());

    shaders[name] = shader;
    return &shaders[name];
}

void ResourceManager::RemoveShader(const std::string &name)
{
    UnloadShader(shaders[name]);
    shaders.erase(name);
}

void lapCore::ResourceManager::ClearShaders()
{
    for (auto &pair : shaders)
        UnloadShader(pair.second);
    shaders.clear();
}

Music *lapCore::ResourceManager::AddMusic(const std::string &name, const std::string &filePath, std::vector<std::string> data)
{
    auto musicStream = LoadMusicStream(filePath.c_str());

    music[name] = musicStream;
    return &music[name];
}

void lapCore::ResourceManager::RemoveMusic(const std::string &name)
{
    UnloadMusicStream(music[name]);
    music.erase(name);
}

void lapCore::ResourceManager::ClearMusic()
{
    for (auto &pair : music)
        UnloadMusicStream(pair.second);
    music.clear();
}

Sound *lapCore::ResourceManager::AddSound(const std::string &name, const std::string &filePath, std::vector<std::string> data)
{
    auto sound = LoadSound(filePath.c_str());

    sounds[name] = sound;
    return &sounds[name];
}

void lapCore::ResourceManager::RemoveSound(const std::string &name)
{
    UnloadSound(sounds[name]);
    sounds.erase(name);
}

void lapCore::ResourceManager::ClearSounds()
{
    for (auto &pair : sounds)
        UnloadSound(pair.second);
    sounds.clear();
}

Model *lapCore::ResourceManager::AddModel(const std::string &name, const std::string &filePath, std::vector<std::string> data)
{
    auto model = LoadModel(filePath.c_str());

    models[name] = model;
    return &models[name];
}

void lapCore::ResourceManager::RemoveModel(const std::string &name)
{
    UnloadModel(models[name]);
    models.erase(name);
}

void lapCore::ResourceManager::ClearModels()
{
    for (auto &pair : models)
        UnloadModel(pair.second);
    models.clear();
}

Font *lapCore::ResourceManager::AddFont(const std::string &name, const std::string &filePath, std::vector<std::string> data)
{
    auto font = LoadFont(filePath.c_str());

    fonts[name] = font;
    return &fonts[name];
}

void lapCore::ResourceManager::RemoveFont(const std::string &name)
{
    UnloadFont(fonts[name]);
    fonts.erase(name);
}

void lapCore::ResourceManager::ClearFonts()
{
    for (auto &pair : fonts)
        UnloadFont(pair.second);
    fonts.clear();
}

Image *lapCore::ResourceManager::AddImage(const std::string &name, const std::string &filePath, std::vector<std::string> data)
{
    auto image = LoadImage(filePath.c_str());

    images[name] = image;
    return &images[name];
}

void lapCore::ResourceManager::RemoveImage(const std::string &name)
{
    UnloadImage(images[name]);
    images.erase(name);
}

void lapCore::ResourceManager::ClearImages()
{
    for (auto &pair : images)
        UnloadImage(pair.second);
    images.clear();
}

void lapCore::ResourceManager::ClearAll()
{
    ClearTextures();
    ClearShaders();
    ClearMusic();
    ClearSounds();
    ClearModels();
    ClearFonts();
    ClearImages();
}
