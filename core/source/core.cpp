#include "core.hpp"

#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "json.hpp"

using namespace lapCore;

lapCore::App::App(Project &project)
{
    this->project = std::move(project);
}

void App::Run()
{
    float delta = 0;
    while (state == AppState::RUNNING)
    {
        delta = GetFrameTime();
        Update(delta);
        Draw();
    }
}

void App::Shutdown()
{
    for (auto &scene : project.scenes)
    {
        scene->Clear();
    }

    // shutdown window if it exists
    CloseWindow();
}

Texture2D* ResourceManager::AddTexture(const std::string &name, const std::string &filePath)
{
    auto tex = LoadTexture(filePath.c_str());
    textures[name] = tex;
    return &textures[name];
}

void ResourceManager::RemoveTexture(const std::string &name)
{
    textures.erase(name);
}

Shader* ResourceManager::AddShader(const std::string &name, const std::string &vertexFilePath, const std::string &fragmentFilePath)
{
    Shader shader = LoadShader(vertexFilePath.c_str(), fragmentFilePath.c_str());
    shaders[name] = shader;
    return &shaders[name];
}

void ResourceManager::RemoveShader(const std::string &name)
{
    shaders.erase(name);
}

void PhysicsSystem::Update(float deltaTime, entt::registry &registry)
{
    auto view = registry.view<Transform2D>();
    for (auto entity : view)
    {
        auto &transform = view.get<Transform2D>(entity);

        transform.position.x += transform.velocity.x * deltaTime;
        transform.position.y += transform.velocity.y * deltaTime;
    }
}

void RenderSystem::Update(float deltaTime, entt::registry &registry)
{
    // First, handle all world-space rendering.

    // Sort world-space sprites
    // We assume Sprite has both zlayer and isScreenSpace members.
    registry.sort<Sprite>([](const auto& lhs, const auto& rhs) { 
        // Ensure world-space items are sorted together
        if (lhs.isScreenSpace != rhs.isScreenSpace) {
            return !lhs.isScreenSpace; // World space (false) comes first
        }
        return lhs.zlayer < rhs.zlayer; 
    });

    auto cameraView = registry.view<Cam2D>();
    // Note: cameraView is a temporary view object that lives until the end of this scope.

    // Capture cameraView AND registry by reference in the lambda
    auto sortedSpriteView = registry.view<Transform2D, Sprite>();
    sortedSpriteView.each([deltaTime, &cameraView, &registry](auto entity, Transform2D& transform, Sprite& sprite) { 
        if (sprite.texture != nullptr)
        {
            if (!sprite.isScreenSpace) {
                // World space items need to be drawn with the camera applied
                for (auto ntt : cameraView)
                {
                    Cam2D cam = registry.get<Cam2D>(ntt);
                    BeginMode2D(cam.camera);
                    DrawTexture(*sprite.texture, transform.position.x, transform.position.y, sprite.tint); 
                    EndMode2D();
                }
            }
            else
            {
                // Screen space items are drawn directly
                DrawTexture(*sprite.texture, transform.position.x, transform.position.y, sprite.tint); 
            }
        }
    });

    // Capture cameraView AND registry by reference in the lambda
    auto otherView = registry.view<Transform2D, RectVisualizer>();
    otherView.each([deltaTime, &cameraView, &registry](auto entity, Transform2D& transform, RectVisualizer& rect) { 
        // We assume RectVisualizer also has an isScreenSpace boolean, or this system is purely world-space
        if (!rect.isScreenSpace) {
             for (auto ntt : cameraView)
            {
                Cam2D cam = registry.get<Cam2D>(ntt);
                BeginMode2D(cam.camera);
                DrawRectangle(transform.position.x, transform.position.y, rect.size.x, rect.size.y, rect.tint); 
                EndMode2D();
            }
        }
        else
        {
            DrawRectangle(transform.position.x, transform.position.y, rect.size.x, rect.size.y, rect.tint); 
        }
    });

    // Capture cameraView by reference in the lambda (registry was already captured here correctly)
    auto labelView = registry.view<TextLabel>();
    labelView.each([deltaTime, &registry, &cameraView](auto entity, TextLabel& textLabel) {
        const auto* transform = registry.try_get<Transform2D>(entity);

        float posX = textLabel.textPosition.x;
        float posY = textLabel.textPosition.y;

        if (transform) {
            // If the entity has a transform, apply its position
            posX += transform->position.x;
            posY += transform->position.y;
        }

        if (!textLabel.isScreenSpace) { // World space label
            for (auto ntt : cameraView)
            {
                Cam2D cam = registry.get<Cam2D>(ntt);
                BeginMode2D(cam.camera);
                DrawText(textLabel.text.c_str(), posX, posY, textLabel.textSize, textLabel.textColor);
                EndMode2D();
            }
        }
        else
        {
            // Screen space label
            DrawText(textLabel.text.c_str(), posX, posY, textLabel.textSize, textLabel.textColor);
        }
    });
}

void Scene::Update(float deltaTime, SystemDrawOrder order)
{
    for (auto &system : systems[order])
    {
        if (system) {
            system->Update(deltaTime, entities);
        }
    }
}

void Scene::AddSystem(std::unique_ptr<System> system)
{
    if (system) {
        systems[system->drawOrder].push_back(std::move(system));
    }
}

entt::entity Scene::AddEntity(const std::string &name)
{
    const auto entity = entities.create();
    nameToEntity[name] = entity;
    return entity;
}

void Scene::DestroyEntity(entt::entity& entity)
{
    entities.destroy(entity);
}

void Scene::Clear()
{
    entities.clear();
    systems.clear();

    for (auto &texture : resources.textures)
    {
        resources.RemoveTexture(texture.first);
    }

    for (auto &shader : resources.shaders)
    {
        resources.RemoveShader(shader.first);
    }
}

entt::entity Scene::FindEntity(const std::string &name)
{
    return nameToEntity[name];
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

enum class ProjectReadMode
{
    DATA,
    PROJECT,
    SCENE,
    SYSTEM,
    OBJECT,
    COMPONENT
};

std::string lapCore::ReadFileToString(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed.\n" << strerror(errno) << "\n";
        return ""; // Return an empty string on error
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // Read the entire file buffer into the stringstream
    file.close(); // Close the file stream

    return buffer.str(); // Convert the stringstream to a std::string
}

Project lapCore::UnpackProject(const char projJson[])
{
    //std::cout << "Opening project file... ";
    //std::string fileContents = ReadFileToString(projectPath);
    //std::cout << "Done.\nReading project file... ";

    Project project;

    nlohmann::json j = nlohmann::json::parse(projJson);

    std::string projectName = j.value("name", "Unnamed Project");

    project.name = projectName;
    project.version = j.value("version", "0.0");
    project.main_scene_index = -1;

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

            if (sceneJson.contains("assets") && sceneJson["assets"].is_array())
            {
                for (const auto &assetJson : sceneJson["assets"])
                {
                    const auto& assetName = assetJson.value("name", "");
                    const auto& assetType = assetJson.value("type", "");
                    const auto& assetPath = assetJson.value("path", "");

                    if (assetType == "texture")
                    {
                        // THIS WILL NOT WORK BECAUSE THE GL CONTEXT HAS NOT BEEN INITIATED YET.
                        // NEED TO MAKE THIS INTO A QUEUE THAT THEN LOADS THE CONTENTS AFTER THE GL CONTEXT IS INITIATED.
                        
                        // scene->resources.AddTexture(assetName, assetPath);
                    }
                }
            }
            
            if (sceneJson.contains("systems") && sceneJson["systems"].is_array())
            {
                for (const auto &systemJson : sceneJson["systems"])
                {
                    std::string systemType = systemJson.get<std::string>();

                    if (systemType == "physics")
                    {
                        scene->AddSystem(std::make_unique<PhysicsSystem>());
                    }
                    else if (systemType == "render")
                    {
                        scene->AddSystem(std::make_unique<RenderSystem>());
                        // Add other systems
                    }
                }
            }
            else
            {
                std::cout << "[WARNING] Project: \"" << projectName << "\", Scene: \"" << scene->name << "\" either did not contain systems or systems was not an array.\n";
            }

            if (sceneJson.contains("objects") && sceneJson["objects"].is_array())
            {
                for (const auto& object : sceneJson["objects"])
                {
                    auto entity = scene->AddEntity(object.value("name", ""));

                    if (!object.contains("components") || !object["components"].is_array())
                        continue;

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

                            scene->AddComponent<RectVisualizer>(entity, size, tint, isScreenSpace);
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

                            scene->AddComponent<Sprite>(entity, nullptr, tint, zlayer, isScreenSpace);
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

                            scene->AddComponent<TextLabel>(entity, text, textPosition, textSize, textColor, isScreenSpace);
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
                    }
                }
            }
            else
            {
                std::cout << "[WARNING] Project: \"" << projectName << "\", Scene: \"" << scene->name << "\" either did not contain objects or objects was not an array.\n";
            }

            project.scenes.push_back(std::move(scene));
        }
    }
    else
    {
        std::cout << "[WARNING] Project: \"" << projectName << "\" either did not contain scenes or scenes was not an array.\n";
    }

    std::cout << "Done.\n";

    return project;
}