#include "world.hpp"
#include "eutil.hpp"

#include "systems/render_sys.hpp"
#include "systems/gui_sys.hpp"
#include "systems/physics_sys.hpp"
#include "systems/script_sys.hpp"
#include "systems/input_sys.hpp"

#include "reflection.hpp"

using namespace lapCore;

void World::SetScene(ProjectSceneData &scene_data)
{
    if (mainScene)
    {
        for (auto &scnData : project.scenes)
        {
            if (scnData.name != mainScene->name)
                continue;

            for (auto &instData : scnData.instances.objects)
            {
                for (auto &elmntData : instData.elements)
                {
                    if (elmntData->synced != SyncProjectRuntimeMode::UPDATE_ON_SCENE)
                        continue;

                    void* existingElement = mainScene->FindElement(HASH_ID(instData.name.c_str()), elmntData->GetTypeID());
                    if (!existingElement)
                        continue;

                    auto* serializer = Reflection::TryGet(elmntData->GetTypeName());
                    if (!serializer)
                        continue;

                    auto ctx = SerializeContext{&resources};
                    auto existingElementJson = serializer->to_json(existingElement, &ctx);
                    serializer->from_json(elmntData->GetDataPtr(), existingElementJson);
                }
            }
        }
    }

    // Clear out and set back up with new scene data
    if (mainScene) mainScene->Clear();
    mainScene = new Scene(this, scene_data.name);

    mainScene->AddObjectsFromProjectData(scene_data.instances.objects);
    mainScene->AddObjectsFromProjectData(scene_data.instances.prefabs);

    for (auto &system : scene_data.systems)
    {
        if (system.type == "render")
            mainScene->AddSystem<RenderSystem>(system.order);
        else if (system.type == "physics")
        {
            mainScene->AddSystem<PhysicsSystem>(system.order);
            mainScene->GetSystem<PhysicsSystem>()->RegisterBodies();
        }
        else if (system.type == "script")
            mainScene->AddSystem<ScriptSystem>(system.order);
        else if (system.type == "gui")
            mainScene->AddSystem<GUISystem>(system.order);
        else if (system.type == "input")
            mainScene->AddSystem<InputSystem>(system.order);
        else
            dbgln("Unknown system type: " + system.type, LogType::WARNING);
    }

    // Cool thing to ensure there is a render system in the scene. cause if not, the window won't update at all and buffer the whole time
    if (IsWindowReady())
        if (mainScene->GetSystem<RenderSystem>() == nullptr)
            mainScene->AddSystem<RenderSystem>(-1);
}

ProjectSceneData &lapCore::World::GetMainSceneData()
{
    return project.scenes[project.main_scene_index];
}

WindowProperties LoadWindowProperties(const nlohmann::json_abi_v3_12_0::json &windowJson)
{
    WindowProperties window_properties;

    if (windowJson.contains("title"))
        window_properties.title = windowJson["title"];

    if (windowJson.contains("resolution"))
        window_properties.resolution = {windowJson["resolution"].at(0), windowJson["resolution"].at(1)};
    
    if (windowJson.contains("logical-resolution"))
        window_properties.logical_resolution = {windowJson["logical-resolution"].at(0), windowJson["logical-resolution"].at(1)};

    if (windowJson.contains("fullscreen"))
        window_properties.fullscreen = windowJson["fullscreen"];
    
    if (windowJson.contains("borderless"))
        window_properties.borderless = windowJson["borderless"];

    if (windowJson.contains("decorated"))
        window_properties.decorated = windowJson["decorated"];

    if (windowJson.contains("resizable"))
        window_properties.resizable= windowJson["resizable"];

    if (windowJson.contains("vsync"))
        window_properties.vsync = windowJson["vsync"];

    if (windowJson.contains("infinite-fps"))
        window_properties.infinite_fps = windowJson["infinite-fps"];

    if (windowJson.contains("max-fps"))
        window_properties.max_fps = windowJson["max-fps"];

    return window_properties;
}

void lapCore::World::LoadAssets()
{
    for (const auto& asset : project.assets)
    {
        if (asset.type == "texture")
            resources.textures.Load(HASH(asset.name.c_str()), LoadTexture(asset.path.c_str()));
        else if (asset.type == "shader")
            resources.shaders.Load(HASH(asset.name.c_str()), LoadShader(asset.path.c_str(), asset.path.c_str()));
        else if (asset.type == "music")
            resources.music.Load(HASH(asset.name.c_str()), LoadMusicStream(asset.path.c_str()));
        else if (asset.type == "sound")
            resources.sounds.Load(HASH(asset.name.c_str()), LoadSound(asset.path.c_str()));
        else if (asset.type == "model")
            resources.models.Load(HASH(asset.name.c_str()), LoadModel(asset.path.c_str()));
        else if (asset.type == "font")
            resources.fonts.Load(HASH(asset.name.c_str()), LoadFont(asset.path.c_str()));
        else if (asset.type == "image")
            resources.images.Load(HASH(asset.name.c_str()), LoadImage(asset.path.c_str()));
        else
            dbgln("Unknown asset type: " + asset.type + " for asset: " + asset.name, LogType::WARNING);
    }
}

void World::LoadSettings(const std::string &settingsFilePath)
{
    std::string fileStr = ReadFileToString(settingsFilePath);
    if (fileStr == "")
    {
        dbgln("Could not get settings file, loading default for window.", LogType::NOTICE);
        window = WindowProperties();
        return;
    }

    auto settingsJson = nlohmann::json::parse(fileStr);
    window = LoadWindowProperties(settingsJson["window"]);
}

void World::LoadWindow()
{
    InitWindow(window.resolution.x, window.resolution.y, window.title.c_str());
    InitAudioDevice();

    ResetWindowProperties();
    window.target = LoadRenderTexture(window.logical_resolution.x, window.logical_resolution.y);
    LoadAssets();
}

void World::ResetWindowProperties()
{
    if (window.fullscreen)
        SetWindowState(FLAG_FULLSCREEN_MODE);
    if (window.borderless)
        SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
    if (window.resizable)
        SetWindowState(FLAG_WINDOW_RESIZABLE);
    if (!window.decorated)
        SetWindowState(FLAG_WINDOW_UNDECORATED);
    
    if (!window.vsync)
        if (window.infinite_fps)
            SetTargetFPS(-1);
        else
            SetTargetFPS(window.max_fps);
    else
        SetWindowState(FLAG_VSYNC_HINT);
}