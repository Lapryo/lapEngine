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
            if (!entry.sustain && entry.pressed) return true;
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
            if (!entry.sustain && entry.pressed) return true;
            entry.pressed = true;
            EventRegistry::Fire<>(entry.event);
        }
    }
    else
        entry.pressed = false;

    return entry.pressed;
}

bool HandleGamepadInput(InputSystem::InputEntry &entry, InputSystem::ControlType controlType, int button, int gamepad)
{
    if (controlType == InputSystem::ControlType::AXIS)
    {
        if (rl::GetGamepadAxisMovement(gamepad, button) > entry.deadzone.upper || rl::GetGamepadAxisMovement(gamepad, button) < entry.deadzone.lower)
        {
            if (entry.active)
            {
                if (!entry.sustain && entry.pressed) return true;
                entry.pressed = true;
                entry.value = rl::GetGamepadAxisMovement(gamepad, button);
                EventRegistry::Fire<>(entry.event);
            }
        }
        else
        {
            entry.pressed = false;
            entry.value = 0.0f;
        }
    }
    else
    {
        if (rl::IsGamepadButtonDown(gamepad, button))
        {
            if (entry.active)
            {
                if (!entry.sustain && entry.pressed) return true;
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
                     if (HandleGamepadInput(inputPair.second, inputPair.second.key.controlType, key, 0)) break;
                break;
            }
        }
    }
}

void InputSystem::RegisterAction(const std::string &actionName, const InputEntry &entry)
{
    actions[actionName] = entry;
}

void InputSystem::RegisterAction(const std::string &actionName, const std::string &event, std::vector<int> codes, bool sustain, InputType inputType, ControlType controlType, InputDeadzone deadzone, bool active)
{
    InputEntry entry;
    entry.key.inputType = inputType;
    entry.key.controlType = controlType;
    entry.key.codes = codes;
    entry.sustain = sustain;
    entry.active = active;
    entry.event = event;
    entry.deadzone = deadzone;

    RegisterAction(actionName, entry);
}