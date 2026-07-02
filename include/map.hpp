#pragma once

#include <map>
#include <entt/entt.hpp>
#include <raylib.h>

namespace lapCore
{
    struct Scene;
    struct ResourceManager;

    using Object = entt::entity;

    struct Tileset {
        entt::id_type textureID = 0;
        unsigned int columns = 0, count = 0;
        std::map<unsigned int, std::string> types = {};

        void LoadFromTSJ(const std::string& filePath);
        void Unload();
    };

    struct Map {
        using TileProperties = std::map<std::string, std::string>;

        struct Tile {
            unsigned int tileset = 0, id = 0;
            Rectangle rect = {0, 0, 0, 0};
            bool flipX = false, flipY = false, flipD = false, isObject = false;
            TileProperties properties = {};
            float opacity = 1.f;
        };

        struct TilesetInfo {
            unsigned int firstGID = 0, count = 0;
            entt::id_type id = 0;
            std::map<unsigned int, std::string> types;
        };

        Vector2 size = {0, 0}, tileSize = {0, 0};
        
        using TileLayer = std::vector<Tile>;
        std::map<unsigned int, TileLayer> layers;

        //         LAYER ID     TILESET INFO
        std::map<unsigned int, TilesetInfo> tilesets;

        //.        LAYER ID      OBJECT ID
        std::map<unsigned int, entt::id_type> objects;

        void LoadFromTMJ(ResourceManager* resources, const std::string& filePath);
        void Unload();
    };
}