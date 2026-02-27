#ifndef INPUT_SYS_HPP
#define INPUT_SYS_HPP

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

    struct InputEntry
    {
        float deadzone;
        bool active;
        bool pressed;
        std::string event;
    };

    std::map<InputKey, InputEntry> inputs;

    InputSystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
    void Update(float deltaTime, entt::registry &registry) override;
};

#endif