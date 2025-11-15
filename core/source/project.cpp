#include "core.hpp"
#include "scriptregistry.hpp"
#include "json.hpp"

#include <iostream>

using namespace lapCore;

/*
    TODO:
        REWORK THE OUTPUT FOR THIS, IT LOOKS TRASH
*/

void GetAssets(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &sceneJson)
{
    std::cout << "[PROJECT] [SCENE] Getting assets...\n";

    if (sceneJson.contains("assets") && sceneJson["assets"].is_array())
    {
        for (const auto &assetJson : sceneJson["assets"])
        {
            const auto &assetName = assetJson.value("name", "");
            const auto &assetType = assetJson.value("type", "");
            const auto &assetPath = assetJson.value("path", "");

            std::cout << "[PROJECT] [SCENE] [ASSET] Queueing asset:\n[PROJECT] [SCENE] [ASSET] \tName: " << assetName << "\n[PROJECT] [SCENE] [ASSET] \tType: " << assetType << "[PROJECT] [SCENE] [ASSET] \tPath: " << assetPath << '\n';

            scene->QueueAsset(assetName, assetType, assetPath);
        }
    }
}

void GetSystems(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &sceneJson, const std::string &projectName)
{
    std::cout << "[PROJECT] [SCENE] Getting systems...\n";

    if (sceneJson.contains("systems") && sceneJson["systems"].is_array())
    {
        for (const auto &systemJson : sceneJson["systems"])
        {
            std::string systemType = systemJson.value("type", "");
            unsigned int systemOrder = systemJson.value("order", -1);
            std::cout << "[PROJECT] [SCENE] [SYSTEM] Adding system type: " << systemType << '\n';

            if (systemType == "physics")
            {
                scene->AddSystem<PhysicsSystem>(systemOrder);
            }
            else if (systemType == "render")
            {
                scene->AddSystem<RenderSystem>(systemOrder);
                // Add other systems
            }
            else if (systemType == "script")
            {
                scene->AddSystem<ScriptSystem>(systemOrder);
            }
        }
    }
    else
    {
        std::cout << "[WARNING] Project: \"" << projectName << "\", Scene: \"" << scene->name << "\" either did not contain systems or systems was not an array\n";
    }
}

void GetComponents(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &object, entt::entity &entity)
{
    std::cout << "[PROJECT] [SCENE] [OBJECT] \tComponents:\n";

    for (const auto &comp : object["components"])
    {
        const auto &type = comp.value("type", "");
        std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] \t\tType: " << type << '\n';

        if (!comp.contains("data"))
            continue;
        const auto &data = comp["data"];

        std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] \t\tData: " << type << '\n';

        if (type == "sprite")
        {
            Color tint{
                data["tint"].at(0).get<unsigned char>(),
                data["tint"].at(1).get<unsigned char>(),
                data["tint"].at(2).get<unsigned char>(),
                data["tint"].at(3).get<unsigned char>()};
            unsigned int zlayer = data["zlayer"].get<unsigned int>();
            bool isScreenSpace = data["isScreenSpace"].get<bool>();

            std::string texName = data["texture"].get<std::string>();

            Renderable renderable(zlayer, isScreenSpace, tint);
            scene->AddComponent<Sprite>(entity, renderable, texName);
        }
        else if (type == "textlabel")
        {
            std::string text = data.value("text", "");
            float textSize = data["size"].get<float>();

            Color textColor{
                data["renderable"]["tint"].at(0).get<unsigned char>(),
                data["renderable"]["tint"].at(1).get<unsigned char>(),
                data["renderable"]["tint"].at(2).get<unsigned char>(),
                data["renderable"]["tint"].at(3).get<unsigned char>()};

            Vector2 bounds{
                data["bounds"].at(0).get<float>(),
                data["bounds"].at(1).get<float>()};

            HorizontalAlignment hAlign = HorizontalAlignment::LEFT;
            VerticalAlignment vAlign = VerticalAlignment::TOP;
            std::string hAlign_text = data.value("horizontal-alignment", "left");
            std::string vAlign_text = data.value("vertical-alignment", "top");

            if (hAlign_text == "middle")
                hAlign = HorizontalAlignment::MIDDLE;
            else
                hAlign = HorizontalAlignment::RIGHT;

            if (vAlign_text == "middle")
                vAlign = VerticalAlignment::MIDDLE;
            else
                vAlign = VerticalAlignment::BOTTOM;

            bool isScreenSpace = data["renderable"]["isScreenSpace"].get<bool>();
            unsigned int zlayer = data["renderable"]["zlayer"].get<unsigned int>();
            bool visible = data["renderable"]["visible"].get<bool>();

            Renderable renderable(zlayer, isScreenSpace, textColor);
            renderable.visible = visible;

            scene->AddComponent<TextLabel>(entity, renderable, text, textSize, hAlign, vAlign, bounds);
        }
        else if (type == "cam2d")
        {
            // THIS ALSO NEEDS TO BE PUSHED TO AFTER THE UNPACKING IS COMPLETE

            /*
            std::vector<entt::entity> excludeList;
            if (data["exclude"] && data["exclude"].is_array())
                for (const auto& objName : data["exclude"])
                {
                    excludeList.push_back(objName);
                }
            }
            */

            std::vector<entt::entity> excludeList;

            Vector2 offset{
                data["offset"].at(0).get<float>(),
                data["offset"].at(1).get<float>()};

            Vector2 target{
                data["target"].at(0).get<float>(),
                data["target"].at(1).get<float>()};

            float rotation = data["rotation"].get<float>();
            float zoom = data["zoom"].get<float>();

            Camera2D camera;
            camera.offset = offset;
            camera.target = target;
            camera.rotation = rotation;
            camera.zoom = zoom;

            for (auto &entity : excludeList)
            {
                std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [CAM2D] \t\t\t\t" << static_cast<std::uint16_t>(entity) << '\n';
            }

            scene->AddComponent<Cam2D>(entity, camera, excludeList);
        }
        else if (type == "script")
        {
            std::string createFuncName = data["onCreate"];
            std::string updateFuncName = data["onUpdate"];
            std::string destroyFuncName = data["onDestroy"];

            scene->AddComponent<Script>(entity, ScriptRegistry::onCreateFunctions[createFuncName], ScriptRegistry::onUpdateFunctions[updateFuncName], ScriptRegistry::onDestroyFunctions[destroyFuncName], false);
        }
        else if (type == "frame")
        {
            FrameVector position{
                {data["position"].at(0).get<float>(),
                 data["position"].at(1).get<float>()},
                {data["position"].at(2).get<float>(),
                 data["position"].at(3).get<float>()}};

            FrameVector size{
                {data["size"].at(0).get<float>(),
                 data["size"].at(1).get<float>()},
                {data["size"].at(2).get<float>(),
                 data["size"].at(3).get<float>()}};

            float rotation = data.value("rotation", 0.0f);
            Vector2 anchor{
                data["anchor"].at(0).get<float>(),
                data["anchor"].at(1).get<float>()};

            Color tint{
                data["renderable"]["tint"].at(0).get<unsigned char>(),
                data["renderable"]["tint"].at(1).get<unsigned char>(),
                data["renderable"]["tint"].at(2).get<unsigned char>(),
                data["renderable"]["tint"].at(3).get<unsigned char>()};

            Renderable renderable(
                data["renderable"]["zlayer"].get<unsigned int>(),
                data["renderable"]["isScreenSpace"].get<bool>(),
                tint);

            // Renderable renderable, FrameVector position, FrameVector size, float rotation, Alignment horizontal, Alignment vertical, Vector2 anchor
            scene->AddComponent<Frame>(entity, renderable, position, size, rotation, anchor);
        }
        else if (type == "origin2d")
        {
        }
        else if (type == "physics2d")
        {
        }
    }
}

void GetObjects(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &sceneJson, const std::string &projectName)
{
    std::cout << "[PROJECT] [SCENE] Gettings objects...";

    if (sceneJson.contains("objects") && sceneJson["objects"].is_array())
    {
        for (const auto &object : sceneJson["objects"])
        {
            auto entity = scene->AddEntity(object.value("name", ""));
            std::cout << "[PROJECT] [SCENE] [OBJECT] Loading object:\n[PROJECT] [SCENE] [OBJECT] \tName: " << object.value("name", "") << '\n';

            if (!object.contains("components") || !object["components"].is_array())
                continue;

            GetComponents(scene, object, entity);
        }
    }
    else
    {
        std::cout << "[WARNING] Project: \"" << projectName << "\", Scene: \"" << scene->name << "\" either did not contain objects or objects was not an array\n";
    }
}

void GetScenes(Project &project, const nlohmann::json_abi_v3_12_0::json &j, const std::string &projectName)
{
    std::cout << "[PROJECT] Getting scenes...\n";

    if (j.contains("scenes") && j["scenes"].is_array())
    {
        const auto &scenesJson = j["scenes"];
        project.scenes.reserve(scenesJson.size());

        for (const auto &sceneJson : scenesJson)
        {
            auto scene = std::make_unique<Scene>();
            scene->name = sceneJson.value("name", "Unnamed Scene");

            std::cout << "[PROJECT] [SCENE] Name: " << scene->name << '\n';

            if (scene->name == "main")
            {
                std::cout << "[PROJECT] [SCENE] Current scene is the main scene\n";
                project.main_scene_index = static_cast<int>(project.scenes.size());
            }

            GetAssets(scene, sceneJson);
            GetSystems(scene, sceneJson, projectName);
            GetObjects(scene, sceneJson, projectName);

            std::cout << "[PROJECT] [SCENE] Finished loading scene\n";

            project.scenes.push_back(std::move(scene));
        }
    }
    else
    {
        std::cout << "[WARNING] Project: \"" << projectName << "\" either did not contain scenes or scenes was not an array.\n";
    }
}

Project lapCore::UnpackProject(const char projJson[])
{
    std::cout << "[PROJECT] Unpacking project\n";
    Project project;

    std::cout << "[PROJECT] Parsing json file\n";
    nlohmann::json j = nlohmann::json::parse(projJson);

    std::string projectName = j.value("name", "Unnamed Project");

    project.name = projectName;
    project.version = j.value("version", "0.0");
    project.main_scene_index = -1;

    std::cout << "[PROJECT] Name: " << projectName << "\n[PROJECT] Version: " << project.version << '\n';

    GetScenes(project, j, projectName);

    // add to the exclude list for cam2d components here.

    std::cout << "[PROJECT] Project loaded succesfully\n";

    return project;
}

Scene *Project::GetMainScene()
{
    if (main_scene_index >= 0 && main_scene_index < (int)scenes.size())
        return scenes[main_scene_index].get();
    return nullptr;
}

void Project::Clear()
{
    for (auto &scene : scenes)
    {
        scene->Clear();
    }
}

void Project::LoadSettings(const std::string &settingsFilePath)
{
    auto settingsJson = nlohmann::json::parse(ReadFileToString(settingsFilePath));
    auto &windowJson = settingsJson["window"];

    std::string windowMode = windowJson.value("mode", "windowed");
    bool resizable = windowJson.value("resizable", false);
    Vector2 windowRes{
        windowJson["resolution"].at(0),
        windowJson["resolution"].at(1)};
    Vector2 logicalRes{
        windowJson["logical-resolution"].at(0),
        windowJson["logical-resolution"].at(1)};

    bool vsync = windowJson.value("vsync", false);
    bool inf_fps = windowJson.value("inf-fps", true);
    bool decorated = windowJson.value("decorated", true);

    std::string windowTitle = windowJson.value("title", "");

    InitWindow(windowRes.x, windowRes.y, windowTitle.c_str());
    for (auto &scene : scenes)
    {
        scene->LoadQueuedAssets();
        scene->logicalResolution = logicalRes;
    }

    if (resizable)
        SetWindowState(FLAG_WINDOW_RESIZABLE);

    if (!decorated)
        SetWindowState(FLAG_WINDOW_UNDECORATED);

    if (windowMode == "fullscreen")
        SetWindowState(FLAG_FULLSCREEN_MODE);

    if (!vsync)
        if (inf_fps)
            SetTargetFPS(-1);
        else
            SetTargetFPS(windowJson["max-fps"].get<int>());
    else
        SetWindowState(FLAG_VSYNC_HINT);

    main_scene = GetMainScene();

    target = LoadRenderTexture(logicalRes.x, logicalRes.y);
}