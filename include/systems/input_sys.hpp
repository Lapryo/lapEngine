#pragma once

#include "system.hpp"
using namespace lapCore;

class InputSystem : public System
{
public:
    InputSystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) {}

    void Update(float deltaTime, 
        entt::registry &registry
    ) override;

    std::vector<int> consumedKeys;
    std::vector<int> consumedMouseButtons;
    std::vector<int> consumedGamepadButtons;
    std::vector<int> consumedGamepadAxes;

    std::string GetName() const override 
    { return "InputSystem"; }
private:
    bool HandleKeyboardInput(
        Scene* scene, 
        ActionMap::InputEntry &entry, 
        int key
    );
    bool HandleMouseInput(
        Scene* scene, 
        ActionMap::InputEntry &entry, 
        int button
    );
    bool HandleGamepadInput(
        Scene* scene, 
        ActionMap::InputEntry &entry, 
        ActionMap::ControlType controlType, 
        int button, 
        int gamepad
    );
};