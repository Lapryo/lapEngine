#pragma once

#include <map>
#include <string>
#include "entt/entt.hpp"
#include "eutil.hpp"

namespace lapCore
{
    using namespace EUTIL::Math;

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
            vector<int> codes;
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
            id_type eventID;
        };

        map<string, InputEntry> actions;

        void RegisterAction(
            const string &actionName, 
            const InputEntry &entry
        );
        void RegisterAction(
            const string &actionName, 
            id_type eventID, 
            vector<int> codes, 
            bool sustain, 
            InputType inputType, 
            ControlType controlType
        );
    };
}