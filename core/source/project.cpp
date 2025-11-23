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
            else if (systemType == "gui")
            {
                scene->AddSystem<GUISystem>(systemOrder);
            }
        }
    }
    else
    {
        std::cout << "[WARNING] Project: \"" << projectName << "\", Scene: \"" << scene->name << "\" either did not contain systems or systems was not an array\n";
    }
}

void GetComponents(std::unique_ptr<Scene> &scene, const nlohmann::json_abi_v3_12_0::json &objectJson, Object &object)
{
    std::cout << "[PROJECT] [SCENE] [OBJECT] \tComponents:\n";

    for (const auto &comp : objectJson["components"])
    {
        const auto &type = comp.value("type", "");
        std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] \t\tType: " << type << '\n';

        if (!comp.contains("data"))
            continue;
        const auto &data = comp["data"];

        std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] \t\tData: " << type << '\n';

        if (type == "sprite")
        {
            rl::Color tint{
                data["tint"].at(0).get<unsigned char>(),
                data["tint"].at(1).get<unsigned char>(),
                data["tint"].at(2).get<unsigned char>(),
                data["tint"].at(3).get<unsigned char>()};
            unsigned int zlayer = data["zlayer"].get<unsigned int>();
            bool visible = data["renderable"].value("visible", true);
            bool isScreenSpace = data["isScreenSpace"].get<bool>();

            std::string texName = data["texture"].get<std::string>();

            Renderable renderable(zlayer, isScreenSpace, visible, tint);
            scene->AddElement<Sprite>(object, renderable, texName);
        }
        else if (type == "textlabel")
        {
            std::cout << "got here.\n";
            std::string text = data.value("text", "");
            float textSize = data.value("text-size", 0.0f);

            std::cout << "got here.\n";

            rl::Color textColor{
                data["renderable"]["tint"].at(0).get<unsigned char>(),
                data["renderable"]["tint"].at(1).get<unsigned char>(),
                data["renderable"]["tint"].at(2).get<unsigned char>(),
                data["renderable"]["tint"].at(3).get<unsigned char>()};

            std::cout << "got here.\n";

            rl::Vector2 bounds{
                data["bounds"].at(0).get<float>(),
                data["bounds"].at(1).get<float>()};

            std::cout << "got here.\n";

            Padding padding{
                data["padding"].at(0).get<float>(),
                data["padding"].at(1).get<float>(),
                data["padding"].at(2).get<float>(),
                data["padding"].at(3).get<float>()};

            std::cout << "got here.\n";

            HorizontalAlignment hAlign = HorizontalAlignment::LEFT;
            VerticalAlignment vAlign = VerticalAlignment::TOP;
            std::string hAlign_text = data.value("horizontal-alignment", "left");
            std::string vAlign_text = data.value("vertical-alignment", "top");

            if (hAlign_text == "middle")
                hAlign = HorizontalAlignment::MIDDLE;
            else if (hAlign_text == "right")
                hAlign = HorizontalAlignment::RIGHT;

            if (vAlign_text == "middle")
                vAlign = VerticalAlignment::MIDDLE;
            else if (hAlign_text == "bottom")
                vAlign = VerticalAlignment::BOTTOM;

            std::cout << "got here.\n";

            bool isScreenSpace = data["renderable"]["isScreenSpace"].get<bool>();
            unsigned int zlayer = data["renderable"]["zlayer"].get<unsigned int>();
            bool visible = data["renderable"]["visible"].get<bool>();

            Renderable renderable(zlayer, isScreenSpace, visible, textColor);
            renderable.visible = visible;

            std::cout << "got here.\n";

            if (!data.contains("position"))
            {
                std::cout << "Component missing position: " << data.dump(4) << "\n";
            }

            if (!data.contains("position") || !data["position"].is_array() || data["position"].size() < 4)
            {
                // handle missing or malformed position
                std::cerr << "Invalid position field\n";
            }

            FrameVector position{
                {data["position"][0].get<float>(),
                 data["position"][1].get<float>()},
                {data["position"][2].get<float>(),
                 data["position"][3].get<float>()}};

            std::cout << "got here.\n";

            FrameVector size{
                {data["size"][0].get<float>(),
                 data["size"][1].get<float>()},
                {data["size"][2].get<float>(),
                 data["size"][3].get<float>()}};

            std::cout << "got here.\n";

            UIOrigin origin(position, size);
            Frame frame(renderable, origin);

            Alignment textAlignment(hAlign, vAlign);
            FrameVector textBounds((rl::Vector2){0, 0}, (rl::Vector2){bounds.x, bounds.y});

            // Frame frame, std::string text, float textSize, Alignment textAlignment, FrameVector textBounds, Padding textPadding
            scene->AddElement<TextLabel>(object, frame, text, textSize, textAlignment, textBounds, padding);
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

            std::vector<Object> excludeList;

            rl::Vector2 offset{
                data["offset"].at(0).get<float>(),
                data["offset"].at(1).get<float>()};

            rl::Vector2 target{
                data["target"].at(0).get<float>(),
                data["target"].at(1).get<float>()};

            float rotation = data["rotation"].get<float>();
            float zoom = data["zoom"].get<float>();

            rl::Camera2D camera;
            camera.offset = offset;
            camera.target = target;
            camera.rotation = rotation;
            camera.zoom = zoom;

            for (auto &entity : excludeList)
            {
                std::cout << "[PROJECT] [SCENE] [OBJECT] [COMPONENT] [CAM2D] \t\t\t\t" << static_cast<std::uint16_t>(entity) << '\n';
            }

            scene->AddElement<Cam2D>(object, camera, excludeList);
        }
        else if (type == "script")
        {
            std::string createFuncName = data.value("onCreate", "");
            std::string updateFuncName = data.value("onUpdate", "");
            std::string destroyFuncName = data.value("onDestroy", "");

            scene->AddElement<Script>(object, createFuncName, updateFuncName, destroyFuncName);
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

            rl::Color tint{
                data["renderable"]["tint"].at(0).get<unsigned char>(),
                data["renderable"]["tint"].at(1).get<unsigned char>(),
                data["renderable"]["tint"].at(2).get<unsigned char>(),
                data["renderable"]["tint"].at(3).get<unsigned char>()};

            Renderable renderable(
                data["renderable"]["zlayer"].get<unsigned int>(),
                data["renderable"]["isScreenSpace"].get<bool>(), data["renderable"]["visible"].get<bool>(),
                tint);

            UIOrigin origin(position, size);

            // Renderable renderable, UIOrigin origin
            scene->AddElement<Frame>(object, renderable, origin);
        }
        else if (type == "UIList")
        {
            FrameVector scrollSize{
                {data["scroll-size"].at(0).get<float>(),
                 data["scroll-size"].at(1).get<float>()},
                {data["scroll-size"].at(2).get<float>(),
                 data["scroll-size"].at(3).get<float>()}};

            FrameVector displaySize{
                {data["display-size"].at(0).get<float>(),
                 data["display-size"].at(1).get<float>()},
                {data["display-size"].at(2).get<float>(),
                 data["display-size"].at(3).get<float>()}};

            bool hScrollBarRight = true;
            std::string horizontalScrollBar = data.value("horizontal-scroll-bar", "right");
            if (horizontalScrollBar == "left")
                hScrollBarRight = false;

            bool vScrollBarBottom = true;
            std::string verticalScrollBar = data.value("vertical-scroll-bar", "bottom");
            if (verticalScrollBar == "top")
                vScrollBarBottom = false;

            bool maskOutsideContent = data.value("mask-outside-content", true);

            float scrollOffset = data.value("scroll-offset", 0.0f);
            float scrollSpeed = data.value("scroll-speed", 20.0f);

            Axis2D direction = Axis2D::VERTICAL;
            std::string dirText = data.value("direction", "vertical");
            if (dirText == "horizontal")
                direction = Axis2D::HORIZONTAL;

            scene->AddElement<UIList>(object, scrollSize, displaySize, hScrollBarRight, vScrollBarBottom, maskOutsideContent, scrollOffset, scrollSpeed, direction);
        }
        else if (type == "origin2d")
        {
        }
        else if (type == "physics2d")
        {
        }
        else if (type == "ui-button")
        {
            EventBus eventBus;
            for (auto &eventJson : data["button-events"])
            {
                std::string name = eventJson.value("name", "");
                std::string event = eventJson.value("event", "");
                eventBus.events[name] = event;
            }

            rl::Vector2 scalePosition = {
                data["bounds"]["position"]["scale"].at(0).get<float>(),
                data["bounds"]["position"]["scale"].at(1).get<float>()};
            rl::Vector2 offsetPosition = {
                data["bounds"]["position"]["offset"].at(0).get<float>(),
                data["bounds"]["position"]["offset"].at(1).get<float>()};

            rl::Vector2 scaleSize = {
                data["bounds"]["size"]["scale"].at(0).get<float>(),
                data["bounds"]["size"]["scale"].at(1).get<float>()};
            rl::Vector2 offsetSize = {
                data["bounds"]["size"]["offset"].at(0).get<float>(),
                data["bounds"]["size"]["offset"].at(1).get<float>()};

            UIOrigin bounds(FrameVector(scalePosition, offsetPosition), FrameVector(scaleSize, offsetSize));
            bool active = data.value("active", true);

            // EventBus buttonEvents, UIOrigin bound
            scene->AddElement<UIButton>(object, eventBus, bounds, active);
        }
        else if (type == "attribute")
        {
            // TODO: SWITCH OVER TO THIS IN FUTURE, BUT FOR NOW, USING BOOL ATTRIB
            std::string attribType = data.value("type", "");
            if (attribType == "bool")
            {
                scene->AddElement<Attribute<bool>>(object, data.value("name", ""), data.value("value", false));
            }
        }
        else if (type == "event-bus")
        {
            std::unordered_map<std::string, std::string> events;
            for (auto &eventJson : data["events"])
            {
                events[eventJson.value("name", "")] = eventJson.value("event", "");
            }

            scene->AddElement<EventBus>(object, events);
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
            auto entity = scene->AddObject(object.value("name", ""), object.value("parent", ""), object.value("child-index", -1));
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

std::string lapCore::PackProject(Project project)
{
    std::cout << "[PROJECT] Packing project\n";
    nlohmann::json j;

    j["name"] = project.name;
    j["version"] = project.version;
    j["scenes"] = nlohmann::json::array();

    for (const auto &scenePtr : project.scenes)
    {
        nlohmann::json sceneJson;
        sceneJson["name"] = scenePtr->name;

        // Assets
        sceneJson["assets"] = nlohmann::json::array();
        for (const auto &asset : scenePtr->resources.textures)
        {
            nlohmann::json assetJson;
            assetJson["name"] = asset.first;
            assetJson["type"] = "texture";
            assetJson["path"] = ""; // You would need to store the path somewhere to retrieve it here

            sceneJson["assets"].push_back(assetJson);
        }

        for (const auto &asset : scenePtr->resources.shaders)
        {
            nlohmann::json assetJson;
            assetJson["name"] = asset.first;
            assetJson["type"] = "shader";
            assetJson["path"] = ""; // You would need to store the path somewhere to retrieve it here

            sceneJson["assets"].push_back(assetJson);
        }

        // Systems
        sceneJson["systems"] = nlohmann::json::array();
        for (const auto &systemPtr : scenePtr->systems)
        {
            if (systemPtr)
            {
                nlohmann::json systemJson;
                // Here you would need a way to get the type of the system
                // For simplicity, let's assume each system has a GetType() method
                // systemJson["type"] = systemPtr->GetType();
                systemJson["order"] = systemPtr->order;

                sceneJson["systems"].push_back(systemJson);
            }
        }

        // Objects would be packed here similarly

        for (const auto &objectEntryPair : scenePtr->objectMap)
        {
            const auto &objectEntry = objectEntryPair.second;
            nlohmann::json objectJson;
            objectJson["name"] = objectEntryPair.first;
            if (objectEntry.parent != entt::null)
            {
                objectJson["parent"] = scenePtr->GetObjectName(objectEntry.parent);
            }
            objectJson["child-index"] = 0; // You would need to determine the correct child index

            // Components would be packed here

            /*objectJson["components"] = nlohmann::json::array();
            auto origin2d = scenePtr->FindElement<Origin2D>(objectEntry.object);
            if (origin2d)
            {
                nlohmann::json compJson;
                compJson["type"] = "origin2d";
                compJson["data"] = nlohmann::json::object();
                // Fill in origin2d data here



                objectJson["components"].push_back(compJson);
            }*/

            // Doing components later...



            sceneJson["objects"].push_back(objectJson);
        }

        j["scenes"].push_back(sceneJson);
    }

    std::cout << "[PROJECT] Project packed successfully\n";
    return j.dump(4);

}

Project lapCore::UnpackProject(const std::string &projJson)
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
    for (auto &scene : scenes)
    {
        scene->LoadQueuedAssets();
        scene->logicalResolution = logicalRes;
    }

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

    main_scene = GetMainScene();

    target = rl::LoadRenderTexture(logicalRes.x, logicalRes.y);
}