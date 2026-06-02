#pragma once

#include <map>
#include <entt/entt.hpp>
#include "scene.hpp"

namespace lapCore
{
    struct Tileset {
        entt::id_type textureID;
        unsigned int columns, count;
        std::map<unsigned int, std::string> types;

        void LoadFromTSJ(const std::string& filePath);
        void Unload();
    };

    struct Map {
        struct Tile {
            unsigned int tileset, id, x, y;
            std::map<std::string, std::string> properties;
        };

        struct TilesetInfo {
            unsigned int firstGID, count;
            entt::id_type id;
        };

        unsigned int width, height, tileWidth, tileHeight;
        
        using TileLayer = std::vector<Tile>;
        std::map<unsigned int, TileLayer> layers;

        //         LAYER ID     TILESET INFO
        std::map<unsigned int, TilesetInfo> tilesets;

        //.        LAYER ID      OBJECT ID
        std::map<unsigned int, entt::id_type> objects;

        void LoadFromTMJ(ResourceManager* resources, const std::string& filePath);
        void Generate(Scene* scene);

        void Unload();
    };
}