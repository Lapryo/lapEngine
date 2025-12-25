#include "core.hpp"

using namespace lapCore;

void HandleKeyboardInput(InputSystem::InputEntry &entry, int key)
{
    if (rl::IsKeyDown(key))
    {
        if (!entry.active)
        {
            entry.pressed = true;
            EventRegistry::Fire<>(entry.event);
        }
    }
    else
        entry.pressed = false;
}

void HandleMouseInput(InputSystem::InputEntry &entry, int button)
{
    if (rl::IsMouseButtonDown(button))
    {
        if (!entry.active)
        {
            entry.pressed = true;
            EventRegistry::Fire<>(entry.event);
        }
    }
    else
        entry.pressed = false;
}

void HandleGamepadInput(InputSystem::InputEntry &entry, InputSystem::ControlType controlType, int button)
{
    if (controlType == InputSystem::ControlType::AXIS)
    {
        if (rl::GetGamepadAxisMovement(1, button) > entry.deadzone)
        {
            if (!entry.active)
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
            if (!entry.active)
            {
                entry.pressed = true;
                EventRegistry::Fire<>(entry.event);
            }
        }
        else
            entry.pressed = false;
    }
}

void InputSystem::Update(float deltaTime, entt::registry &registry)
{
    for (auto &inputPair : inputs)
    {
        switch (inputPair.first.inputType)
        {
            case InputType::KEYBOARD:
            {
                HandleKeyboardInput(inputPair.second, inputPair.first.code);
                break;
            }
            case InputType::MOUSE:
            {
                HandleMouseInput(inputPair.second, inputPair.first.code);
                break;
            }
            case InputType::GAMEPAD:
            {
                HandleGamepadInput(inputPair.second, inputPair.first.controlType, inputPair.first.code);
                break;
            }
        }
    }
}