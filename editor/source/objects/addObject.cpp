#include "objects/addObject.hpp"
#include "editor.hpp"

void AddObject(Scene *scene, Object object)
{
    auto selectionMenu = scene->FindObject("selectionMenu");

    auto *frame = scene->FindElement<Frame>(selectionMenu);
    if (frame)
        frame->renderable.visible = false;

    // TODO: Add an object to the project map if one has not been created yet

    // TODO: IMPLEMENT PREFABS

    std::string objectName = "Object";
    auto obj = scene->AddObject(objectName, "sidebar", -1);

    Renderable renderable(30, true, true, (rl::Color){125, 97, 103, 255});
    UIOrigin origin(
        FrameVector((rl::Vector2){0, 0}, (rl::Vector2){0, 0}),
        FrameVector((rl::Vector2){0.2, 0.05}, (rl::Vector2){0, 0}));

    Frame textframe(renderable, origin);
    Alignment textAlignment(HorizontalAlignment::LEFT, VerticalAlignment::TOP);

    scene->AddElement<TextLabel>(obj, textframe, objectName, 20, textAlignment, FrameVector{{0.2, 0.05}, {0, 0}}, Padding{5, 0, 5, 0});
}

void AddObjectObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["connect-add-object"] = [](Scene *scene, Object object)
    {
        ConnectECSEvent(scene, object, "add-object", AddObject);
    };
}