#pragma once

#include <map>
#include <string>
#include "entt/entt.hpp"
#include "eutil.hpp"

namespace lapCore
{
    struct ActionMap
    {
        enum class ControlType
        {
            BUTTON,
            AXIS
        };

        enum class InputType
        {
            KEYBOARD,
            MOUSE,
            GAMEPAD
        };

        struct InputKey
        {
            ControlType controlType;
            InputType inputType;
            std::vector<int> codes;
        };

        struct InputDeadzone
        {
            float lower;
            float upper;
        };

        struct InputEntry
        {
            InputKey key;
            Range<float> deadzone = {0.f, 0.f};
            bool active = true;
            float value;
            bool pressed;
            bool sustain;
            bool shared = false;
            entt::id_type eventID;
        };

        std::map<std::string, InputEntry> actions;

        void RegisterAction(
            const std::string &actionName, 
            const InputEntry &entry
        );
        void RegisterAction(
            const std::string &actionName, 
            entt::id_type eventID, 
            std::vector<int> codes, 
            bool sustain, 
            InputType inputType, 
            ControlType controlType
        );
    };
}