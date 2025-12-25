#pragma once

#include "entt/entt.hpp"

#include "raylib/raylib_namespace.h"
#include "raylib/rlgl_namespace.h"

#include "portable-file-dialogs.h"

// windows.h defines a DrawText macro (mapping to DrawTextA/DrawTextW) which
// breaks calls to `rl::DrawText` inside the `rl` namespace. Undefine it here
// so raylib's `DrawText` symbol can be used as `rl::DrawText`.
#ifdef DrawText
#undef DrawText
#endif

#include "json.hpp"

#include <unordered_map>
#include <functional>
#include <any>
#include <vector>
#include <string>
#include <iostream>

namespace lapCore
{
    class Scene;

    struct Project
    {
        std::string name;
        std::string version;
        std::string path;
        int main_scene_index;
        Scene *main_scene;
        std::vector<std::unique_ptr<Scene>> scenes;

        rl::Vector2 logicalResolution;
        rl::RenderTexture target;

        Scene *GetMainScene();
        void Clear();

        void LoadSettings(const std::string &settingsFilePath);
    };

    struct Renderable
    {
        rl::Rectangle space;

        unsigned int zlayer;
        bool isScreenSpace;
        bool visible = true;
        rl::Color tint;

        bool usesUIListVisiblity;

        Renderable(unsigned int zlayer, bool isScreenSpace, bool visible, rl::Color tint, bool usesUIListVisiblity)
            : zlayer(zlayer), isScreenSpace(isScreenSpace), visible(visible), tint(tint), usesUIListVisiblity(usesUIListVisiblity) {}
    };

    enum class HorizontalAlignment
    {
        LEFT,
        MIDDLE,
        RIGHT
    };

    enum class VerticalAlignment
    {
        TOP,
        MIDDLE,
        BOTTOM
    };

    struct Alignment
    {
        HorizontalAlignment horizontal;
        VerticalAlignment vertical;

        Alignment(HorizontalAlignment horizontal, VerticalAlignment vertical)
            : horizontal(horizontal), vertical(vertical) {}
    };

    enum class Axis2D
    {
        HORIZONTAL,
        VERTICAL
    };

    struct FrameVector
    {
        rl::Vector2 scale;
        rl::Vector2 offset;

        FrameVector(rl::Vector2 scale, rl::Vector2 offset)
            : scale(scale), offset(offset) {}
    };

    struct Padding
    {
        float top, bottom, left, right;

        Padding(float top, float bottom, float left, float right)
            : top(top), bottom(bottom), left(left), right(right) {}
    };

    struct UIOrigin
    {
        FrameVector position;
        FrameVector size;

        UIOrigin(FrameVector position, FrameVector size)
            : position(position), size(size) {}
    };

    enum class AppState
    {
        DEAD,
        RUNNING,
        ERR
    };

    class App
    {
    public:
        virtual ~App() = default;
        App(Project &project);

        virtual void Init() = 0;
        virtual void Update(float deltaTime) = 0;

        void Run();
        void Shutdown();

        AppState state = AppState::DEAD;
        Project project;
    };

    struct Origin2D
    {
        rl::Vector2 position, scale;
    };

    struct Physics2D
    {
        rl::Vector2 velocity;
        rl::Vector2 gravity;
    };

    struct RotationalData
    {
        rl::Vector2 anchor;
        float rotation;
    };

    // New version of RectVisualizer, the basis of all GUI components (needed for all of them)
    struct Frame
    {
        Renderable renderable;
        UIOrigin origin;

        Frame(Renderable renderable, UIOrigin origin)
            : renderable(renderable), origin(origin) {}
    };

    struct UIList
    {
        FrameVector scrollSize;
        FrameVector displaySize;

        Axis2D direction;

        bool hScrollRight = true;
        bool vScrollBottom = true;

        bool maskOutsideContent = true;

        float scrollOffset = 0.f;
        float scrollSpeed = 20.f; // in pixels

        UIList(FrameVector scrollSize, FrameVector displaySize, bool hScrollRight, bool vScrollBottom, bool maskOutsideContent, float scrollOffset, float scrollSpeed, Axis2D direction)
            : scrollSize(scrollSize), displaySize(displaySize), hScrollRight(hScrollRight), vScrollBottom(vScrollBottom), maskOutsideContent(maskOutsideContent),
              scrollOffset(scrollOffset), scrollSpeed(scrollSpeed), direction(direction) {}
    };

    struct Sprite // Uses Origin2D instead
    {
        Renderable renderable;
        std::string textureName;

        Sprite(Renderable renderable, std::string textureName)
            : renderable(renderable), textureName(textureName) {}
    };

    struct Image
    {
        Sprite sprite;
        UIOrigin origin;

        Image(Sprite sprite, UIOrigin origin)
            : sprite(sprite), origin(origin) {}
    };

    struct TextLabel
    {
        Frame frame;

        std::string text;
        float textSize;
        Alignment textAlignment;
        FrameVector textBounds;
        Padding textPadding;

        TextLabel(Frame frame, std::string text, float textSize, Alignment textAlignment, FrameVector textBounds, Padding textPadding)
            : frame(frame), text(text), textSize(textSize), textAlignment(textAlignment), textBounds(textBounds), textPadding(textPadding) {}
    };

    struct EventBus
    {
        std::unordered_map<std::string, std::string> events;
    };

    struct UIButton
    {
        EventBus events;
        bool active = true;

        UIOrigin bounds;
        bool mouseHovering = false;

        bool usesListVisibility = false;

        UIButton(EventBus buttonEvents, UIOrigin bounds, bool active = true, bool usesListVisibility = false)
            : events(buttonEvents), bounds(bounds), active(active), usesListVisibility(usesListVisibility) {}
    };

    struct Cam2D
    {
        rl::Camera2D camera;
        std::vector<entt::entity> exclude;
    };

    template <typename T>
    struct Attribute
    {
        std::string name;
        T value;

        Attribute(std::string name, T value)
            : name(name), value(value) {}
    };

    struct Script
    {
        std::string onCreateFunction, onUpdateFunction, onDestroyFunction;

        bool active = true;
        bool initiated = false;

        Script(std::string onCreateFunction, std::string onUpdateFunction, std::string onDestroyFunction)
            : onCreateFunction(onCreateFunction), onUpdateFunction(onUpdateFunction), onDestroyFunction(onDestroyFunction) {}
    };

    class FileDialogs
    {
    public:
        static std::string OpenFile(std::vector<std::string> filters);
        static std::string SaveFile(std::vector<std::string> filters);
    };

    std::string ReadFileToString(const std::string &filePath);
    void WriteStringToFile(const std::string &filePath, const std::string &data);

    rl::Vector2 GetMouseInViewportSpace(int logicalWidth, int logicalHeight);

    rl::Rectangle UIOriginToRect(UIOrigin origin, int logicalWidth, int logicalHeight);
    rl::Vector2 FrameVectorToVec2(lapCore::FrameVector vector, int logicalWidth, int logicalHeight);

    using Object = entt::entity;

    class EventRegistry
    {
        // ... (rest of the EventRegistry class implementation is correct) ...
    public:
        inline static std::unordered_map<std::string, std::any> eventCallbacks;

        // --- 1. Declaration and Definition for Connect (Handles all argument counts, including zero) ---
        template <typename... Args, typename Func>
        static void Connect(const std::string &name, Func &&func)
        {
            using Fn = std::function<void(Args...)>;
            auto &anyRef = eventCallbacks[name];
            if (!anyRef.has_value())
            {
                anyRef = std::vector<Fn>{};
            }
            auto *listeners_vec = std::any_cast<std::vector<Fn>>(&anyRef);
            if (!listeners_vec)
            {
                std::cerr << "[EventRegistry] Mismatched event signature for '" << name << "'\n";
                return;
            }
            listeners_vec->emplace_back(std::forward<Func>(func));
        }

        // --- 2. Declaration and Definition of Fire (Handles all argument counts, including zero) ---
        template <typename... Args>
        static void Fire(const std::string &name, Args... args)
        {
            auto it = eventCallbacks.find(name);
            if (it == eventCallbacks.end())
                return;

            auto *listeners_vec = std::any_cast<std::vector<std::function<void(Args...)>>>(&it->second);
            if (!listeners_vec)
            {
                std::cerr << "[EventRegistry] Tried to fire event '" << name << "' with wrong argument types.\n";
                return;
            }

            for (auto &listener : *listeners_vec)
            {
                if (listener)
                    listener(std::forward<Args>(args)...);
            }
        }
    };

    // =======================================================================
    // ECS Helper Functions
    // =======================================================================

    template <typename SystemFunc>
    void ConnectECSEvent(Scene *scene, Object object, const std::string &eventName, SystemFunc &&systemHandler)
    {
        auto wrapper_callback = [scene,
                                 object,
                                 handler = std::forward<SystemFunc>(systemHandler)]()
        {
            handler(scene, object);
        };

        EventRegistry::Connect(eventName, wrapper_callback);
    }

    template <typename EventType, typename SystemFunc>
    void ConnectECSEvent(Scene *scene, Object object, const std::string &eventName, SystemFunc &&systemHandler)
    {
        auto wrapper_callback = [scene,
                                 object,
                                 handler = std::forward<SystemFunc>(systemHandler)](const EventType &eventData)
        {
            handler(eventData, scene, object);
        };
        EventRegistry::Connect<const EventType &>(eventName, wrapper_callback);
    }

    std::string PackProject(Project project);
    Project UnpackProject(const std::string &projJson);

    struct ResourceManager
    {
        std::unordered_map<std::string, rl::Texture2D> textures;
        std::unordered_map<std::string, rl::Shader> shaders;

        rl::Texture2D *AddTexture(const std::string &name, const std::string &filePath);
        void RemoveTexture(const std::string &name);

        rl::Shader *AddShader(const std::string &name, const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
        void RemoveShader(const std::string &name);
    };

    const unsigned int LOGICAL_RESOLUTION_REFERENCE = 800;

    using Object = entt::entity;

    struct AssetLoadRequest
    {
        std::string name;
        std::string type;
        std::string path;
    };

    struct ObjectInfo
    {
        std::string name;
        Object object;
    };

    struct ObjectEntry
    {
        ObjectInfo info;
        ObjectInfo parent;
        std::vector<ObjectInfo> children;
        int childIndex = -1;
    };

    class System
    {
    public:
        System(unsigned int order, Scene *scene, bool isDrawing) : order(order), scene(scene), drawing(isDrawing) {}
        virtual ~System() = default;
        virtual void Update(float deltaTime, entt::registry &reg) = 0;

        unsigned int order;
        bool active = true;
        bool drawing = false;
        Scene *scene;
    };

    struct Scene
    {
        std::string name;

        entt::registry objects;
        std::unordered_map<std::string, ObjectEntry> objectMap;

        entt::registry prefabs;
        std::unordered_map<std::string, ObjectEntry> prefabMap;

        std::vector<std::unique_ptr<System>> systems;

        std::vector<AssetLoadRequest> queuedAssets;
        ResourceManager resources;

        rl::Vector2 logicalWindowPos;
        rl::Vector2 logicalResolution;
        double resolutionScale = 1;

        void QueueAsset(const std::string &name, const std::string &type, const std::string &path);
        void QueueAsset(const AssetLoadRequest &asset);
        void LoadQueuedAssets();

        void Update(float deltaTime, rl::RenderTexture2D &target);

        template <typename SystemType, typename... SystemArgs>
        void AddSystem(unsigned int order, SystemArgs &&...args)
        {
            auto sys = std::make_unique<SystemType>(this, order, std::forward<SystemArgs>(args)...);
            if constexpr (requires(SystemType &t, entt::registry &r) { t.Connect(r); })
                sys->Connect(objects);

            if (order > systems.size())
                systems.resize(order + 1);

            systems[order] = std::move(sys);
        }

        template <typename T>
        T *GetSystem() const
        {
            for (const auto &systemPtr : systems)
            {
                if (T *foundSystem = dynamic_cast<T *>(systemPtr.get()))
                {
                    return foundSystem;
                }
            }

            std::cerr << "Error: System of type " << typeid(T).name() << " not found!" << std::endl;
            return nullptr;
        }

        Object AddObject(const std::string &name, const std::string &parent, int childIndex);
        void RemoveObject(Object object);
        ObjectEntry FindObject(const std::string &name);

        Object AddPrefab(const std::string &name, const std::string &parent, int childIndex);
        ObjectEntry FindPrefab(const std::string &name);

        Object AddObjectFromPrefab(const std::string &prefabName, const std::string &newName);

        std::string GetObjectName(Object object);

        std::vector<ObjectInfo> GetChildren(Object object);
        Object FindChild(Object object, const std::string &name);

        void SetParent(const std::string &name, Object parent);
        ObjectInfo GetParent(const std::string &name);

        template <typename Element, typename... ElementArgs>
        Element AddElement(entt::registry &registry, Object object, ElementArgs &&...args)
        {
            return registry.emplace<Element>(object, std::forward<ElementArgs>(args)...);
        }

        template <typename Element>
        void RemoveElement(entt::registry &registry, Object object)
        {
            registry.remove<Element>(object);
        }

        template <typename Element>
        Element *FindElement(entt::registry &registry, Object object)
        {
            return registry.try_get<Element>(object);
        }

        template <typename Element>
        entt::view<Element> GetElements(entt::registry &registry)
        {
            return registry.view<Element>();
        }

        void Clear();
    };

    struct ScriptRegistry
    {
        inline static std::unordered_map<std::string, std::function<void(Scene *, Object &)>> onCreateFunctions = {
            {"", [](Scene *, Object) {}}};
        inline static std::unordered_map<std::string, std::function<void(Scene *, Object &, float)>> onUpdateFunctions = {
            {"", [](Scene *, Object, float) {}}};
        inline static std::unordered_map<std::string, std::function<void(Scene *, Object &)>> onDestroyFunctions = {
            {"", [](Scene *, Object) {}}};
    };

    using Object = entt::entity;

    class PhysicsSystem : public System
    {
    public:
        PhysicsSystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
        void Update(float deltaTime, entt::registry &reg) override;
    };

    class RenderSystem : public System
    {
    public:
        struct RenderEntry
        {
            Object entity;
            unsigned int zlayer;
            bool isScreenSpace;
            enum class Type
            {
                Sprite,
                Text,
                Rect,
                RoundedRect,
                Circle,
                Image
            } type;
        };

        std::vector<RenderEntry> renderList;

        RenderSystem(Scene *scene, unsigned int order) : System(order, scene, true) {}
        void Update(float deltaTime, entt::registry &reg) override;

        void Connect(entt::registry &registry);
        void OnRenderableUpdated(entt::registry &registry, Object entity);

        void RebuildRenderList(entt::registry &registry);

        bool needsResort = true;
    };

    class ScriptSystem : public System
    {
    public:
        ScriptSystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
        void Update(float deltaTime, entt::registry &registry) override;
        void OnDestroy(entt::registry &registry);
    };

    class GUISystem : public System
    {
    public:
        GUISystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
        void Update(float deltaTime, entt::registry &registry) override;
    };
}