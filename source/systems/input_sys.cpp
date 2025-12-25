#include "core.hpp"

using namespace lapCore;

void InputSystem::Update(float deltaTime, entt::registry &registry)
{
    for (auto &inputPair : inputs)
    {
        switch (inputPair.first.inputType)
        {
            case InputType::KEYBOARD:
            {
                if (rl::IsKeyDown(inputPair.first.code))
                {
                    if (!inputPair.second.active)
                    {
                        inputPair.second.pressed = true;
                        EventRegistry::Fire<>(inputPair.second.event);
                    }
                }
                else
                    inputPair.second.pressed = false;
                break;
            }
            case InputType::MOUSE:
            {
                if (rl::IsMouseButtonDown(inputPair.first.code))
                {
                    if (!inputPair.second.active)
                    {
                        inputPair.second.pressed = true;
                        EventRegistry::Fire<>(inputPair.second.event);
                    }
                }
                else
                    inputPair.second.pressed = false;
                break;
            }
            case InputType::GAMEPAD:
            {
                if (inputPair.first.controlType == ControlType::AXIS)
                {
                    if (rl::GetGamepadAxisMovement(1, inputPair.first.code) > inputPair.second.deadzone)
                    {
                        if (!inputPair.second.active)
                        {
                            inputPair.second.pressed = true;
                            EventRegistry::Fire<>(inputPair.second.event);
                        }
                    }
                    else
                        inputPair.second.pressed = false;
                }
                else
                {
                    if (rl::IsGamepadButtonDown(1, inputPair.first.code))
                    {
                        if (!inputPair.second.active)
                        {
                            inputPair.second.pressed = true;
                            EventRegistry::Fire<>(inputPair.second.event);
                        }
                    }
                    else
                        inputPair.second.pressed = false;
                }
                break;
            }
            default:
                break;
        }
    }
}