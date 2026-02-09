#include "world.hpp"
#include "eutil.hpp"

void lapCore::World::LoadSettings(const std::string &settingsFilePath)
{
    // Load the JSON settings file
    auto settings = lapCore::ReadFileToJsonObject(settingsFilePath);
    
    // Load logical resolution if present
    if (settings.contains("logicalResolution"))
    {
        auto res = settings["logicalResolution"];
        if (res.contains("x") && res.contains("y"))
        {
            logicalResolution = {
                res["x"].get<float>(),
                res["y"].get<float>()
            };
        }
    }
}

void lapCore::World::ResetSceneToDefault(ProjectSceneData &scene_data)
{
    // TODO: Implement scene reset to default
}
