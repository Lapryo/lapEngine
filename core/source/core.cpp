#include "core.hpp"

#include <utility>
#include <iostream>
#include <fstream>
#include <cstring>     // for strerror
#include <cerrno>      // for errno
#include <cstdio>      // for FILE operations
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
    }
}

void App::Shutdown()
{
    for (auto &scene : project.scenes)
    {
        scene.Clear();
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

void Scene::Update(float deltaTime)
{
    for (System* system : systems)
    {
        if (system) {
            system->Update(deltaTime, entities);
        }
    }
}

void Scene::AddSystem(System* system)
{
    if (system) {
        systems.push_back(system);
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
    for (auto* system : systems) {
        delete system;
    }
    systems.clear();
}

void Project::Clear()
{
    for (auto &scene : scenes)
    {
        scene.Clear();
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

#include <typeinfo>

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

Project lapCore::UnpackProject(const std::string &projectPath)
{
    std::cout << "Opening project file... ";
    std::string fileContents = ReadFileToString(projectPath);
    std::cout << "Done.\nReading project file... ";

    Project project;

    nlohmann::json j = nlohmann::json::parse(fileContents);

    std::string projectName = j["name"];
    std::string projectVersion = j["version"];

    project.name = projectName;
    project.version = projectVersion;
    project.path = projectPath;

    auto scenes = j["scenes"];

    for (int i = 0; i < scenes.size(); ++i)
    {
        Scene scene;
        scene.name = scenes[i]["name"];
        if (scene.name == "main")
            project.main_scene = &scene;

        std::cout << "got to here. 1\n";
        
        auto systems = scenes[i]["systems"];
        std::cout << "got to here. 2\n";
        for (auto system : systems)
        {
            if (system == "physics")
                scene.AddSystem(new PhysicsSystem());
            else if (system == "renderer")
            {
                // add other systems as such
            }
        }

        auto objects = scenes[i]["objects"];
        std::cout << "got to here. 3\n";
        for (auto object : objects)
        {
            auto entity = scene.AddEntity();
            
            auto components = object["components"];
            std::cout << "got to here. 4\n";
            for (auto comp : components)
            {
                auto data = comp["data"];
                std::cout << "got to here. 5\n";
                std::cout << data << '\n';

                if (comp["type"] == "transform2d")
                {
                    Transform2D transform;

                    std::cout << data["position"];
                    std::cout << "got to here so i fucking kill myself and die and kill the person right next to me.\n";
                    transform.position = (Vector2){data["position"][0], data["position"][1]};
                    std::cout << "got to here. 6\n";
                    transform.velocity = (Vector2){data["velocity"][0], data["velocity"][1]};
                    transform.scale = (Vector2){data["scale"][0], data["scale"][1]};
                    transform.rotation = (float)data["rotation"];

                    scene.AddComponent<Transform2D>(entity, transform);
                }
                else {
                    // add other component types as such
                }
            }
        }

        std::cout << "got to where we put the scene inside of scenes.\n";

        project.scenes.push_back(std::move(scene));
        std::cout << "got past.\n";
    }

    std::cout << j << "\n";

    std::cout << projectName << '\n' << projectVersion << '\n' << scenes << '\n' << typeid(scenes).name() << '\n';



    /*char buffer[512] = "";

    Project project;

    ProjectReadMode mode = ProjectReadMode::DATA;
    while (fgets(buffer, sizeof(buffer), projFile))
    {
        std::string line = buffer;
        size_t found = std::string::npos;

        found = line.find("PROJECT");
        if (found != std::string::npos)
            mode = ProjectReadMode::PROJECT;
        else 
        {
            found = line.find("SCENE");
            if (found != std::string::npos)
                mode = ProjectReadMode::SCENE;
            else
            {
                found = line.find("SYSTEM");
                if (found != std::string::npos)
                    mode = ProjectReadMode::SYSTEM;
                else
                {
                    found = line.find("OBJECT");
                    if (found != std::string::npos)
                        mode = ProjectReadMode::OBJECT;
                    else
                    {
                        found = line.find("COMPONENT");
                        if (found != std::string::npos)
                            mode = ProjectReadMode::COMPONENT;
                        else
                            mode = ProjectReadMode::DATA;
                    }
                }
            }
        }

        switch (mode)
        {
            case ProjectReadMode::PROJECT:
            {
                line = line.substr(found + 9, line.length() - 1);
                std::string name = line;
                break;
            }
            case ProjectReadMode::SCENE:
            {
                line = line.substr(found + 7);
                std::string name = line;
                break;
            }
            case ProjectReadMode::SYSTEM:
            {
                line = line.substr(found + 7);
                size_t namePos = line.find('\"');
                std::string type = line.substr(0, namePos - 1);
                std::string name = line.substr(namePos + 1);
                line = type;
                break;
            }
            case ProjectReadMode::OBJECT:
            {
                line = line.substr(found + 8);
                break;
            }
            case ProjectReadMode::COMPONENT:
            {
                line = line.substr(found + 10);
                break;
            }
            case ProjectReadMode::DATA:
            {
                
                break;
            }
        }

        std::cout << line;


    }*/

    // go through lines and get all the data

    std::cout << "Done.\n";

    return project;
}