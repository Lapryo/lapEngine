#pragma once

#include <entt/entt.hpp>

#include <iostream>
#include <type_traits>

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
        bool cachedActive = true;
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

        using UniqueEventContainer = unique_ptr<IEventContainer>;
        using EventMap = unordered_map<id_type, UniqueEventContainer>;
        using ObjectMap = unordered_map<Object, UniqueEventContainer>;

        static EventMap* GetEventMap(Scene* scene)
        {
            auto sceneIt = eventCallbacks.find(scene);
            if (sceneIt == eventCallbacks.end()) return nullptr;
            return &sceneIt->second;
        }

        static ObjectMap* GetObjectMap(Scene* scene)
        {
            auto sceneIt = objEventCallbacks.find(scene);
            if (sceneIt == objEventCallbacks.end()) return nullptr;
            return &sceneIt->second;
        }

        // ---------------------------------------------------------------------
        // Global Events
        // ---------------------------------------------------------------------

        template <typename... Args, typename Func>
        static void Connect(Scene* owner, id_type eventID, Func&& func)
        {
            using Container = EventContainer<decay_t<Args>...>;

            auto& ptr = eventCallbacks[owner][eventID];

            if (!ptr)
                ptr = make_unique<Container>();

            if (ptr->type != type_hash<Container>::value())
            {
                cerr << "[EventRegistry] Mismatched event signature for '"
                          << eventID << "'.\n";
                return;
            }

            auto* container = static_cast<Container*>(ptr.get());
            container->listeners.emplace_back(std::forward<Func>(func));
        }

        template <typename... Args>
        static void Fire(Scene* scene, id_type eventID, Args&&... args)
        {
            auto eventMap = GetEventMap(scene);
            if (!eventMap) return;

            auto it = eventMap->find(eventID);
            if (it == eventMap->end()) return;
            if (!it->second->active) return;

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
        static void Connect(Scene* scene, Object object, Func&& func)
        {
            using Container = EventContainer<decay_t<Args>...>;

            auto& ptr = objEventCallbacks[scene][object];

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
            auto objectMap = GetObjectMap(scene);
            if (!objectMap) return;

            auto it = objectMap->find(object);
            if (it == objectMap->end()) return;
            if (!it->second->active) return;

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

        template <typename... Args, typename Func>
        static void ConnectGlobal(id_type eventID, Func&& func)
        {
            using Container = EventContainer<decay_t<Args>...>;

            auto& ptr = globalEventCallbacks[eventID];

            if (!ptr)
                ptr = make_unique<Container>();

            if (ptr->type != type_hash<Container>::value())
            {
                cerr << "[EventRegistry] Mismatched global event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(ptr.get());
            container->listeners.emplace_back(std::forward<Func>(func));
        }

        template <typename... Args>
        static void FireGlobal(id_type eventID, Args&&... args)
        {
            auto it = globalEventCallbacks.find(eventID);

            if (it == globalEventCallbacks.end())
                return;

            if (!it->second->active)
                return;

            using Container = EventContainer<decay_t<Args>...>;

            if (it->second->type != type_hash<Container>::value())
            {
                cerr << "[EventRegistry] Mismatched global event signature.\n";
                return;
            }

            auto* container = static_cast<Container*>(it->second.get());

            for (auto& listener : container->listeners)
            {
                if (listener)
                    listener(nullptr, args...);
            }
        }

        // ---------------------------------------------------------------------
        // Global Events With Source Object
        // ---------------------------------------------------------------------

        template <typename... Args>
        static void Fire(Scene* scene, id_type eventID, Object source, Args&&... args)
        {
            auto eventMap = GetEventMap(scene);
            if (!eventMap) return;

            auto it = eventMap->find(eventID);
            if (it == eventMap->end()) return;
            if (!it->second->active) return;

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
                    listener(scene, source, std::forward<Args>(args)...);
            }
        }

        // ---------------------------------------------------------------------
        // Disconnect / SetActive
        // ---------------------------------------------------------------------
        static void Disconnect(Scene* scene, id_type eventID)
        {
            auto eventMap = GetEventMap(scene);
            if (!eventMap) return;

            eventMap->erase(eventID);
        }

        static void SetActive(Scene* scene, id_type eventID, bool isActive)
        {
            auto eventMap = GetEventMap(scene);
            if (!eventMap) return;

            auto it = eventMap->find(eventID);
            if (it == eventMap->end()) return;

            it->second->active = isActive;
        }

        static void Disconnect(Scene* scene, Object object)
        {
            auto objMap = GetObjectMap(scene);
            if (!objMap) return;

            objMap->erase(object);
        }

        static void SetActive(Scene* scene, Object object, bool isActive)
        {
            auto objectMap = GetObjectMap(scene);
            if (!objectMap) return;

            auto it = objectMap->find(object);
            if (it == objectMap->end()) return;

            it->second->active = isActive;
        }

        static void Disconnect(Scene* scene)
        {
            eventCallbacks.erase(scene);
            objEventCallbacks.erase(scene);
        }

        static void SetActive(Scene* s, bool isActive, bool useCache)
        {
            if (useCache)
            {
                if (isActive) // We are restoring the scene so assign to the eventContainer's own cachedActive
                {
                    for (auto& [scene, eventMap] : eventCallbacks)
                    {
                        if (scene == s)
                            for (auto& [eventID, eventContainer] : eventMap)
                            {
                                eventContainer->active = eventContainer->cachedActive;
                            }
                    }

                    for (auto& [scene, objectMap] : objEventCallbacks)
                    {
                        if (scene == s)
                            for (auto& [object, eventContainer] : objectMap)
                            {
                                eventContainer->active = eventContainer->cachedActive;
                            }
                    }
                }
                else // Store the current active state into the cache and set it to false after
                {
                    for (auto& [scene, eventMap] : eventCallbacks)
                    {
                        if (scene == s)
                            for (auto& [eventID, eventContainer] : eventMap)
                            {
                                eventContainer->cachedActive = eventContainer->active;
                                eventContainer->active = false;
                            }
                    }

                    for (auto& [scene, objectMap] : objEventCallbacks)
                    {
                        if (scene == s)
                            for (auto& [object, eventContainer] : objectMap)
                            {
                                eventContainer->cachedActive = eventContainer->active;
                                eventContainer->active = false;
                            }
                    }
                }
            }
            else
            {
                for (auto& [scene, eventMap] : eventCallbacks)
                {
                    if (scene == s)
                        for (auto& [eventID, eventContainer] : eventMap)
                        {
                            eventContainer->active = isActive;
                        }
                }

                for (auto& [scene, objectMap] : objEventCallbacks)
                {
                    if (scene == s)
                        for (auto& [object, eventContainer] : objectMap)
                        {
                            eventContainer->active = isActive;
                        }
                }
            }
        }

        static void DisconnectGlobal(id_type eventID)
        {
            globalEventCallbacks.erase(eventID);
        }

        static void SetActiveGlobal(id_type eventID, bool isActive)
        {
            auto it = globalEventCallbacks.find(eventID);
            if (it == globalEventCallbacks.end()) return;

            it->second->active = isActive;
        }

        inline static unordered_map<Scene*, EventMap> eventCallbacks;
        inline static unordered_map<Scene*, ObjectMap> objEventCallbacks;
        inline static unordered_map<id_type, UniqueEventContainer> globalEventCallbacks;
    };

    // =========================================================================
    // ECS Event Helpers
    // =========================================================================

    // Fixed ECS object.
    // Fixed ECS object.
    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEvent(
        Scene* scene,
        Object object,
        id_type eventID,
        SystemFunc&& systemHandler
    )
    {
        auto callback = [object, eventID,
            handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, EventArgs... args)
        {
            handler(scene, object, eventID, std::forward<EventArgs>(args)...);
        };

        EventRegistry::Connect<EventArgs...>(
            scene,
            eventID,
            std::move(callback)
        );
    }

    // Object is provided when the event is fired.
    template <typename... EventArgs, typename SystemFunc>
    void ConnectECSEventWithSource(
        Scene* scene,
        id_type eventID,
        SystemFunc&& systemHandler
    )
    {
        auto callback = [eventID,
            handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, Object object, EventArgs... args)
        {
            handler(scene, object, eventID, std::forward<EventArgs>(args)...);
        };

        EventRegistry::Connect<Object, EventArgs...>(
            scene,
            eventID,
            std::move(callback)
        );
    }

    // Application-wide event with a source object.
    template <typename... EventArgs, typename SystemFunc>
    void ConnectGlobalECSEvent(
        id_type eventID,
        SystemFunc&& systemHandler
    )
    {
        auto callback = [eventID,
            handler = std::forward<SystemFunc>(systemHandler)]
            (Scene* scene, Object object, EventArgs... args)
        {
            handler(scene, object, eventID, std::forward<EventArgs>(args)...);
        };

        EventRegistry::ConnectGlobal<Object, EventArgs...>(
            eventID,
            std::move(callback)
        );
    }

    // Maybe a global ConnectECSEvent here?
}