#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <raylib.h>

#include <unordered_map>
#include <string>
#include <vector>

namespace lapCore
{
    struct ResourceManager
    {
        std::unordered_map<std::string, Texture2D> textures;
        std::unordered_map<std::string, Shader> shaders;
        std::unordered_map<std::string, Music> music;
        std::unordered_map<std::string, Sound> sounds;
        std::unordered_map<std::string, Model> models;
        std::unordered_map<std::string, Font> fonts;
        std::unordered_map<std::string, Image> images;

        Texture2D *AddTexture(const std::string &name, const std::string &filePath, std::vector<std::string> data = {});
        void RemoveTexture(const std::string &name);
        void ClearTextures();

        Shader *AddShader(const std::string &name, const std::string &vertexShaderPath, const std::string &fragmentShaderPath, std::vector<std::string> data = {});
        void RemoveShader(const std::string &name);
        void ClearShaders();

        Music *AddMusic(const std::string &name, const std::string &filePath, std::vector<std::string> data = {});
        void RemoveMusic(const std::string &name);
        void ClearMusic();

        Sound *AddSound(const std::string &name, const std::string &filePath, std::vector<std::string> data = {});
        void RemoveSound(const std::string &name);
        void ClearSounds();

        Model *AddModel(const std::string &name, const std::string &filePath, std::vector<std::string> data = {});
        void RemoveModel(const std::string &name);
        void ClearModels();

        Font *AddFont(const std::string &name, const std::string &filePath, std::vector<std::string> data = {});
        void RemoveFont(const std::string &name);
        void ClearFonts();

        Image *AddImage(const std::string &name, const std::string &filePath, std::vector<std::string> data = {});
        void RemoveImage(const std::string &name);
        void ClearImages();

        void ClearAll();
    };
}

#endif