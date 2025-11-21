#ifndef EVENT_HPP
#define EVENT_HPP

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <any>
#include <iostream>
#include <entt/entt.hpp>

namespace lapCore
{
    // --- FORWARD DECLARATION ---
    // This tells the compiler that 'Scene' is a class defined elsewhere.
    class Scene; 

    using Object = entt::entity;

    class EventRegistry
    {
    // ... (rest of the EventRegistry class implementation is correct) ...
    public:
        inline static std::unordered_map<std::string, std::any> eventCallbacks;

        // --- 1. Declaration and Definition for Connect (Handles all argument counts, including zero) ---
        template<typename... Args, typename Func>
        static void Connect(const std::string& name, Func&& func)
        {
            using Fn = std::function<void(Args...)>; 
            auto& anyRef = eventCallbacks[name];
            if (!anyRef.has_value()) {
                anyRef = std::vector<Fn>{};
            }
            auto* listeners_vec = std::any_cast<std::vector<Fn>>(&anyRef);
            if (!listeners_vec) {
                std::cerr << "[EventRegistry] Mismatched event signature for '" << name << "'\n";
                return;
            }
            listeners_vec->emplace_back(std::forward<Func>(func));
        }

        // --- 2. Declaration and Definition of Fire (Handles all argument counts, including zero) ---
        template<typename... Args>
        static void Fire(const std::string& name, Args... args)
        {
            auto it = eventCallbacks.find(name);
            if (it == eventCallbacks.end()) return;

            auto* listeners_vec = std::any_cast<std::vector<std::function<void(Args...)>>>(&it->second);
            if (!listeners_vec) {
                std::cerr << "[EventRegistry] Tried to fire event '" << name << "' with wrong argument types.\n";
                return;
            }

            for (auto& listener : *listeners_vec) {
                if (listener) listener(std::forward<Args>(args)...);
            }
        }
    };

    // =======================================================================
    // ECS Helper Functions
    // =======================================================================
    
    template<typename SystemFunc>
    void ConnectECSEvent(Scene* scene, Object &object, const std::string& eventName, SystemFunc&& systemHandler)
    {
        auto wrapper_callback = [
            scene, 
            &object,
            handler = std::forward<SystemFunc>(systemHandler)
        ]() { 
            handler(scene, object);
        };
        
        EventRegistry::Connect(eventName, wrapper_callback); 
    }
    
    template<typename EventType, typename SystemFunc>
    void ConnectECSEvent(Scene* scene, Object &object, const std::string& eventName, SystemFunc&& systemHandler)
    {
        auto wrapper_callback = [
            scene, 
            &object, 
            handler = std::forward<SystemFunc>(systemHandler)
        ](const EventType& eventData) {
            handler(eventData, scene, object);
        };
        EventRegistry::Connect<const EventType&>(eventName, wrapper_callback);
    }
}

#endif