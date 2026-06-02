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

void GetTiles(Map& map, const json& layerDataJson, unsigned int layerID)
{
    Map::TileLayer tileLayer;
    for (unsigned int i = 0; i < layerDataJson.size(); i++)
    {
        unsigned int tileID = layerDataJson.at(i).get<unsigned int>();
        if (tileID == 0)
            continue; // Skip empty tiles

        Map::Tile tile;
        tile.id = tileID;
        tile.x = (i % map.width) * map.tileWidth;
        tile.y = (i / map.width) * map.tileHeight;

        // Determine which tileset this tile belongs to
        for (const auto& [tilesetIndex, tileset] : map.tilesets)
        {
            if (tileID >= tileset.firstGID && tileID < tileset.firstGID + tileset.count)
            {
                tile.tileset = tilesetIndex;
                break;
            }
        }

        tileLayer.push_back(tile);
    }

    map.layers[layerID] = tileLayer;
}

void GetLayer(Map& map, const json& layerJson)
{
    if (layerJson.value("type", "") != "tilelayer")
        return;

    unsigned int layerID = layerJson.value("id", 0);
    const auto& data = layerJson.at("data");
    GetTiles(map, data, layerID);
}

void GetLayers(Map& map, const json& mapJson)
{
    if (mapJson.contains("layers"))
    {
        for (const auto& layerJson : mapJson.at("layers"))
        {
            GetLayer(map, layerJson);
        }
    }
}

void lapCore::Map::LoadFromTMJ(ResourceManager* resources, const std::string &filePath)
{
    json mapJson = ReadFileToJsonObject(filePath);

    width = mapJson.value("width", 0);
    height = mapJson.value("height", 0);
    tileWidth = mapJson.value("tilewidth", 0);
    tileHeight = mapJson.value("tileheight", 0);

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
    json tilesetJson = ReadFileToJsonObject(filePath);

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
