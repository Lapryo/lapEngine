#include "world.hpp"
#include "eutil.hpp"

#include "systems/render_sys.hpp"
#include "systems/gui_sys.hpp"
#include "systems/physics_sys.hpp"
#include "systems/script_sys.hpp"
#include "systems/input_sys.hpp"

using namespace lapCore;

void World::RegisterElements()
{
    RegisterElement<Origin2D>("Origin2D");
    RegisterElement<Rotation2D>("Rotation2D");
    RegisterElement<Physics2D>("Physics2D");
    RegisterElement<Frame>("Frame");
    RegisterElement<UIList>("UIList");
    RegisterElement<Sprite>("Sprite");
    RegisterElement<Image>("Image");
    RegisterElement<TextLabel>("TextLabel");
    RegisterElement<EventBus>("EventBus");
    RegisterElement<UIButton>("UIButton");
    RegisterElement<Cam2D>("Cam2D");
    RegisterElement<Attribute<std::any>>("Attribute");
    RegisterElement<Script>("Script");
}

void World::SetScene(ProjectSceneData &scene_data)
{
    main_scene.Clear();

    main_scene = Scene(this, scene_data.name);
    for (auto &instance : scene_data.instances)
    {
        Object object = main_scene.AddObject(instance.name, instance.parent, instance.child_index);

        for (auto &element : instance.elements)
            // AI-generated code
            std::visit([&](auto&& data)
            {
                using T = std::decay_t<decltype(data)>;
                if constexpr (!std::is_same_v<T, std::monostate>)
                {
                    main_scene.AddElement<T>(main_scene.objects, object, data);
                }
                    
            }, element.data);
    }

    for (auto &system : scene_data.systems)
    {
        if (system.type == "render")
            main_scene.AddSystem<RenderSystem>(system.order);
        else if (system.type == "physics")
        {
            main_scene.AddSystem<PhysicsSystem>(system.order);
            main_scene.GetSystem<PhysicsSystem>()->RegisterBodies();
        }
        else if (system.type == "script")
            main_scene.AddSystem<ScriptSystem>(system.order);
        else if (system.type == "gui")
            main_scene.AddSystem<GUISystem>(system.order);
        else if (system.type == "input")
            main_scene.AddSystem<InputSystem>(system.order);
        else
            dbgln("Unknown system type: " + system.type, LogType::WARNING);
    }

    // cool thing to ensure there is a render system in the scene cause if not, the window wouldn't update at all and buffer the whole time
    if (IsWindowReady())
        if (main_scene.GetSystem<RenderSystem>() == nullptr)
            main_scene.AddSystem<RenderSystem>(-1);
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
    ResetWindowProperties();
    window.target = LoadRenderTexture(window.logical_resolution.x, window.logical_resolution.y);
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