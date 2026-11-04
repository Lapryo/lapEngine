#pragma once

#include "system.hpp"
#include "actionmap.hpp"
using namespace lapCore;

class InputSystem : public System
{
public:
    InputSystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) {}

    void Update(float deltaTime, 
        registry &registry
    ) override;

    vector<int> consumedKeys;
    vector<int> consumedMouseButtons;
    vector<int> consumedGamepadButtons;
    vector<int> consumedGamepadAxes;

    string GetName() const override 
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