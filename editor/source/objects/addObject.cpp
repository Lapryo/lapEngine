#include "objects/addObject.hpp"

void AddObject(Scene* scene, Object object)
{
    auto selectionMenu = scene->FindObject("selectionMenu");

    scene->FindElement<Frame>(selectionMenu).renderable.visible = false;

    // TODO: Add an object to the project map if one has not been created yet

    // TODO: IMPLEMENT PREFABS

    std::string objectName = "Object";
    auto obj = scene->AddObject(objectName, "sidebar");

    Renderable renderable(30, true, true, (Color){125, 97, 103, 255});
    UIOrigin origin(
        FrameVector((Vector2){0, 0}, (Vector2){0, 0}),
        FrameVector((Vector2){0.2, 0.05}, (Vector2){0, 0})
    );

    Frame frame(renderable, origin);
    Alignment textAlignment(HorizontalAlignment::LEFT, VerticalAlignment::TOP);

    scene->AddElement<TextLabel>(obj, frame, objectName, 20, textAlignment, FrameVector{{0.2, 0.05}, {0, 0}}, Padding{5, 0, 5, 0});
}

void AddObjectObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["connect-add-object"] = [](Scene* scene, Object &object) {
        ConnectECSEvent(scene, object, "add-object", AddObject);
    };
}