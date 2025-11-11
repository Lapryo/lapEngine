#ifndef EVENT_HPP
#define EVENT_HPP

#include <unordered_map>
#include <string>

namespace lapCore
{
    struct Event
    {
        bool active = false;
        bool enabled = true;
    };

    struct EventRegistry
    {
        inline static std::unordered_map<std::string, Event> events;
    };

    void AddEvent(const std::string &name);
}

#endif