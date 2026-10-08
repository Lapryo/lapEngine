#pragma once

#include <entt/entt.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace lapCore
{
    struct Scene;

    using Object = entt::entity;

    // =========================================================================
    // Event Containers
    // =========================================================================

    struct IEventContainer
    {
        virtual ~IEventContainer() = default;

        entt::id_type type;
    };

    template <typename... Args>
    struct EventContainer : IEventContainer
    {
        using Listener = std::function<void(Scene*, Args...)>;

        EventContainer()
        {
            type = entt::type_hash<EventContainer<Args...>>::value();
        }

        std::vector<Listener> listeners;
    };

    // =========================================================================
    // Event Registry
    // =========================================================================

    class EventRegistry
    {
    public:
        // ---------------------------------------------------------------------
        // Global Events
        // ---------------------------------------------------------------------

        template <typename... Args, typename Func>
        static void Connect(entt::hashed_string name, Func&& func)
        {
            using Container = EventContainer<std::decay_t<Args>...>;

            const entt::id_type eventID = name.value();
            auto& ptr = eventCallbacks[eventID];

            reverseLookup.try_emplace(eventID, name.data());

            if (!ptr)
                ptr = std::make_unique<Container>();

            if (ptr->type != entt::type_hash<Container>::value())
            {
                std::cerr << "[EventRegistry] Mismatched event signature for '"
                          << name.data() << "'.\n";
                return;
            }

            auto* container = static_cast<Container*>(ptr.get());
            container->listeners.emplace_back(std::forward<Func>(func));
        }

        template <typename... Args>
        static void Fire(Scene* scene, entt::id_type eventID, Args&&... args)
        {
            auto it = eventCallbacks.find(eventID);

            if (it == eventCallbacks.end())
                return;

            using Container = EventContainer<std::decay_t<Args>...>;

            if (it->second->type != entt::type_hash<Container>::value())
            {
                std::cerr << "[EventRegistry] Mismatched event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(it->second.get());

            for (auto& listener : container->listeners)
            {
                if (listener)
                    listener(scene, std::forward<Args>(args)...);
            }
        }

        // ---------------------------------------------------------------------
        // Object Events
        // ---------------------------------------------------------------------

        template <typename... Args, typename Func>
        static void Connect(Object object, Func&& func)
        {
            using Container = EventContainer<std::decay_t<Args>...>;

            auto& ptr = objEventCallbacks[object];

            if (!ptr)
                ptr = std::make_unique<Container>();

            if (ptr->type != entt::type_hash<Container>::value())
            {
                std::cerr << "[EventRegistry] Mismatched object event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(ptr.get());
            container->listeners.emplace_back(std::forward<Func>(func));
        }

        template <typename... Args>
        static void Fire(Scene* scene, Object object, Args&&... args)
        {
            auto it = objEventCallbacks.find(object);

            if (it == objEventCallbacks.end())
                return;

            using Container = EventContainer<std::decay_t<Args>...>;

            if (it->second->type != entt::type_hash<Container>::value())
            {
                std::cerr << "[EventRegistry] Mismatched object event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(it->second.get());

            for (auto& listener : container->listeners)
            {
                if (listener)
                    listener(scene, std::forward<Args>(args)...);
            }
        }

        // ---------------------------------------------------------------------
        // Global Events With Source Object
        // ---------------------------------------------------------------------

        template <typename... Args>
        static void Fire(Scene* scene, entt::id_type eventID, Object source, Args&&... args)
        {
            auto it = eventCallbacks.find(eventID);

            if (it == eventCallbacks.end())
                return;

            using Container = EventContainer<Object, std::decay_t<Args>...>;

            if (it->second->type != entt::type_hash<Container>::value())
            {
                std::cerr << "[EventRegistry] Mismatched event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(it->second.get());

            for (auto& listener : container->listeners)
            {
                if (listener)
                    listener(scene, source, std::forward<Args>(args)...);
            }
        }

        // ---------------------------------------------------------------------
        // Disconnect
        // ---------------------------------------------------------------------

        static void Disconnect(entt::id_type eventID)
        {
            eventCallbacks.erase(eventID);
            reverseLookup.erase(eventID);
        }

        static void Disconnect(Object object)
        {
            objEventCallbacks.erase(object);
        }
        
        inline static std::unordered_map<entt::id_type, std::unique_ptr<IEventContainer>> eventCallbacks;
        inline static std::unordered_map<Object, std::unique_ptr<IEventContainer>> objEventCallbacks;
        inline static std::unordered_map<entt::id_type, std::string> reverseLookup;
    };

    // =========================================================================
    // ECS Event Helpers
    // =========================================================================

    // Fixed object ID.
    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEvent(entt::id_type objectID, entt::hashed_string name, SystemFunc&& systemHandler)
    {
        const entt::id_type eventID = name.value();

        auto callback = [objectID, eventID, handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, EventArgs... args)
        {
            handler(scene, objectID, eventID, std::forward<EventArgs>(args)...);
        };

        EventRegistry::Connect<EventArgs...>(name, std::move(callback));
    }

    // Fixed ECS object.
    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEvent(Object object, entt::hashed_string name, SystemFunc&& systemHandler)
    {
        const entt::id_type eventID = name.value();

        auto callback = [object, eventID, handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, EventArgs... args)
        {
            handler(scene, object, eventID, std::forward<EventArgs>(args)...);
        };

        EventRegistry::Connect<EventArgs...>(name, std::move(callback));
    }

    // Object is provided when the event is fired.
    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEventWithSource(entt::hashed_string name, SystemFunc&& systemHandler)
    {
        const entt::id_type eventID = name.value();

        auto callback = [eventID, handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, Object object, EventArgs... args)
        {
            handler(scene, object, eventID, std::forward<EventArgs>(args)...);
        };

        EventRegistry::Connect<Object, EventArgs...>(name, std::move(callback));
    }
}