#include "core.hpp"

#include <utility>
#include <iostream>
#include <fstream>
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

    for (auto &texture : resources.textures)
    {
        resources.RemoveTexture(texture.first);
    }

    for (auto &shader : resources.shaders)
    {
        resources.RemoveShader(shader.first);
    }

    // shutdown window if it exists
    CloseWindow();
}

Texture2D ResourceManager::AddTexture(const std::string &name, const std::string &filePath)
{
    Texture2D tex = LoadTexture(filePath.c_str());
    textures[name] = tex;
    return tex;
}

void ResourceManager::RemoveTexture(const std::string &name)
{
    textures.erase(name);
}

Shader ResourceManager::AddShader(const std::string &name, const std::string &vertexFilePath, const std::string &fragmentFilePath)
{
    Shader shader = LoadShader(vertexFilePath.c_str(), fragmentFilePath.c_str());
    shaders[name] = shader;
    return shader;
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

        std::cout << "X: " << transform.position.x << " | Y: " << transform.position.y << "\n";
    }
}

void RenderSystem::Update(float deltaTime, entt::registry &registry)
{
    auto view = registry.view<Transform2D, RectVisualizer>();
    view.each([deltaTime](auto entity, Transform2D& transform, RectVisualizer& rect) {
        DrawRectangle(transform.position.x, transform.position.y, rect.size.x, rect.size.y, rect.tint);
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

entt::entity Scene::AddEntity()
{
    const auto entity = entities.create();
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
                    auto entity = scene->AddEntity();

                    if (!object.contains("components") || !object["components"].is_array())
                        continue;

                    for (const auto& comp : object["components"])
                    {
                        const auto& type = comp.value("type", "");

                        if (!comp.contains("data")) continue;
                        const auto& data = comp["data"];

                        if (type == "transform2d") {
                            Transform2D transform;
                            transform.position = {
                                data["position"].at(0).get<float>(),
                                data["position"].at(1).get<float>()
                            };
                            transform.velocity = {
                                data["velocity"].at(0).get<float>(),
                                data["velocity"].at(1).get<float>()
                            };
                            transform.scale = {
                                data["scale"].at(0).get<float>(),
                                data["scale"].at(1).get<float>()
                            };
                            transform.rotation = data.value("rotation", 0.0f);

                            scene->AddComponent<Transform2D>(entity, transform);
                        }
                        else if (type == "rectvisualizer")
                        {
                            RectVisualizer visualizer;
                            visualizer.size = {
                                data["size"].at(0).get<float>(),
                                data["size"].at(1).get<float>()
                            };
                            visualizer.tint = RED;

                            scene->AddComponent<RectVisualizer>(entity, visualizer);

                            // handle other component types
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