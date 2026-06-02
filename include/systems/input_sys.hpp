#pragma once

#include "system.hpp"
using namespace lapCore;

class InputSystem : public System
{
public:
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
        InputDeadzone deadzone;
        float value;
        bool active;
        bool pressed;
        bool sustain;
        entt::id_type eventID;
    };

    std::map<std::string, InputEntry> actions;

    void RegisterAction(const std::string &actionName, const InputEntry &entry);
    void RegisterAction(const std::string &actionName, entt::id_type eventID, std::vector<int> codes, bool sustain, InputType inputType, ControlType controlType, InputDeadzone deadzone = {0.0f, 0.0f}, bool active = true);

    InputSystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
    void Update(float deltaTime, entt::registry &registry) override;
    std::string GetName() const override { return "InputSystem"; }
};