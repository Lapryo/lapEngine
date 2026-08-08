#include "world.hpp"
#include "eutil.hpp"

#include "systems/render_sys.hpp"
#include "systems/gui_sys.hpp"
#include "systems/physics_sys.hpp"
#include "systems/script_sys.hpp"
#include "systems/input_sys.hpp"

#include "reflection.hpp"

#include <sstream>

// TODO: Add string serialization

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

    if (preloadSceneCallback)
    {
        preloadSceneCallback();
    }
    preloadSceneCallback = nullptr;
}

ProjectSceneData &lapCore::World::GetMainSceneData()
{
    return project.scenes[project.main_scene_index];
}

void lapCore::World::SwitchScene(const ProjectSceneData &scene_data, std::function<void()> preloadCallback)
{
    switchingScene = true;
    nextSceneData = scene_data;
    preloadSceneCallback = preloadCallback;
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
        std::string assetPath = std::string(GetApplicationDirectory()) + asset.path;
        auto name = HASH(asset.name.c_str());

        if (asset.type == "texture")
        {
            auto texture = LoadTexture(assetPath.c_str());

            for (const auto& data : asset.data)
            {
                if (data.first == "gen-mipmaps")
                {
                    if (data.second == "true") GenTextureMipmaps(&texture);
                }
                else if (data.first == "filter")
                {
                    if (data.second == "point")
                        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
                    else if (data.second == "bilinear")
                        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
                    else if (data.second == "trilinear")
                        SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
                    else if (data.second == "anisotropic-4x")
                        SetTextureFilter(texture, TEXTURE_FILTER_ANISOTROPIC_4X);
                    else if (data.second == "anisotropic-8x")
                        SetTextureFilter(texture, TEXTURE_FILTER_ANISOTROPIC_8X);
                    else if (data.second == "anisotropic-16x")
                        SetTextureFilter(texture, TEXTURE_FILTER_ANISOTROPIC_16X);
                }
                else if (data.first == "wrap")
                {
                    if (data.second == "repeat")
                        SetTextureWrap(texture, TEXTURE_WRAP_REPEAT);
                    else if (data.second == "clamp")
                        SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);
                    else if (data.second == "mirror-repeat")
                        SetTextureWrap(texture, TEXTURE_WRAP_MIRROR_REPEAT);
                    else if (data.second == "mirror-clamp")
                        SetTextureWrap(texture, TEXTURE_WRAP_MIRROR_CLAMP);
                }
            }

            resources.textures.Load(name, texture);
        }
        else if (asset.type == "shader")
            resources.shaders.Load(HASH(asset.name.c_str()), LoadShader(std::string(assetPath + ".vs").c_str(), std::string(assetPath + ".fs").c_str()));
        else if (asset.type == "music")
        {
            auto music = LoadMusicStream(assetPath.c_str());

            for (const auto& data : asset.data)
            {
                if (data.first == "looped")
                    data.second == "true" ? music.looping = true : music.looping = false;
                else if (data.first == "volume")
                    SetMusicVolume(music, std::stof(data.second));
                else if (data.first == "pan")
                    SetMusicPan(music, std::stof(data.second));
                else if (data.first == "pitch")
                    SetMusicPitch(music, std::stof(data.second));
            }

            resources.music.Load(name, music);
        }
        else if (asset.type == "sound")
        {
            auto sound = LoadSound(assetPath.c_str());

            for (const auto& data : asset.data)
            {
                if (data.first == "volume")
                    SetSoundVolume(sound, std::stof(data.second));
                else if (data.first == "pan")
                    SetSoundPan(sound, std::stof(data.second));
                else if (data.first == "pitch")
                    SetSoundPitch(sound, std::stof(data.second));
            }

            resources.sounds.Load(name, sound);
        }
        else if (asset.type == "model")
        {
            auto model = LoadModel(assetPath.c_str());
            resources.models.Load(name, model);
        }
        else if (asset.type == "font")
        {
            auto font = LoadFont(assetPath.c_str());
            resources.fonts.Load(name, font);
        }
        else if (asset.type == "image")
        {
            auto image = LoadImage(assetPath.c_str());
            
            for (const auto& data : asset.data)
            {
                if (data.first == "premultiply")
                {
                    if (data.second == "true") ImageAlphaPremultiply(&image); 
                }
                else if (data.first == "blur")
                    ImageBlurGaussian(&image, std::stof(data.second));
                else if (data.first == "background-color")
                {
                    Color c;
                    std::stringstream ss(data.second);
                    std::string value;
                    std::getline(ss, value, ',');
                    c.r = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.g = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.b = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.a = static_cast<unsigned char>(std::stoi(value));
                    ImageClearBackground(&image, c);
                }
                else if (data.first == "brightness")
                    ImageColorBrightness(&image, std::stoi(data.second));
                else if (data.first == "contrast")
                    ImageColorContrast(&image, std::stof(data.second));
                else if (data.first == "grayscale")
                {
                    if (data.second == "true") ImageColorGrayscale(&image);
                }
                else if (data.first == "invert")
                {
                    if (data.second == "true") ImageColorInvert(&image);
                }
                else if (data.first == "tint")
                {
                    Color c;
                    std::stringstream ss(data.second);
                    std::string value;
                    std::getline(ss, value, ',');
                    c.r = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.g = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.b = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.a = static_cast<unsigned char>(std::stoi(value));
                    ImageColorTint(&image, c);
                }
                else if (data.first == "replace")
                {
                    Color c, c2;
                    std::stringstream ss(data.second);
                    std::string value;
                    std::getline(ss, value, ',');
                    c.r = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.g = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.b = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.a = static_cast<unsigned char>(std::stoi(value));

                    std::getline(ss, value, ',');
                    c2.r = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c2.g = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c2.b = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c2.a = static_cast<unsigned char>(std::stoi(value));
                    ImageColorReplace(&image, c, c2);
                }
                else if (data.first == "crop")
                {
                    Rectangle r;
                    std::stringstream ss(data.second);
                    std::string value;
                    std::getline(ss, value, ',');
                    r.x = std::stof(value);
                    std::getline(ss, value, ',');
                    r.y = std::stof(value);
                    std::getline(ss, value, ',');
                    r.width = std::stof(value);
                    std::getline(ss, value, ',');
                    r.height = std::stof(value);

                    ImageCrop(&image, r);
                }
                else if (data.first == "dither")
                {
                    int r, g, b, a;
                    std::stringstream ss(data.second);
                    std::string value;
                    std::getline(ss, value, ',');
                    r = std::stoi(value);
                    std::getline(ss, value, ',');
                    g = std::stoi(value);
                    std::getline(ss, value, ',');
                    b = std::stoi(value);
                    std::getline(ss, value, ',');
                    a = std::stoi(value);

                    ImageDither(&image, r, g, b, a);
                }
                else if (data.first == "alpha-clear")
                {
                    Color c;
                    float t;
                    std::stringstream ss(data.second);
                    std::string value;
                    std::getline(ss, value, ',');
                    c.r = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.g = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.b = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    c.a = static_cast<unsigned char>(std::stoi(value));
                    std::getline(ss, value, ',');
                    t = std::stof(value);

                    ImageAlphaClear(&image, c, t);
                }
                else if (data.first == "alpha-crop")
                    ImageAlphaCrop(&image, std::stof(data.second));
                else if (data.first == "flip-horizontal")
                {
                    if (data.second == "true") ImageFlipHorizontal(&image);
                }
                else if (data.first == "flip-vertical")
                {
                    if (data.second == "true") ImageFlipVertical(&image);
                }
                else if (data.first == "compute-mipmaps")
                {
                    if (data.second == "true") ImageMipmaps(&image);
                }
                else if (data.first == "resize")
                {
                    int w, h;
                    std::stringstream ss(data.second);
                    std::string value;
                    std::getline(ss, value, ',');
                    w = std::stoi(value);
                    std::getline(ss, value, ',');
                    h = std::stoi(value);
                    ImageResize(&image, w, h);
                }
                else if (data.first == "resize-nearest-neighbor")
                {
                    int w, h;
                    std::stringstream ss(data.second);
                    std::string value;
                    std::getline(ss, value, ',');
                    w = std::stoi(value);
                    std::getline(ss, value, ',');
                    h = std::stoi(value);
                    ImageResizeNN(&image, w, h);
                }
                else if (data.first == "rotation")
                    ImageRotate(&image, std::stoi(data.second));
                else if (data.first == "rotate-cw")
                {
                    if (data.second == "true") ImageRotateCW(&image);
                }
                else if (data.first == "rotate-ccw")
                {
                    if (data.second == "true") ImageRotateCCW(&image);
                }
            }

            resources.images.Load(name, image);
        }
        else if (asset.type == "map")
        {
            Map map;

            // Handle map loading here
            for (const auto& data : asset.data)
            {
                if (data.first == "map-type")
                {
                    if (data.second == "tiled")
                    {
                        map.LoadFromTMJ(&resources, assetPath);
                    }
                    else if (data.second == "lapmap")
                    {
                        dbgln("Loading lapmap format is not yet implemented! Map is empty.", LogType::WARNING);
                    }
                }
            }

            resources.maps.Load(name, map);
        }
        else if (asset.type == "tileset")
        {
            Tileset tileset;

            tileset.LoadFromTSJ(assetPath);
            resources.tilesets.Load(name, tileset);
        }
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
