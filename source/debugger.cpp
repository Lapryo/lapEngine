#include "debugger.hpp"

#ifdef LAP_DEBUG

#include <imgui.h>
#include <rlImGui.h>

#include "middleman.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#endif

using namespace lapCore;

#ifdef LAP_DEBUG

// ============================================================
// Inspector State
// ============================================================

struct ObjectInspector
{
    Scene* scene;
    Object object;
};

static std::vector<Scene*> openScenes;
static std::vector<ObjectInspector> openObjects;

// ============================================================
// Inspector Titles
// ============================================================

static std::string SceneTitle(Scene* scene)
{
    return "Scene: " + GET_FORMER_HASHED(scene->id) +
           "###scene_" +
           std::to_string(reinterpret_cast<std::uintptr_t>(scene));
}

static std::string ObjectTitle(Scene* scene, Object object)
{
    auto* info = scene->objects.try_get<ObjectData>(object);

    std::string name = info
        ? GET_FORMER_HASHED(info->id)
        : "Unknown";

    return "Object: " + name +
           "###object_" +
           std::to_string(reinterpret_cast<std::uintptr_t>(scene)) +
           "_" +
           std::to_string(entt::to_integral(object));
}

// ============================================================
// Open Inspectors
// ============================================================

static void OpenScene(Scene* scene)
{
    auto it = std::find(
        openScenes.begin(),
        openScenes.end(),
        scene
    );

    if (it == openScenes.end())
        openScenes.push_back(scene);
    else
        ImGui::SetWindowFocus(SceneTitle(scene).c_str());
}

static void OpenObject(Scene* scene, Object object)
{
    auto it = std::find_if(
        openObjects.begin(),
        openObjects.end(),
        [&](const ObjectInspector& inspector)
        {
            return inspector.scene == scene &&
                   inspector.object == object;
        }
    );

    if (it == openObjects.end())
        openObjects.push_back({scene, object});
    else
        ImGui::SetWindowFocus(ObjectTitle(scene, object).c_str());
}

// ============================================================
// Object Browser
// ============================================================

static void DrawObjects(Scene* scene)
{
    for (auto [object, info] : scene->objects.view<ObjectData>().each())
    {
        ImGui::PushID(
            static_cast<int>(entt::to_integral(object))
        );

        if (ImGui::Selectable(GET_FORMER_HASHED(info.id).c_str()))
        {
            OpenObject(scene, object);
        }

        ImGui::PopID();
    }
}

// ============================================================
// Object Inspector
// ============================================================

static bool DrawObjectInspector(Scene* scene, Object object)
{
    auto& objects = scene->objects;

    if (!objects.valid(object))
        return false;

    auto* info = objects.try_get<ObjectData>(object);

    if (!info)
        return false;

    bool open = true;

    std::string title = ObjectTitle(scene, object);

    ImGui::SetNextWindowSize(
        ImVec2(350, 400),
        ImGuiCond_FirstUseEver
    );

    if (ImGui::Begin(title.c_str(), &open))
    {
        ImGui::Text(
            "Name: %s",
            GET_FORMER_HASHED(info->id).c_str()
        );

        ImGui::Text(
            "Object ID: %u",
            static_cast<unsigned>(entt::to_integral(object))
        );

        // Parent

        if (info->parent == entt::null)
        {
            ImGui::Text("Parent: null");
        }
        else if (auto* parent =
            objects.try_get<ObjectData>(info->parent))
        {
            ImGui::Text(
                "Parent: %s (%u)",
                GET_FORMER_HASHED(parent->id).c_str(),
                static_cast<unsigned>(
                    entt::to_integral(info->parent)
                )
            );
        }
        else
        {
            ImGui::Text("Parent: Invalid");
        }

        ImGui::Text("Index: %d", info->index);

        // Children

        if (ImGui::CollapsingHeader("Children"))
        {
            if (info->children.empty())
            {
                ImGui::TextDisabled("No children");
            }

            for (const auto& [index, child] : info->children)
            {
                if (child == entt::null)
                {
                    ImGui::Text("Child %d: null", index);
                }
                else if (auto* data =
                    objects.try_get<ObjectData>(child))
                {
                    ImGui::Text(
                        "Child %d: %s (%u)",
                        index,
                        GET_FORMER_HASHED(data->id).c_str(),
                        static_cast<unsigned>(
                            entt::to_integral(child)
                        )
                    );
                }
                else
                {
                    ImGui::Text("Child %d: Invalid", index);
                }
            }
        }

        // Elements

        if (ImGui::CollapsingHeader("Elements"))
        {
            bool hasElements = false;

            for (auto&& [type, storage] : objects.storage())
            {
                const auto& typeInfo = storage.info();

                if (typeInfo.hash() == GetTypeID<ObjectData>())
                    continue;

                if (!storage.contains(object))
                    continue;

                hasElements = true;

                auto* entry = Middleman::TryGet(typeInfo.hash());

                if (entry)
                {
                    ImGui::BulletText("%s", entry->name.c_str());
                }
                else
                {
                    auto name = typeInfo.name();

                    ImGui::BulletText(
                        "%.*s (Unregistered)",
                        static_cast<int>(name.size()),
                        name.data()
                    );
                }
            }

            if (!hasElements)
                ImGui::TextDisabled("No elements");
        }
    }

    ImGui::End();

    return open;
}

// ============================================================
// Scene Inspector
// ============================================================

static bool DrawSceneInspector(Scene* scene)
{
    bool open = true;

    std::string title = SceneTitle(scene);

    ImGui::SetNextWindowSize(
        ImVec2(350, 450),
        ImGuiCond_FirstUseEver
    );

    if (ImGui::Begin(title.c_str(), &open))
    {
        ImGui::Text(
            "Name: %s",
            GET_FORMER_HASHED(scene->id).c_str()
        );

        const auto& color = scene->backgroundColor;

        ImGui::Text("Background color:");
        ImGui::SameLine();

        ImGui::ColorButton(
            "##backgroundColor",
            ImVec4(
                color.r / 255.0f,
                color.g / 255.0f,
                color.b / 255.0f,
                color.a / 255.0f
            ),
            ImGuiColorEditFlags_NoTooltip,
            ImVec2(20, 20)
        );

        // Objects

        if (ImGui::CollapsingHeader(
            "Objects",
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            DrawObjects(scene);
        }

        // Systems

        if (ImGui::CollapsingHeader("Systems"))
        {
            for (const auto& [order, system] : scene->systems)
            {
                if (!system)
                    continue;

                ImGui::Text(
                    "[%d] %s: %s (%.3f ms)",
                    static_cast<int>(order),
                    system->GetName().c_str(),
                    system->active ? "active" : "inactive",
                    scene->timers[system->GetName()] * 1000.f
                );
            }
        }
    }

    ImGui::End();

    return open;
}

// ============================================================
// Window Inspector
// ============================================================

static void DrawWindowInfo(Scene* scene)
{
    auto& window = scene->world->window;

    ImGui::Text(
        "Title: %s",
        window.title.c_str()
    );

    ImGui::Text(
        "Resolution: (%d, %d)",
        static_cast<int>(window.resolution.x),
        static_cast<int>(window.resolution.y)
    );

    ImGui::Text(
        "Logical Resolution: (%d, %d)",
        static_cast<int>(window.logical_resolution.x),
        static_cast<int>(window.logical_resolution.y)
    );

    ImGui::Text(
        "Fullscreen: %s",
        window.fullscreen ? "true" : "false"
    );

    ImGui::Text(
        "Borderless: %s",
        window.borderless ? "true" : "false"
    );

    ImGui::Text(
        "Decorated: %s",
        window.decorated ? "true" : "false"
    );

    ImGui::Text(
        "Resizable: %s",
        window.resizable ? "true" : "false"
    );

    ImGui::Text(
        "VSync: %s",
        window.vsync ? "true" : "false"
    );

    if (window.infinite_fps)
        ImGui::Text("FPS Cap: Inf");
    else
        ImGui::Text("FPS Cap: %d", window.max_fps);

    ImGui::Separator();

    ImGui::Text("FPS: %d", GetFPS());

    ImGui::Text(
        "Frame time: %.3f ms",
        GetFrameTime() * 1000.0f
    );
}

#endif

// ============================================================
// Debugger GUI
// ============================================================

void DebuggerGUI::Draw(Scene* scene)
{
#ifdef LAP_DEBUG

    if (!scene || !scene->world)
        return;

    auto* world = scene->world;

    rlImGuiBegin();

    // Main debugger window

    ImGui::SetNextWindowSize(
        ImVec2(300, 400),
        ImGuiCond_FirstUseEver
    );

    if (ImGui::Begin("World Info"))
    {
        ImGui::Text(
            "Main scene: %s",
            GET_FORMER_HASHED(scene->id).c_str()
        );

        // Window

        if (ImGui::CollapsingHeader("Window"))
        {
            DrawWindowInfo(scene);
        }

        // Scenes

        if (ImGui::CollapsingHeader(
            "Scenes",
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (auto* currentScene : world->scenes)
            {
                if (!currentScene)
                    continue;

                ImGui::PushID(currentScene);

                if (ImGui::Selectable(
                    GET_FORMER_HASHED(currentScene->id).c_str()))
                {
                    OpenScene(currentScene);
                }

                ImGui::PopID();
            }
        }

        // Prefabs

        if (ImGui::CollapsingHeader("Prefabs"))
        {
            if (world->prefabs)
            {
                ImGui::PushID("prefabs");

                DrawObjects(world->prefabs);

                ImGui::PopID();
            }
        }
    }

    ImGui::End();

    // Verify that inspector scenes still exist.

    auto sceneExists = [&](Scene* target)
    {
        return target &&
            (
                target == world->prefabs ||
                std::find(
                    world->scenes.begin(),
                    world->scenes.end(),
                    target
                ) != world->scenes.end()
            );
    };

    // Draw opened scene inspectors.

    for (auto it = openScenes.begin(); it != openScenes.end();)
    {
        Scene* target = *it;

        if (!sceneExists(target) || !DrawSceneInspector(target))
            it = openScenes.erase(it);
        else
            ++it;
    }

    // Draw opened object inspectors.

    for (auto it = openObjects.begin(); it != openObjects.end();)
    {
        if (!sceneExists(it->scene) ||
            !DrawObjectInspector(it->scene, it->object))
        {
            it = openObjects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    rlImGuiEnd();

#else

    (void)scene;

#endif
}