#pragma once

#include <raylib.h>
#include <entt/entt.hpp>

#include <unordered_map>
#include <string>
#include <functional>
#include <cassert>
#include <memory>

#include "map.hpp"

namespace lapCore
{
    using namespace std;
    using namespace entt;

    class IAssetStorage
    {
    public:
        virtual ~IAssetStorage() = default;
        virtual void UnloadAll() = 0;
    };

    template <typename T>
    struct AssetData
    {
        AssetData();
        AssetData(T a, string p) : asset(a), path(p) {}

        T asset;
        string path;
    };

    template <typename T>
    class AssetStorage : public IAssetStorage
    {
    public:
        using Deleter = function<void(T&)>;

        AssetStorage(Deleter deleter = nullptr)
            : deleter(deleter)
        {
        }

        ~AssetStorage()
        {
            UnloadAll();
        }

        id_type Load(hashed_string name, AssetData<T> asset)
        {
            id_type id = name.value();

            auto [it, inserted] = assets.emplace(id, asset);

            if (!inserted)
            {
                if (deleter)
                    deleter(asset.asset);

                return id;
            }

            reverseLookup[id] = name.data();

            return id;
        }

        string GetName(id_type id) const
        {
            auto it = reverseLookup.find(id);
            if (it != reverseLookup.end())
                return it->second;
            return "";
        }

        void Unload(id_type id)
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

        void UnloadAll() override
        {
            for (auto& [id, asset] : assets)
            {
                if (deleter)
                    deleter(asset.asset);
            }

            assets.clear();
            reverseLookup.clear();
        }

        AssetData<T>* TryGet(id_type id)
        {
            auto it = assets.find(id);
            if (it == assets.end())
                return nullptr;

            return &it->second;
        }

        const AssetData<T>* TryGet(id_type id) const
        {
            auto it = assets.find(id);
            if (it == assets.end())
                return nullptr;

            return &it->second;
        }

        AssetData<T>& Get(id_type id)
        {
            AssetData<T>* asset = TryGet(id);
            assert(asset && "Asset not found");
            return *asset;
        }

        const AssetData<T>& Get(id_type id) const
        {
            const AssetData<T>* asset = TryGet(id);
            assert(asset && "Asset not found");
            return *asset;
        }

        /**
         * @brief Attempts to find the data of a given id and returns its asset value of type T
         * @return The asset value of AssetData<T>
         */
        T* GetAsset(id_type id)
        {
            auto assetData = TryGet(id);
            if (!assetData) return nullptr;

            return &assetData->asset;
        }

        /**
        * @brief Attempts to find the data of a given id and returns its path value
        * @param id The hash ID of the asset data's name, id_type
        * @return The path string of AssetData<T>
        */
        string GetPath(id_type id)
        {
            auto assetData = TryGet(id);
            if (!assetData) return "";

            return assetData->path;
        }

        bool Has(id_type id) const
        {
            return assets.find(id) != assets.end();
        }

        unordered_map<id_type, AssetData<T>>& GetAssets()
        {
            return assets;
        }

        const unordered_map<id_type, AssetData<T>>& GetAssets() const
        {
            return assets;
        }

    private:
        unordered_map<id_type, AssetData<T>> assets;
        unordered_map<id_type, string> reverseLookup;

        Deleter deleter;
    };

    struct ResourceManager
    {
        ResourceManager() = default;

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

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

        AssetStorage<Tileset> tilesets{
            [](Tileset& tileset)
            {
                tileset.Unload();
            }
        };

        unordered_map<id_type, unique_ptr<IAssetStorage>> others;

        template <typename T>
        AssetStorage<T>* AddStorageToOthers(
            id_type id,
            function<void(T&)> deleter)
        {
            if (others.contains(id))
                return nullptr;

            auto storage = make_unique<AssetStorage<T>>(deleter);

            auto* storagePointer = storage.get();

            others.emplace(id, move(storage));

            return storagePointer;
        }

        template <typename T>
        AssetStorage<T>* GetStorageFromOthers(id_type id)
        {
            auto it = others.find(id);
            if (it == others.end())
                return nullptr;

            auto* storagePtr = dynamic_cast<AssetStorage<T>*>(it->second.get());

            return storagePtr;
        }

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
            tilesets.UnloadAll();

            for (auto& [id, storage] : others)
            {
                storage->UnloadAll();
            }

            others.clear();
        }
    };
}