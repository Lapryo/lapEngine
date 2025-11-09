#include "project.hpp"
#include "scriptregistry.hpp"
#include "json.hpp"

#include <iostream>

using namespace lapCore;

void GetAssets(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &sceneJson)
{
    if (sceneJson.contains("assets") && sceneJson["assets"].is_array())
    {
        for (const auto &assetJson : sceneJson["assets"])
        {
            const auto& assetName = assetJson.value("name", "");
            const auto& assetType = assetJson.value("type", "");
            const auto& assetPath = assetJson.value("path", "");

            scene->QueueAsset(assetName, assetType, assetPath);
        }
    }
}

void GetSystems(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &sceneJson, const std::string &projectName)
{
    if (sceneJson.contains("systems") && sceneJson["systems"].is_array())
    {
        for (const auto &systemJson : sceneJson["systems"])
        {
            std::string systemType = systemJson.get<std::string>();

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
        std::cout << "[WARNING] Project: \"" << projectName << "\", Scene: \"" << scene->name << "\" either did not contain systems or systems was not an array.\n";
    }
}

void GetComponents(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &object, entt::entity &entity)
{
    for (const auto& comp : object["components"])
    {
        const auto& type = comp.value("type", "");

        if (!comp.contains("data")) continue;
        const auto& data = comp["data"];

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

            scene->AddComponent<Transform2D>(entity, position, velocity, scale, rotation);
        }
        else if (type == "rectvisualizer")
        {
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

            scene->AddComponent<RectVisualizer>(entity, size, tint, zlayer, isScreenSpace);
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

            scene->AddComponent<Sprite>(entity, texName, tint, zlayer, isScreenSpace);
        }
        else if (type == "textlabel")
        {
            std::string text = data.value("text", "");

            Vector2 textPosition{
                data["position"].at(0).get<float>(),
                data["position"].at(1).get<float>()
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

            scene->AddComponent<Cam2D>(entity, camera, excludeList);
        }
        else if (type == "script")
        {
            std::string createFuncName = data["onCreate"];
            std::string updateFuncName = data["onUpdate"];
            std::string destroyFuncName = data["onDestroy"];

            scene->AddComponent<Script>(entity, ScriptRegistry::onCreateFunctions[createFuncName], ScriptRegistry::onUpdateFunctions[updateFuncName], ScriptRegistry::onDestroyFunctions[destroyFuncName], true);
        }
    }
}

void GetObjects(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &sceneJson, const std::string &projectName)
{
    if (sceneJson.contains("objects") && sceneJson["objects"].is_array())
    {
        for (const auto& object : sceneJson["objects"])
        {
            auto entity = scene->AddEntity(object.value("name", ""));

            if (!object.contains("components") || !object["components"].is_array())
                continue;

            GetComponents(scene, object, entity);
        }
    }
    else
    {
        std::cout << "[WARNING] Project: \"" << projectName << "\", Scene: \"" << scene->name << "\" either did not contain objects or objects was not an array.\n";
    }
}

void GetScenes(Project &project, const nlohmann::json_abi_v3_12_0::json &j, const std::string &projectName)
{
    if (j.contains("scenes") && j["scenes"].is_array())
    {
        const auto &scenesJson = j["scenes"];
        project.scenes.reserve(scenesJson.size());

        for (const auto &sceneJson : scenesJson)
        {
            auto scene = std::make_unique<Scene>();
            scene->name = sceneJson.value("name", "Unnamed Scene");

            if (scene->name == "main")
                project.main_scene_index = static_cast<int>(project.scenes.size());

            GetAssets(scene, sceneJson);
            GetSystems(scene, sceneJson, projectName);
            GetObjects(scene, sceneJson, projectName);

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
    Project project;

    nlohmann::json j = nlohmann::json::parse(projJson);

    std::string projectName = j.value("name", "Unnamed Project");

    project.name = projectName;
    project.version = j.value("version", "0.0");
    project.main_scene_index = -1;

    GetScenes(project, j, projectName);

    // add to the exclude list for cam2d components here.

    std::cout << "Done.\n";

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
    
}