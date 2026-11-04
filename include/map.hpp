#pragma once

#include <map>
#include <entt/entt.hpp>
#include <raylib.h>

using namespace std;

namespace lapCore
{
    struct Scene;
    struct ResourceManager;
    using namespace entt;

    using Object = entity;

    struct Tileset {
        id_type textureID = 0;
        unsigned int columns = 0, count = 0;
        map<unsigned int, string> types = {};

        void LoadFromTSJ(const string& filePath);
        void Unload();
    };

    struct Map {
        using TileProperties = map<string, string>;

        struct Tile {
            unsigned int tileset = 0, id = 0;
            Rectangle rect = {0, 0, 0, 0};
            bool flipX = false, flipY = false, flipD = false, isObject = false;
            TileProperties properties = {};
            float opacity = 1.f;
        };

        struct TilesetInfo {
            unsigned int firstGID = 0, count = 0;
            id_type id = 0;
            map<unsigned int, string> types;
        };

        Vector2 size = {0, 0}, tileSize = {0, 0};
        
        using TileLayer = pair<string, vector<Tile>>;
        map<unsigned int, TileLayer> layers;

        //         LAYER ID     TILESET INFO
        map<unsigned int, TilesetInfo> tilesets;

        //.        LAYER ID      OBJECT ID
        map<unsigned int, id_type> objects;

        void LoadFromTMJ(ResourceManager* resources, const string& filePath);
        void Unload();
    };
}