#include "core.hpp"
#include "systems/input_sys.hpp"

using namespace lapCore;

bool HandleKeyboardInput(Scene* scene, InputSystem::InputEntry &entry, int key)
{
    if (IsKeyDown(key))
    {
        if (entry.active)
        {
            if (!entry.sustain && entry.pressed) return true;
            entry.pressed = true;
            EventRegistry::Fire(scene, entry.eventID);
        }
    }
    else
        entry.pressed = false;

    return entry.pressed;
}

bool HandleMouseInput(Scene* scene, InputSystem::InputEntry &entry, int button)
{
    if (IsMouseButtonDown(button))
    {
        if (entry.active)
        {
            if (!entry.sustain && entry.pressed) return true;
            entry.pressed = true;
            EventRegistry::Fire(scene, entry.eventID);
        }
    }
    else
        entry.pressed = false;

    return entry.pressed;
}

bool HandleGamepadInput(Scene* scene, InputSystem::InputEntry &entry, InputSystem::ControlType controlType, int button, int gamepad)
{
    if (controlType == InputSystem::ControlType::AXIS)
    {
        if (GetGamepadAxisMovement(gamepad, button) > entry.deadzone.upper || GetGamepadAxisMovement(gamepad, button) < entry.deadzone.lower)
        {
            if (entry.active)
            {
                if (!entry.sustain && entry.pressed) return true;
                entry.pressed = true;
                entry.value = GetGamepadAxisMovement(gamepad, button);
                EventRegistry::Fire(scene, entry.eventID);
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
        if (IsGamepadButtonDown(gamepad, button))
        {
            if (entry.active)
            {
                if (!entry.sustain && entry.pressed) return true;
                entry.pressed = true;
                EventRegistry::Fire(scene, entry.eventID);
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
                    if (HandleKeyboardInput(scene, inputPair.second, key)) break;
                break;
            }
            case InputType::MOUSE:
            {
                for (int key : inputPair.second.key.codes)
                     if (HandleMouseInput(scene, inputPair.second, key)) break;
                break;
            }
            case InputType::GAMEPAD:
            {
                for (int key : inputPair.second.key.codes)
                     if (HandleGamepadInput(scene, inputPair.second, inputPair.second.key.controlType, key, 0)) break;
                break;
            }
        }
    }
}

void InputSystem::RegisterAction(const std::string &actionName, const InputEntry &entry)
{
    actions[actionName] = entry;
}

void InputSystem::RegisterAction(const std::string &actionName, entt::id_type eventID, std::vector<int> codes, bool sustain, InputType inputType, ControlType controlType, InputDeadzone deadzone, bool active)
{
    InputEntry entry;
    entry.key.inputType = inputType;
    entry.key.controlType = controlType;
    entry.key.codes = codes;
    entry.sustain = sustain;
    entry.active = active;
    entry.eventID = eventID;
    entry.deadzone = deadzone;

    RegisterAction(actionName, entry);
}