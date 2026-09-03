#include "core.hpp"
#include "systems/input_sys.hpp"

using namespace lapCore;

bool InputSystem::HandleKeyboardInput(
    Scene* scene,
    ActionMap::InputEntry& entry,
    int key
)
{
    const bool down = IsKeyDown(key);
    entry.pressed = down;

    if (!down || !entry.active)
        return false;

    // Sustained: every frame while held.
    // Non-sustained: only the physical down transition.
    const bool shouldFire =
        entry.sustain || IsKeyPressed(key);

    if (!shouldFire)
        return true;

    const auto consumedIt =
        std::find(consumedKeys.begin(), consumedKeys.end(), key);

    const bool consumed =
        consumedIt != consumedKeys.end();

    if (consumed && !entry.shared)
        return true;

    // Mark it consumed regardless of whether this entry is shared.
    // "shared" means this entry may ignore previous consumption,
    // not that this entry never consumes the key.
    if (!consumed)
        consumedKeys.push_back(key);

    EventRegistry::Fire(scene, entry.eventID);
    return true;
}

bool InputSystem::HandleMouseInput(
    Scene* scene,
    ActionMap::InputEntry& entry,
    int button
)
{
    const bool down = IsMouseButtonDown(button);
    entry.pressed = down;

    if (!down || !entry.active)
        return false;

    // Sustained: every frame while held.
    // Non-sustained: only the physical down transition.
    const bool shouldFire =
        entry.sustain || IsMouseButtonPressed(button);

    if (!shouldFire)
        return true;

    const bool consumed =
        std::find(
            consumedKeys.begin(),
            consumedKeys.end(),
            button
        ) != consumedKeys.end();

    if (consumed && !entry.shared)
        return true;

    // Shared inputs may ignore previous consumption, but still consume
    // the input for later non-shared actions.
    if (!consumed)
        consumedKeys.push_back(button);

    EventRegistry::Fire(scene, entry.eventID);
    return true;
}

bool InputSystem::HandleGamepadInput(
    Scene* scene,
    ActionMap::InputEntry& entry,
    ActionMap::ControlType controlType,
    int button,
    int gamepad
)
{
    bool down = false;
    bool freshlyPressed = false;
    float value = 0.0f;

    const bool wasPressed = entry.pressed;

    if (controlType == ActionMap::ControlType::AXIS)
    {
        value = GetGamepadAxisMovement(gamepad, button);

        down =
            value > entry.deadzone.upper ||
            value < entry.deadzone.lower;

        // An axis is freshly pressed when it transitions from
        // inside the deadzone to outside the deadzone.
        freshlyPressed = down && !wasPressed;
    }
    else
    {
        
        down = IsGamepadButtonDown(gamepad, button);
        freshlyPressed = IsGamepadButtonPressed(gamepad, button);
        value = down ? 1.0f : 0.0f;
    }

    /*
     * Always track the physical state, even while the entry is inactive.
     *
     * This prevents an action enabled while an axis is already held
     * from interpreting that held axis as a fresh press.
     */
    entry.pressed = down;

    if (!down)
    {
        entry.value = 0.0f;
        return false;
    }

    if (!entry.active)
    {
        entry.value = 0.0f;
        return false;
    }

    entry.value = value;

    // Sustained: every frame while held.
    // Non-sustained: only the physical down transition.

    std::cout << "recieved input\n";

    const bool shouldFire =
        entry.sustain || freshlyPressed;

    if (!shouldFire)
        return true;

    const bool consumed =
        std::find(
            consumedKeys.begin(),
            consumedKeys.end(),
            button
        ) != consumedKeys.end();

    if (consumed && !entry.shared)
        return true;

    if (!consumed)
        consumedKeys.push_back(button);

    EventRegistry::Fire(scene, entry.eventID);
    std::cout << "fired input\n";
    return true;
}

void InputSystem::Update(float deltaTime, entt::registry &registry)
{
    consumedKeys.clear();

    for (auto &inputPair : scene->world->currentActionMap->actions)
    {
        switch (inputPair.second.key.inputType)
        {
            case ActionMap::InputType::KEYBOARD:
            {
                for (int key : inputPair.second.key.codes)
                    if (HandleKeyboardInput(scene, inputPair.second, key))
                        break;
                break;
            }
            case ActionMap::InputType::MOUSE:
            {
                for (int key : inputPair.second.key.codes)
                    if (HandleMouseInput(scene, inputPair.second, key))
                        break;
                break;
            }
            case ActionMap::InputType::GAMEPAD:
            {
                for (int key : inputPair.second.key.codes)
                    if (HandleGamepadInput(scene, inputPair.second, inputPair.second.key.controlType, key, 0))
                        break;
                break;
            }
        }
    }
}