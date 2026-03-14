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
        InputKey key;
        float deadzone;
        bool active;
        bool pressed;
        std::string event;
    };

    std::map<std::string, InputEntry> actions;

    void RegisterAction(const std::string &actionName, const InputEntry &entry);
    void RegisterAction(const std::string &actionName, const std::string &event, std::vector<int> codes, InputType inputType, ControlType controlType, float deadzone = 0.0f, bool active = true);

    InputSystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
    void Update(float deltaTime, entt::registry &registry) override;
};

#endif