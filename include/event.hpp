#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <any>
#include <iostream>
#include <entt/entt.hpp>

namespace lapCore
{
    struct Scene;

    struct IEventContainer
    {
        virtual ~IEventContainer() = default;
        entt::id_type type;
    };
    template <typename... Args>
    struct EventContainer : IEventContainer
    {
        EventContainer()
        {
            type = entt::type_hash<EventContainer<Args...>>::value();
        }

        std::vector<std::function<void(Scene*, Args...)>> listeners;
    };

    class EventRegistry
    {
        // ... (rest of the EventRegistry class implementation is correct) ...
    public:
        inline static std::unordered_map<entt::id_type, std::unique_ptr<IEventContainer>> eventCallbacks;
        inline static std::unordered_map<Object, std::unique_ptr<IEventContainer>> objEventCallbacks;

        inline static std::unordered_map<entt::id_type, std::string> reverseLookup;

        // --- 1. Declaration and Definition for Connect (Handles all argument counts, including zero) ---
        template <typename... Args, typename Func>
        static void Connect(entt::hashed_string name, Func &&func)
        {
            using container = EventContainer<std::decay_t<Args>...>;

            reverseLookup.try_emplace(name.value(), name.data());
            auto &ptr = eventCallbacks[name.value()];

            if (!ptr)
                ptr = std::make_unique<container>();
            if (ptr->type != entt::type_hash<container>::value())
            {
                std::cerr << "[EventRegistery] Pointer types don't match\n";
                return;
            }

            auto* c = static_cast<container*>(ptr.get());
            if (!c)
            {
                std::cerr << "[EventRegistry] Mismatched event signature for '" << name.data() << "'.\n";
                return;
            }

            c->listeners.emplace_back(std::forward<Func>(func));
        }

        template <typename... Args, typename Func>
        static void Connect(Object object, Func &&func)
        {
            using container = EventContainer<std::decay_t<Args>...>;
            auto &ptr = objEventCallbacks[object];

            if (!ptr)
                ptr = std::make_unique<container>();
            if (ptr->type != entt::type_hash<container>::value())
            {
                std::cerr << "[EventRegistery] Pointer types don't match\n";
                return;
            }

            auto* c = static_cast<container*>(ptr.get());
            if (!c)
            {
                return;
            }

            c->listeners.emplace_back(std::forward<Func>(func));
        }

        // --- 2. Declaration and Definition of Fire (Handles all argument counts, including zero) ---
        template <typename... Args>
        static void Fire(Scene* scene, entt::id_type id, Args&&... args)
        {
            auto it = eventCallbacks.find(id);
            if (it == eventCallbacks.end())
                return;

            using container = EventContainer<std::decay_t<Args>...>;
            auto *ptr = it->second.get();

            if (ptr->type != entt::type_hash<container>::value())
            {
                std::cerr << "[EventRegistery] Pointer types don't match\n";
                return;
            }

            auto *c = static_cast<container*>(ptr);
            for (auto &listener : c->listeners)
                if (listener)
                    listener(scene, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void Fire(Scene* scene, Object obj, Args&&... args)
        {
            auto it = objEventCallbacks.find(obj);
            if (it == objEventCallbacks.end())
                return;

            using container = EventContainer<std::decay_t<Args>...>;
            auto *ptr = it->second.get();

            if (ptr->type != entt::type_hash<container>::value())
            {
                std::cerr << "[EventRegistery] Pointer types don't match\n";
                return;
            }

            auto *c = static_cast<container*>(ptr);
            for (auto &listener : c->listeners)
                if (listener)
                    listener(scene, std::forward<Args>(args)...);
        }

        static void Disconnect(entt::id_type eventID)
        {
            eventCallbacks.erase(eventID);
            reverseLookup.erase(eventID);
        }

        static void Disconnect(Object obj)
        {
            objEventCallbacks.erase(obj);
        }
    };

    // =======================================================================
    // ECS Helper Functions
    // =======================================================================

    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEvent(
        entt::id_type objectID,
        entt::hashed_string name,
        SystemFunc&& systemHandler)
    {
        auto id = name.value();

        auto wrapper_callback =
            [objectID,
            id,
            handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, EventArgs... args)
        {
            handler(
                scene,
                objectID,
                id,
                std::forward<decltype(args)>(args)...
            );
        };

        EventRegistry::Connect<EventArgs...>(name, wrapper_callback);
    }

    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEvent(
        Object object,
        entt::hashed_string name,
        SystemFunc&& systemHandler)
    {
        auto id = name.value();

        auto wrapper_callback =
            [object,
            id,
            handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, EventArgs... args)
        {
            handler(
                scene,
                object,
                id,
                std::forward<decltype(args)>(args)...
            );
        };

        EventRegistry::Connect<EventArgs...>(name, wrapper_callback);
    }
}