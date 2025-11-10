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
            const auto& assetName = assetJson.value("name", "");
            const auto& assetType = assetJson.value("type", "");
            const auto& assetPath = assetJson.value("path", "");

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
            std::string systemType = systemJson.get<std::string>();
            std::cout << "[PROJECT] [SCENE] [SYSTEM] Adding system type: " << systemType << '\n';

            if (systemType == "physics")
            {
                scene->AddSystem<PhysicsSystem>();
            }
            else if (systemType == "render")
            {
                scene->AddSystem<RenderSystem>();
                // Add other systems
            }
            else if (systemType == "script")
            {
                scene->AddSystem<ScriptSystem>();
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

    for (const auto& comp : object["components"])
    {
        const auto& type = comp.value("type", "");
        std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] \t\tType: " << type << '\n';

        if (!comp.contains("data")) continue;
        const auto& data = comp["data"];

        std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] \t\tData: " << type << '\n';

        if (type == "transform2d") {
            Vector2 position{
                data["position"].at(0).get<float>(),
                data["position"].at(1).get<float>()
            };
            Vector2 velocity{
                data["velocity"].at(0).get<float>(),
                data["velocity"].at(1).get<float>()
            };
            Vector2 scale{
                data["scale"].at(0).get<float>(),
                data["scale"].at(1).get<float>()
            };
            float rotation = data.value("rotation", 0.0f);

            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [TRANSFORM2D] \t\t\tPosition: " << position.x << ", " << position.y << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [TRANSFORM2D] \t\t\tVelocity: " << velocity.x << ", " << velocity.y << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [TRANSFORM2D] \t\t\tScale: " << scale.x << " x " << scale.y << '\n';

            scene->AddComponent<Transform2D>(entity, position, velocity, scale, rotation);
        }
        else if (type == "rectvisualizer")
        {
            Vector2 offset{
                data["offset"].at(0).get<float>(),
                data["offset"].at(1).get<float>()
            };

            Vector2 size{
                data["size"].at(0).get<float>(),
                data["size"].at(1).get<float>()
            };

            Color tint{
                data["tint"].at(0).get<unsigned char>(),
                data["tint"].at(1).get<unsigned char>(),
                data["tint"].at(2).get<unsigned char>(),
                data["tint"].at(3).get<unsigned char>()
            };

            bool isScreenSpace = data["isScreenSpace"].get<bool>();
            unsigned int zlayer = data["zlayer"].get<unsigned int>();

            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [RECTVISUALIZER] \t\t\tOffset: " << offset.x << ", " << offset.y << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [RECTVISUALIZER] \t\t\tSize: " << size.x << " x " << size.y << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [RECTVISUALIZER] \t\t\tTint: " << tint.r << ", " << tint.g << ", " << tint.b << ", " << tint.a << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [RECTVISUALIZER] \t\t\tScreen Space: " << (isScreenSpace ? "True" : "False") << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [RECTVISUALIZER] \t\t\tZ-Layer: " << zlayer << '\n';

            scene->AddComponent<RectVisualizer>(entity, offset, size, tint, zlayer, isScreenSpace);
        }
        else if (type == "sprite")
        {
            Color tint{
                data["tint"].at(0).get<unsigned char>(),
                data["tint"].at(1).get<unsigned char>(),
                data["tint"].at(2).get<unsigned char>(),
                data["tint"].at(3).get<unsigned char>()
            };
            unsigned int zlayer = data["zlayer"].get<unsigned int>();
            bool isScreenSpace = data["isScreenSpace"].get<bool>();

            std::string texName = data["texture"].get<std::string>();

            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tTexture Name: " << texName << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tTint: " << tint.r << ", " << tint.g << ", " << tint.b << ", " << tint.a << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tScreen Space: " << (isScreenSpace ? "True" : "False") << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tZ-Layer: " << zlayer << '\n';

            scene->AddComponent<Sprite>(entity, texName, tint, zlayer, isScreenSpace);
        }
        else if (type == "textlabel")
        {
            std::string text = data.value("text", "");

            Vector2 textPosition{
                data["offset"].at(0).get<float>(),
                data["offset"].at(1).get<float>()
            };

            float textSize = data["size"].get<float>();

            Color textColor{
                data["color"].at(0).get<unsigned char>(),
                data["color"].at(1).get<unsigned char>(),
                data["color"].at(2).get<unsigned char>(),
                data["color"].at(3).get<unsigned char>()
            };

            bool isScreenSpace = data["isScreenSpace"].get<bool>();
            unsigned int zlayer = data["zlayer"].get<unsigned int>();

            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tText: " << text << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tOffset: " << textPosition.x << ", " << textPosition.y << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tSize: " << textSize << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tColor: " << textColor.r << ", " << textColor.g << ", " << textColor.b << ", " << textColor.a << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tScreen Space: " << (isScreenSpace ? "True" : "False") << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SPRITE] \t\t\tZ-Layer: " << zlayer << '\n';

            scene->AddComponent<TextLabel>(entity, text, textPosition, textSize, textColor, zlayer, isScreenSpace);
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
                data["offset"].at(1).get<float>()
            };

            Vector2 target{
                data["target"].at(0).get<float>(),
                data["target"].at(1).get<float>()
            };

            float rotation = data["rotation"].get<float>();
            float zoom = data["zoom"].get<float>();
            
            Camera2D camera;
            camera.offset = offset;
            camera.target = target;
            camera.rotation = rotation;
            camera.zoom = zoom;

            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [CAM2D] \t\t\tOffset: " << offset.x << ", " << offset.y << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [CAM2D] \t\t\tTarget: " << target.x << ", " << target.y << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [CAM2D] \t\t\tRotation: " << rotation << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [CAM2D] \t\t\tZoom: " << zoom << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [CAM2D] \t\t\tExclude List:\n";
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

            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SCRIPT] \t\t\tCreate Function: " << createFuncName << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SCRIPT] \t\t\tUpdate Function: " << updateFuncName << '\n';
            std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [SCRIPT] \t\t\tDestroy Function: " << destroyFuncName << '\n';

            scene->AddComponent<Script>(entity, ScriptRegistry::onCreateFunctions[createFuncName], ScriptRegistry::onUpdateFunctions[updateFuncName], ScriptRegistry::onDestroyFunctions[destroyFuncName], false);
        }
    }
}

void GetObjects(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &sceneJson, const std::string &projectName)
{
    std::cout << "[PROJECT] [SCENE] Gettings objects...";

    if (sceneJson.contains("objects") && sceneJson["objects"].is_array())
    {
        for (const auto& object : sceneJson["objects"])
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

Scene* Project::GetMainScene()
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
        windowJson["resolution"].at(1)
    };
    Vector2 logicalRes{
        windowJson["logical-resolution"].at(0),
        windowJson["logical-resolution"].at(1)
    };

    bool vsync = windowJson.value("vsync", false);
    bool inf_fps = windowJson.value("inf-fps", true);
    bool decorated = windowJson.value("decorated", true);

    std::string windowTitle = windowJson.value("title", "");

    InitWindow(windowRes.x, windowRes.y, windowTitle.c_str());
    for (auto& scene : scenes) {
        scene->LoadQueuedAssets();
        scene->logicalResolution = logicalRes;
    }

    if (resizable)
        SetWindowState(FLAG_WINDOW_RESIZABLE);

    if (windowMode == "fullscreen")
        SetWindowState(FLAG_FULLSCREEN_MODE);

    if (!decorated)
        SetWindowState(FLAG_WINDOW_UNDECORATED);
    
    if (!vsync)
        if (inf_fps)
            SetTargetFPS(-1);
        else
            SetTargetFPS(windowJson["max-fps"].get<int>());
    else
        SetWindowState(FLAG_VSYNC_HINT);

    main_scene = GetMainScene();

    target = LoadRenderTexture(logicalRes.x, logicalRes.y);
    SetTextureFilter(target.texture, TEXTURE_FILTER_TRILINEAR);
}