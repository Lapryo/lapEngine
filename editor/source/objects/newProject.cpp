#include "objects/newProject.hpp"
#include "editor.hpp"

void NewProject(Scene *scene, Object object)
{
    
}

void HighlightNewProjectButton(Scene *scene, Object object)
{
    std::cout << "mouse enter\n";
    auto *frame = scene->FindElement<Frame>(object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 25};
}

void UnhighlightNewProjectButton(Scene *scene, Object object)
{
    std::cout << "mouse leave\n";
    auto *frame = scene->FindElement<Frame>(object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 0};
}

void NewProjectObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-new-project-button"] = [](Scene *scene, Object object)
    {
        ConnectECSEvent(scene, object, "new-project", NewProject);
        ConnectECSEvent(scene, object, "highlight-new-project-button", HighlightNewProjectButton);
        ConnectECSEvent(scene, object, "unhighlight-new-project-button", UnhighlightNewProjectButton);
    };
}