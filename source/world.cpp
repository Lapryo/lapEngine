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

    std::cout << "got to here. 5\n";

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
                    main_scene.AddElement<T>(main_scene.objects, object, data);
            }, element.data);
    }

    std::cout << "got to here. 6\n";

    for (auto &system : scene_data.systems)
    {
        std::cout << "got to here. 6.1\n";
        if (system.type == "render")
            main_scene.AddSystem<RenderSystem>(system.order);
        else if (system.type == "physics")
            main_scene.AddSystem<PhysicsSystem>(system.order);
        else if (system.type == "script")
            main_scene.AddSystem<ScriptSystem>(system.order);
        else if (system.type == "gui")
            main_scene.AddSystem<GUISystem>(system.order);
        else if (system.type == "input")
            main_scene.AddSystem<InputSystem>(system.order);
        else
            std::cout << "[WARNING] Unknown system type: " << system.type << '\n';
    }

    std::cout << "got to here. 7\n";

    // cool thing to ensure there is a render system in the scene cause if not, the window wouldn't update at all and buffer the whole time
    if (rl::IsWindowReady())
        if (main_scene.GetSystem<RenderSystem>() == nullptr)
            main_scene.AddSystem<RenderSystem>(-1);

    std::cout << "got to here. 8\n";
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