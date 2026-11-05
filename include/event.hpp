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

    using Object = entity;
    using namespace entt;
    using namespace std;

    // =========================================================================
    // Event Containers
    // =========================================================================

    struct IEventContainer
    {
        virtual ~IEventContainer() = default;

        id_type type;
        bool active = true;
    };

    template <typename... Args>
    struct EventContainer : IEventContainer
    {
        using Listener = function<void(Scene*, Args...)>;

        EventContainer()
        {
            type = type_hash<EventContainer<Args...>>::value();
        }

        vector<Listener> listeners;
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
        static void Connect(hashed_string name, Func&& func)
        {
            using Container = EventContainer<decay_t<Args>...>;

            const id_type eventID = name.value();
            auto& ptr = eventCallbacks[eventID];

            reverseLookup.try_emplace(eventID, name.data());

            if (!ptr)
                ptr = make_unique<Container>();

            if (ptr->type != type_hash<Container>::value())
            {
                cerr << "[EventRegistry] Mismatched event signature for '"
                          << name.data() << "'.\n";
                return;
            }

            auto* container = static_cast<Container*>(ptr.get());
            container->listeners.emplace_back(std::forward<Func>(func));
        }

        template <typename... Args>
        static void Fire(Scene* scene, id_type eventID, Args&&... args)
        {
            auto it = eventCallbacks.find(eventID);

            if (it == eventCallbacks.end())
                return;

            if (!it->second->active)
                return;

            using Container = EventContainer<decay_t<Args>...>;

            if (it->second->type != type_hash<Container>::value())
            {
                cerr << "[EventRegistry] Mismatched event signature.\n";
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
            using Container = EventContainer<decay_t<Args>...>;

            auto& ptr = objEventCallbacks[object];

            if (!ptr)
                ptr = make_unique<Container>();

            if (ptr->type != type_hash<Container>::value())
            {
                cerr << "[EventRegistry] Mismatched object event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(ptr.get());
            container->listeners.emplace_back(forward<Func>(func));
        }

        template <typename... Args>
        static void Fire(Scene* scene, Object object, Args&&... args)
        {
            auto it = objEventCallbacks.find(object);

            if (it == objEventCallbacks.end())
                return;

            if (!it->second->active)
                return;

            using Container = EventContainer<decay_t<Args>...>;

            if (it->second->type != type_hash<Container>::value())
            {
                cerr << "[EventRegistry] Mismatched object event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(it->second.get());

            for (auto& listener : container->listeners)
            {
                if (listener)
                    listener(scene, forward<Args>(args)...);
            }
        }

        // ---------------------------------------------------------------------
        // Global Events With Source Object
        // ---------------------------------------------------------------------

        template <typename... Args>
        static void Fire(Scene* scene, id_type eventID, Object source, Args&&... args)
        {
            auto it = eventCallbacks.find(eventID);

            if (it == eventCallbacks.end())
                return;

            if (!it->second->active)
                return;

            using Container = EventContainer<Object, decay_t<Args>...>;

            if (it->second->type != type_hash<Container>::value())
            {
                cerr << "[EventRegistry] Mismatched event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(it->second.get());

            for (auto& listener : container->listeners)
            {
                if (listener)
                    listener(scene, source, forward<Args>(args)...);
            }
        }

        // ---------------------------------------------------------------------
        // Disconnect
        // ---------------------------------------------------------------------

        static void Disconnect(id_type eventID)
        {
            eventCallbacks.erase(eventID);
            reverseLookup.erase(eventID);
        }

        static void Disconnect(Object object)
        {
            objEventCallbacks.erase(object);
        }
        
        inline static unordered_map<id_type, unique_ptr<IEventContainer>> eventCallbacks;
        inline static unordered_map<Object, unique_ptr<IEventContainer>> objEventCallbacks;
        inline static unordered_map<id_type, string> reverseLookup;
    };

    // =========================================================================
    // ECS Event Helpers
    // =========================================================================

    // Fixed ECS object.
    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEvent(Object object, hashed_string name, SystemFunc&& systemHandler)
    {
        const id_type eventID = name.value();

        auto callback = [object, eventID, handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, EventArgs... args)
        {
            handler(scene, object, eventID, std::forward<EventArgs>(args)...);
        };

        EventRegistry::Connect<EventArgs...>(name, std::move(callback));
    }

    // Object is provided when the event is fired.
    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEventWithSource(hashed_string name, SystemFunc&& systemHandler)
    {
        const id_type eventID = name.value();

        auto callback = [eventID, handler = forward<SystemFunc>(systemHandler)]
            (Scene* scene, Object object, EventArgs... args)
        {
            handler(scene, object, eventID, forward<EventArgs>(args)...);
        };

        EventRegistry::Connect<Object, EventArgs...>(name, move(callback));
    }
}