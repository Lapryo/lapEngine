#include "world.hpp"
#include "eutil.hpp"

#include "systems/render_sys.hpp"

using namespace lapCore;

void World::SetScene(ProjectSceneData &scene_data)
{
    // TODO: Implement scene reset to default
    main_scene.Clear();

    main_scene = Scene(this, scene_data.name);
    for (auto &instance : scene_data.instances)
    {
        Object object = main_scene.AddObject(instance.name, instance.parent, instance.child_index);

        for (auto &element : instance.elements)
        {
            if (element.type == "frame")
            {
                auto frameData = std::any_cast<Frame>(element.data);
                main_scene.AddElement<Frame>(main_scene.objects, object, frameData);
            }
        }
    }

    for (auto &system : scene_data.systems)
    {
        if (system.type == "render")
            main_scene.AddSystem<RenderSystem>(system.order);
    }
}

std::string defaultSettingsStr = R"(
{
    "window":
    {
        "title": "Default Window",
        "mode": "windowed",
        "decorated": true,
        "resizable": true,
        "resolution": [1200, 900],
        "logical-resolution": [800, 600],
        "vsync": false,
        "inf-fps": true,
        "max-fps": 60
    }
}
)";

void World::LoadSettings(const std::string &settingsFilePath)
{
    std::string fileStr = ReadFileToString(settingsFilePath);
    if (fileStr == "")
    {
        std::cout << "Could not get settings file, loading default.\n";
        fileStr = defaultSettingsStr;
    }

    auto settingsJson = nlohmann::json::parse(fileStr);
    auto &windowJson = settingsJson["window"];

    std::string windowMode = windowJson.value("mode", "windowed");
    bool resizable = windowJson.value("resizable", false);
    rl::Vector2 windowRes{
        windowJson["resolution"].at(0),
        windowJson["resolution"].at(1)};
    rl::Vector2 logicalRes{
        windowJson["logical-resolution"].at(0),
        windowJson["logical-resolution"].at(1)};

    bool vsync = windowJson.value("vsync", false);
    bool inf_fps = windowJson.value("inf-fps", true);
    bool decorated = windowJson.value("decorated", true);

    std::string windowTitle = windowJson.value("title", "");

    rl::InitWindow(windowRes.x, windowRes.y, windowTitle.c_str());

    if (resizable)
        rl::SetWindowState(rl::FLAG_WINDOW_RESIZABLE);

    if (!decorated)
        rl::SetWindowState(rl::FLAG_WINDOW_UNDECORATED);

    if (windowMode == "fullscreen")
        rl::SetWindowState(rl::FLAG_FULLSCREEN_MODE);

    if (!vsync)
        if (inf_fps)
            rl::SetTargetFPS(-1);
        else
            rl::SetTargetFPS(windowJson["max-fps"].get<int>());
    else
        rl::SetWindowState(rl::FLAG_VSYNC_HINT);

    SetScene(project.scenes[project.main_scene_index]);

    target = rl::LoadRenderTexture(logicalRes.x, logicalRes.y);
}