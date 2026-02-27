#include "world.hpp"
#include "eutil.hpp"

#include "systems/render_sys.hpp"

using namespace lapCore;

void World::SetScene(ProjectSceneData &scene_data)
{
    main_scene.Clear();

    main_scene = Scene(this, scene_data.name);
    for (auto &instance : scene_data.instances)
    {
        Object object = main_scene.AddObject(instance.name, instance.parent, instance.child_index);

        for (auto &element : instance.elements)
        {
            // AI-generated code
            std::visit([&](auto&& data)
            {
                using T = std::decay_t<decltype(data)>;
                if constexpr (!std::is_same_v<T, std::monostate>)
                    main_scene.AddElement<T>(main_scene.objects, object, data);
            }, element.data);
        }
    }

    for (auto &system : scene_data.systems)
    {
        std::cout << system.type << ' ' << system.order << '\n';

        if (system.type == "render")
            main_scene.AddSystem<RenderSystem>(system.order);
    }

    // cool thing to ensure there is a render system in the scene cause if not, the window wouldn't update at all and buffer the whole time
    if (rl::IsWindowReady())
    {
        std::cout << "window is ready\n";
        if (main_scene.GetSystem<RenderSystem>() == nullptr)
        {
            std::cout << "render system does not exist in the scene, adding it now\n";
            main_scene.AddSystem<RenderSystem>(-1);
        }
    }
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

void World::LoadSettings(const std::string &settingsFilePath)
{
    std::string fileStr = ReadFileToString(settingsFilePath);
    if (fileStr == "")
    {
        std::cout << "Could not get settings file, loading default for window.\n";
        window = WindowProperties();
        return;
    }

    auto settingsJson = nlohmann::json::parse(fileStr);
    window = LoadWindowProperties(settingsJson["window"]);
}

void World::LoadWindow()
{
    rl::InitWindow(window.resolution.x, window.resolution.y, window.title.c_str());
    ResetWindowProperties();
    window.target = rl::LoadRenderTexture(window.logical_resolution.x, window.logical_resolution.y);
}

void World::ResetWindowProperties()
{
    if (window.fullscreen)
        rl::SetWindowState(rl::FLAG_FULLSCREEN_MODE);
    if (window.borderless)
        rl::SetWindowState(rl::FLAG_BORDERLESS_WINDOWED_MODE);
    if (window.resizable)
        rl::SetWindowState(rl::FLAG_WINDOW_RESIZABLE);
    if (!window.decorated)
        rl::SetWindowState(rl::FLAG_WINDOW_UNDECORATED);
    
    if (!window.vsync)
        if (window.infinite_fps)
            rl::SetTargetFPS(-1);
        else
            rl::SetTargetFPS(window.max_fps);
    else
        rl::SetWindowState(rl::FLAG_VSYNC_HINT);
}