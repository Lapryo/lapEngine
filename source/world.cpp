#include "world.hpp"
#include "eutil.hpp"

#include "systems.hpp"

#include "reflection.hpp"

#include <sstream>

// TODO: Add string serialization

using namespace lapCore;

void World::SetScene(ProjectSceneData& scene_data)
{
    id_type targetID = HASH_ID(scene_data.name.c_str());

    // Already in the requested scene.
    if (mainScene && mainScene->id == targetID)
    {
        if (preloadSceneCallback)
            preloadSceneCallback();

        preloadSceneCallback = nullptr;
        return;
    }

    Scene* nextScene = nullptr;
    bool wasStale = false;

    // Find the scene among the preloaded stale scenes.
    auto it = staleScenes.find(targetID);

    if (it != staleScenes.end())
    {
        nextScene = it->second;
        staleScenes.erase(it);

        wasStale = true;
    }
    else
    {
        // Fallback for scenes that weren't preloaded.
        nextScene = new Scene(
            this,
            scene_data,
            Scene::LoadMode::ACTIVE
        );
    }

    // Make the previous active scene stale.
    if (mainScene)
    {
        if (auto* soundSys = mainScene->FindSystem<SoundSystem>())
            soundSys->StopAll();

        mainScene->MakeStale(true);

        staleScenes[mainScene->id] = mainScene;
    }

    // Activate the requested scene.
    mainScene = nextScene;

    if (wasStale)
    {
        // Use saved runtime state if available;
        // otherwise restore the original project data.
        mainScene->Restore(
            true
        );
    }

    // Ensure the active scene can render.
    if (IsWindowReady())
    {
        if (!mainScene->FindSystem<RenderSystem>())
            mainScene->AddSystem<RenderSystem>(-1);
    }

    // Run the callback after restoring the scene.
    if (preloadSceneCallback)
        preloadSceneCallback();

    preloadSceneCallback = nullptr;

    dbgln("Finished setting scene.", LogType::INFO);
}

void World::LoadScenes(id_type mainID)
{
    for (auto& data : project.scenes)
    {
        std::cout << "loading scene " << data.name << '\n';
        id_type sceneID = HASH_ID(data.name.c_str());

        if (sceneID == mainID)
        {
            mainScene = new Scene(
                this,
                data,
                Scene::LoadMode::ACTIVE
            );
        }
        else
        {
            staleScenes[sceneID] = new Scene(
                this,
                data,
                Scene::LoadMode::STALE
            );
        }
    }
}

void lapCore::World::SwitchScene(const ProjectSceneData &scene_data, function<void()> preloadCallback)
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
        string assetPath = string(GetApplicationDirectory()) + asset.path.string();
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

            resources.textures.Load(name, {texture, assetPath});
        }
        else if (asset.type == "shader")
            resources.shaders.Load(HASH(asset.name.c_str()), {LoadShader(string(assetPath + ".vs").c_str(), string(assetPath + ".fs").c_str()), assetPath});
        else if (asset.type == "music")
        {
            auto music = LoadMusicStream(assetPath.c_str());

            for (const auto& data : asset.data)
            {
                if (data.first == "looped")
                    data.second == "true" ? music.looping = true : music.looping = false;
                else if (data.first == "volume")
                    SetMusicVolume(music, stof(data.second));
                else if (data.first == "pan")
                    SetMusicPan(music, stof(data.second));
                else if (data.first == "pitch")
                    SetMusicPitch(music, stof(data.second));
            }

            resources.music.Load(name, {music, assetPath});
        }
        else if (asset.type == "sound")
        {
            auto sound = LoadSound(assetPath.c_str());

            for (const auto& data : asset.data)
            {
                if (data.first == "volume")
                    SetSoundVolume(sound, stof(data.second));
                else if (data.first == "pan")
                    SetSoundPan(sound, stof(data.second));
                else if (data.first == "pitch")
                    SetSoundPitch(sound, stof(data.second));
            }

            resources.sounds.Load(name, {sound, assetPath});
        }
        else if (asset.type == "model")
        {
            auto model = LoadModel(assetPath.c_str());
            resources.models.Load(name, {model, assetPath});
        }
        else if (asset.type == "font")
        {
            auto font = LoadFont(assetPath.c_str());
            if (!IsFontValid(font))
            {
                dbgln(
                    "Failed to load font: " + assetPath,
                    LogType::ERROR
                );

                continue;
            }

            resources.fonts.Load(name, {font, assetPath});
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
                    ImageBlurGaussian(&image, stof(data.second));
                else if (data.first == "background-color")
                {
                    Color c;
                    stringstream ss(data.second);
                    string value;
                    getline(ss, value, ',');
                    c.r = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.g = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.b = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.a = static_cast<unsigned char>(stoi(value));
                    ImageClearBackground(&image, c);
                }
                else if (data.first == "brightness")
                    ImageColorBrightness(&image, stoi(data.second));
                else if (data.first == "contrast")
                    ImageColorContrast(&image, stof(data.second));
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
                    stringstream ss(data.second);
                    string value;
                    getline(ss, value, ',');
                    c.r = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.g = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.b = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.a = static_cast<unsigned char>(stoi(value));
                    ImageColorTint(&image, c);
                }
                else if (data.first == "replace")
                {
                    Color c, c2;
                    stringstream ss(data.second);
                    string value;
                    getline(ss, value, ',');
                    c.r = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.g = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.b = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.a = static_cast<unsigned char>(stoi(value));

                    getline(ss, value, ',');
                    c2.r = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c2.g = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c2.b = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c2.a = static_cast<unsigned char>(stoi(value));
                    ImageColorReplace(&image, c, c2);
                }
                else if (data.first == "crop")
                {
                    Rectangle r;
                    stringstream ss(data.second);
                    string value;
                    getline(ss, value, ',');
                    r.x = stof(value);
                    getline(ss, value, ',');
                    r.y = stof(value);
                    getline(ss, value, ',');
                    r.width = stof(value);
                    getline(ss, value, ',');
                    r.height = stof(value);

                    ImageCrop(&image, r);
                }
                else if (data.first == "dither")
                {
                    int r, g, b, a;
                    stringstream ss(data.second);
                    string value;
                    getline(ss, value, ',');
                    r = stoi(value);
                    getline(ss, value, ',');
                    g = stoi(value);
                    getline(ss, value, ',');
                    b = stoi(value);
                    getline(ss, value, ',');
                    a = stoi(value);

                    ImageDither(&image, r, g, b, a);
                }
                else if (data.first == "alpha-clear")
                {
                    Color c;
                    float t;
                    stringstream ss(data.second);
                    string value;
                    getline(ss, value, ',');
                    c.r = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.g = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.b = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    c.a = static_cast<unsigned char>(stoi(value));
                    getline(ss, value, ',');
                    t = stof(value);

                    ImageAlphaClear(&image, c, t);
                }
                else if (data.first == "alpha-crop")
                    ImageAlphaCrop(&image, stof(data.second));
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
                    stringstream ss(data.second);
                    string value;
                    getline(ss, value, ',');
                    w = stoi(value);
                    getline(ss, value, ',');
                    h = stoi(value);
                    ImageResize(&image, w, h);
                }
                else if (data.first == "resize-nearest-neighbor")
                {
                    int w, h;
                    stringstream ss(data.second);
                    string value;
                    getline(ss, value, ',');
                    w = stoi(value);
                    getline(ss, value, ',');
                    h = stoi(value);
                    ImageResizeNN(&image, w, h);
                }
                else if (data.first == "rotation")
                    ImageRotate(&image, stoi(data.second));
                else if (data.first == "rotate-cw")
                {
                    if (data.second == "true") ImageRotateCW(&image);
                }
                else if (data.first == "rotate-ccw")
                {
                    if (data.second == "true") ImageRotateCCW(&image);
                }
            }

            resources.images.Load(name, {image, assetPath});
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

            resources.maps.Load(name, {map, assetPath});
        }
        else if (asset.type == "tileset")
        {
            Tileset tileset;

            tileset.LoadFromTSJ(assetPath);
            resources.tilesets.Load(name, {tileset, assetPath});
        }
        else
            dbgln("Unknown asset type: " + asset.type + " for asset: " + asset.name, LogType::WARNING);
    }
}

void World::LoadSettings(const string &settingsFilePath)
{
    string fileStr = ReadFileToString(settingsFilePath);
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
