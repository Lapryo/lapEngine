#ifndef EVENT_HPP
#define EVENT_HPP

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <any>
#include <iostream>

namespace lapCore
{
    class EventRegistry
    {
    public:
        // Store all event listeners — type-erased using std::any
        inline static std::unordered_map<std::string, std::any> eventCallbacks;

        // Overload to allow lambdas directly without manually writing std::function<>
        template<typename... Args, typename Func>
        static void Connect(const std::string& name, Func&& func)
        {
            using Fn = std::function<void(Args...)>;

            auto& anyRef = eventCallbacks[name];
            if (!anyRef.has_value())
                anyRef = std::vector<Fn>{};

            auto* listeners = std::any_cast<std::vector<Fn>>(&anyRef);
            if (!listeners)
            {
                std::cerr << "[EventRegistry] Mismatched event signature for '" << name << "'\n";
                return;
            }

            listeners->emplace_back(std::forward<Func>(func));
        }

        // Fire all listeners for a given event name with arguments
        template<typename... Args>
        static void Fire(const std::string& name, Args... args)
        {
            auto it = eventCallbacks.find(name);
            if (it == eventCallbacks.end())
                return;

            auto* listeners = std::any_cast<std::vector<std::function<void(Args...)>>>(&it->second);
            if (!listeners)
            {
                std::cerr << "[EventRegistry] Tried to fire event '" << name << "' with wrong argument types.\n";
                return;
            }

            for (auto& listener : *listeners)
                if (listener)
                    listener(std::forward<Args>(args)...);
        }
    };
}

#endif