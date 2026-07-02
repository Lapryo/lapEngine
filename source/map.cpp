#include "map.hpp"

#include "eutil.hpp"

#include <nlohmann/json.hpp>

using namespace lapCore;

void GetTileset(Map& map, const json& tilesetJson, const std::string &filePath, unsigned int tilesetIndex, ResourceManager* resources)
{
    Map::TilesetInfo tileset;

    // tilesetJson will have a "source" field that points to the tileset
    std::string sourceFile = tilesetJson.value("source", "");
    if (!sourceFile.empty())
    {
        tileset.id = HASH_ID(sourceFile.c_str());
        tileset.firstGID = tilesetJson.value("firstgid", 0);

        auto tilesetPtr = resources->tilesets.TryGet(tileset.id);
        if (tilesetPtr)
        {
            tileset.count = tilesetPtr->count;
            tileset.types = tilesetPtr->types;
        }
    }

    map.tilesets[tilesetIndex] = tileset;
}

void GetTilesets(Map& map, const json& mapJson, const std::string& filePath, ResourceManager* resources)
{
    if (mapJson.contains("tilesets"))
    {
        int tilesetIndex = 0;
        for (const auto& tilesetJson : mapJson.at("tilesets"))
        {
            GetTileset(map, tilesetJson, filePath, tilesetIndex, resources);
            tilesetIndex++;
        }
    }
}

constexpr uint32_t FLIP_H = 0x80000000;
constexpr uint32_t FLIP_V = 0x40000000;
constexpr uint32_t FLIP_D = 0x20000000;
constexpr uint32_t TILE_MASK = 0x1FFFFFFF;

void GetTiles(Map& map, const json& layerDataJson, unsigned int layerID)
{
    Map::TileLayer tileLayer;
    for (unsigned int i = 0; i < layerDataJson.size(); i++)
    {
        uint32_t rawGID = layerDataJson.at(i).get<uint32_t>();
        
        bool flipX = rawGID & FLIP_H;
        bool flipY = rawGID & FLIP_V;
        bool flipD = rawGID & FLIP_D;
        uint32_t tileID = rawGID & TILE_MASK;

        if (tileID == 0)
            continue; // Skip empty tiles

        Map::Tile tile;
        tile.id = tileID;

        unsigned int mapWidth = static_cast<unsigned int>(map.size.x);
        tile.rect.x = (i % mapWidth) * map.tileSize.x;
        tile.rect.y = (i / mapWidth) * map.tileSize.y;

        tile.rect.width = map.tileSize.x;
        tile.rect.height = map.tileSize.y;

        tile.flipX = flipX;
        tile.flipY = flipY;
        tile.flipD = flipD;

        // Determine which tileset this tile belongs to
        for (const auto& [tilesetIndex, tileset] : map.tilesets)
        {
            if (tileID >= tileset.firstGID && tileID < tileset.firstGID + tileset.count)
            {
                tile.tileset = tilesetIndex;
                auto it = tileset.types.find(tileID);
                if (it != tileset.types.end())
                    tile.properties["type"] = it->second;

                break;
            }
        }

        tileLayer.push_back(tile);
    }

    map.layers[layerID] = tileLayer;
}

void GetObjects(Map& map, const json& objectsJson, unsigned int layerID)
{
    Map::TileLayer tileLayer;

    for (const auto& objectJson : objectsJson)
    {
        Map::Tile tile;

        tile.id = objectJson.value("gid", 0);
        tile.rect.x = objectJson.value("x", 0);
        tile.rect.y = objectJson.value("y", 0);
        tile.rect.width = objectJson.value("width", 0);
        tile.rect.height = objectJson.value("height", 0);

        tile.rect.y -= tile.rect.height;
        tile.opacity = objectJson.value("opacity", 1.f);
        tile.isObject = true;

        tile.properties["name"] = objectJson.value("name", "");
        tile.properties["type"] = objectJson.value("type", "");

        if (objectJson.contains("properties"))
        {
            for (const auto& propJson : objectJson.at("properties"))
            {
                std::string propName = propJson.value("name", "");
                if (propName == "") break;
                
                std::string propVal = propJson.value("value", "");
                tile.properties[propName] = propVal;
            }
        }

        for (const auto& [tilesetIndex, tileset] : map.tilesets)
        {
            if (tile.id >= tileset.firstGID && tile.id < tileset.firstGID + tileset.count)
            {
                tile.tileset = tilesetIndex;

                auto it = tileset.types.find(tile.id - tileset.firstGID);
                if (it != tileset.types.end())
                    tile.properties["type"] = it->second;
                break;
            }
        }

        tileLayer.push_back(tile);
    }

    map.layers[layerID] = tileLayer;
}

void GetLayer(Map& map, const json& layerJson, unsigned int index)
{
    std::string layerType = layerJson.value("type", "");
    
    if (layerType == "tilelayer")
    {
        const auto& data = layerJson.at("data");
        GetTiles(map, data, index);
    }
    else if (layerType == "objectgroup")
    {
        const auto& data = layerJson.at("objects");
        GetObjects(map, data, index);
    }
    else
        return;

    
    if (layerJson.value("type", "") != "tilelayer")
        return;
}

void GetLayers(Map& map, const json& mapJson)
{
    if (mapJson.contains("layers"))
    {
        for (unsigned int i = 0; i < mapJson.at("layers").size(); i++)
            GetLayer(map, mapJson.at("layers").at(i), i);
    }
}

void lapCore::Map::LoadFromTMJ(ResourceManager* resources, const std::string &filePath)
{
    json mapJson = ReadFileToJsonObject(filePath, false);

    size.x = mapJson.value("width", 0);
    size.y = mapJson.value("height", 0);
    tileSize.x = mapJson.value("tilewidth", 0);
    tileSize.y = mapJson.value("tileheight", 0);

    GetTilesets(*this, mapJson, filePath, resources);
    GetLayers(*this, mapJson);
}

void lapCore::Map::Unload()
{
    layers.clear();
    tilesets.clear();
    objects.clear();
}

void lapCore::Tileset::LoadFromTSJ(const std::string &filePath)
{
    json tilesetJson = ReadFileToJsonObject(filePath, false);

    textureID = HASH_ID(tilesetJson.value("image", "").c_str());

    columns = tilesetJson.value("columns", 0);
    count = tilesetJson.value("tilecount", 0);

    if (tilesetJson.contains("tiles"))
    {
        for (const auto& tileJson : tilesetJson.at("tiles"))
        {
            unsigned int tileID = tileJson.value("id", 0);
            std::string type = tileJson.value("type", "");
            types[tileID] = type;
        }
    }
}

void lapCore::Tileset::Unload()
{
    types.clear();
}
