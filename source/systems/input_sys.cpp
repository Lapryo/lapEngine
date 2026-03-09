#include "core.hpp"
#include "systems/input_sys.hpp"

using namespace lapCore;

// TODO: FIX THIS TO MAKE MULTIPLE KEYS FOR ONE ACTION WORK

bool HandleKeyboardInput(InputSystem::InputEntry &entry, int key)
{
    if (rl::IsKeyDown(key))
    {
        if (entry.active)
        {
            entry.pressed = true;
            EventRegistry::Fire<>(entry.event);
        }
    }
    else
        entry.pressed = false;

    return entry.pressed;
}

bool HandleMouseInput(InputSystem::InputEntry &entry, int button)
{
    if (rl::IsMouseButtonDown(button))
    {
        if (entry.active)
        {
            entry.pressed = true;
            EventRegistry::Fire<>(entry.event);
        }
    }
    else
        entry.pressed = false;

    return entry.pressed;
}

bool HandleGamepadInput(InputSystem::InputEntry &entry, InputSystem::ControlType controlType, int button)
{
    if (controlType == InputSystem::ControlType::AXIS)
    {
        if (rl::GetGamepadAxisMovement(1, button) > entry.deadzone)
        {
            if (entry.active)
            {
                entry.pressed = true;
                EventRegistry::Fire<>(entry.event);
            }
        }
        else
            entry.pressed = false;
    }
    else
    {
        if (rl::IsGamepadButtonDown(1, button))
        {
            if (entry.active)
            {
                entry.pressed = true;
                EventRegistry::Fire<>(entry.event);
            }
        }
        else
            entry.pressed = false;
    }

    return entry.pressed;
}

void InputSystem::Update(float deltaTime, entt::registry &registry)
{
    for (auto &inputPair : actions)
    {
        switch (inputPair.second.key.inputType)
        {
            case InputType::KEYBOARD:
            {
                for (int key : inputPair.second.key.codes)
                    if (HandleKeyboardInput(inputPair.second, key)) break;
                break;
            }
            case InputType::MOUSE:
            {
                for (int key : inputPair.second.key.codes)
                     if (HandleMouseInput(inputPair.second, key)) break;
                break;
            }
            case InputType::GAMEPAD:
            {
                for (int key : inputPair.second.key.codes)
                     if (HandleGamepadInput(inputPair.second, inputPair.second.key.controlType, key)) break;
                break;
            }
        }
    }
}

void InputSystem::RegisterAction(const std::string &actionName, const InputEntry &entry)
{
    actions[actionName] = entry;
}

void InputSystem::RegisterAction(const std::string &actionName, const std::string &event, std::vector<int> codes, InputType inputType, ControlType controlType, float deadzone, bool active)
{
    InputEntry entry;
    entry.key.inputType = inputType;
    entry.key.controlType = controlType;
    entry.key.codes = codes;
    entry.active = active;
    entry.event = event;
    entry.deadzone = deadzone;

    RegisterAction(actionName, entry);
}