#include "map.hpp"

#include <nlohmann/json.hpp>

using namespace lapCore;

void GetTileset(Map& map, const json& tilesetJson, const std::string &filePath, unsigned int tilesetIndex)
{
    Map::Tileset tileset;
    tileset.firstGID = tilesetJson.value("firstgid", 0);

    // tilesetJson will have a "source" field that points to the tileset
    std::string sourceFile = tilesetJson.value("source", "");
    if (!sourceFile.empty())
    {
        std::string sourceFilePath = filePath + "/tilesets/" + sourceFile;
        json tilesetData = ReadFileToJsonObject(sourceFilePath);

        tileset.textureID = HASH_ID(tilesetData.value("name", "").c_str());
        tileset.columns = tilesetData.value("columns", 0);
        tileset.count = tilesetData.value("tilecount", 0);

        if (tilesetData.contains("tiles"))
        {
            for (const auto& tileJson : tilesetData.at("tiles"))
            {
                unsigned int id = tileJson.value("id", 0);
                std::string type = tileJson.value("type", "");
                tileset.types[id] = type;
            }
        }
    }

    map.tilesets[tilesetIndex] = tileset;
}

void GetTilesets(Map& map, const json& mapJson, const std::string& filePath)
{
    if (mapJson.contains("tilesets"))
    {
        int tilesetIndex = 0;
        for (const auto& tilesetJson : mapJson.at("tilesets"))
        {
            GetTileset(map, tilesetJson, filePath, tilesetIndex);
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

void lapCore::Map::LoadFromTMJ(const std::string &filePath)
{
    json mapJson = ReadFileToJsonObject(filePath);

    width = mapJson.value("width", 0);
    height = mapJson.value("height", 0);
    tileWidth = mapJson.value("tilewidth", 0);
    tileHeight = mapJson.value("tileheight", 0);

    GetTilesets(*this, mapJson, filePath);
    GetLayers(*this, mapJson);
}

void lapCore::Map::Generate(Scene *scene)
{
    for (const auto& layer : layers)
    {
        for (const auto& tileData : layer.second)
        {
            std::string tileName = "tile_" + 
                std::to_string(layer.first) + "_" + 
                std::to_string(tileData.id) + "_" + 
                std::to_string(tileData.x) + "_" + 
                std::to_string(tileData.y);

            auto tile = scene->AddObject(HASH(tileName.c_str()), HASH(""), -1);
            objects[layer.first] = tile.id;

            // Add tile here properties as elements



        }
    }
}

void lapCore::Map::Unload()
{

}
