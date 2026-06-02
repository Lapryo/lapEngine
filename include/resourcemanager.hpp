#pragma once

#include <raylib.h>
#include <entt/entt.hpp>

#include <unordered_map>
#include <string>
#include <functional>
#include <cassert>

#include "map.hpp"

namespace lapCore
{
    template <typename T>
    class AssetStorage
    {
    public:
        using Deleter = std::function<void(T&)>;

        AssetStorage(Deleter deleter = nullptr)
            : deleter(deleter)
        {
        }

        ~AssetStorage()
        {
            UnloadAll();
        }

        entt::id_type Load(entt::hashed_string name, T asset)
        {
            entt::id_type id = name.value();
            assets.emplace(id, std::move(asset));
            reverseLookup[id] = name.data();

            return id;
        }

        std::string GetName(entt::id_type id) const
        {
            auto it = reverseLookup.find(id);
            if (it != reverseLookup.end())
                return it->second;
            return "";
        }

        void Unload(entt::id_type id)
        {
            auto assetIt = assets.find(id);
            if (assetIt == assets.end())
                return;

            if (deleter)
                deleter(assetIt->second);

            auto reverseIt = reverseLookup.find(id);
            if (reverseIt != reverseLookup.end())
                reverseLookup.erase(reverseIt);

            assets.erase(assetIt);
        }

        void UnloadAll()
        {
            for (auto& [id, asset] : assets)
            {
                if (deleter)
                    deleter(asset);
            }

            assets.clear();
            reverseLookup.clear();
        }

        T* TryGet(entt::id_type id)
        {
            auto it = assets.find(id);
            if (it == assets.end())
                return nullptr;

            return &it->second;
        }

        const T* TryGet(entt::id_type id) const
        {
            auto it = assets.find(id);
            if (it == assets.end())
                return nullptr;

            return &it->second;
        }

        T& Get(entt::id_type id)
        {
            T* asset = TryGet(id);
            assert(asset && "Asset not found");
            return *asset;
        }

        const T& Get(entt::id_type id) const
        {
            const T* asset = TryGet(id);
            assert(asset && "Asset not found");
            return *asset;
        }

        bool Has(entt::id_type id) const
        {
            return assets.find(id) != assets.end();
        }

        std::unordered_map<entt::id_type, T>& GetAssets()
        {
            return assets;
        }

        const std::unordered_map<entt::id_type, T>& GetAssets() const
        {
            return assets;
        }

    private:
        std::unordered_map<entt::id_type, T> assets;
        std::unordered_map<entt::id_type, std::string> reverseLookup;

        Deleter deleter;
    };

    struct ResourceManager
    {
        AssetStorage<Texture2D> textures{
            [](Texture2D& texture)
            {
                UnloadTexture(texture);
            }
        };

        AssetStorage<Shader> shaders{
            [](Shader& shader)
            {
                UnloadShader(shader);
            }
        };

        AssetStorage<Music> music{
            [](Music& music)
            {
                UnloadMusicStream(music);
            }
        };

        AssetStorage<Sound> sounds{
            [](Sound& sound)
            {
                UnloadSound(sound);
            }
        };

        AssetStorage<Model> models{
            [](Model& model)
            {
                UnloadModel(model);
            }
        };

        AssetStorage<Font> fonts{
            [](Font& font)
            {
                UnloadFont(font);
            }
        };

        AssetStorage<Image> images{
            [](Image& image)
            {
                UnloadImage(image);
            }
        };

        AssetStorage<Map> maps{
            [](Map& map)
            {
                map.Unload();
            }
        };

        void ClearAll()
        {
            textures.UnloadAll();
            shaders.UnloadAll();
            music.UnloadAll();
            sounds.UnloadAll();
            models.UnloadAll();
            fonts.UnloadAll();
            images.UnloadAll();
            maps.UnloadAll();
        }
    };
}