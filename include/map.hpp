#pragma once

#include <map>
#include <entt/entt.hpp>
#include "scene.hpp"

namespace lapCore
{
    struct Map {
        struct Tile {
            unsigned int tileset, id, x, y;
            std::map<std::string, std::string> properties;
        };

        struct Tileset {
            entt::id_type textureID;
            unsigned int firstGID, columns, count;
            std::map<unsigned int, std::string> types;
        };

        unsigned int width, height, tileWidth, tileHeight;
        
        using TileLayer = std::vector<Tile>;
        std::map<unsigned int, TileLayer> layers;

        //         LAYER ID      TEXTURE ID
        std::map<unsigned int, Tileset> tilesets;

        //.        LAYER ID      OBJECT ID
        std::map<unsigned int, entt::id_type> objects;

        void LoadFromTMJ(const std::string& filePath);
        void Generate(Scene* scene);

        void Unload();
    };
}