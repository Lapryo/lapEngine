#include "objects/fpslabel.hpp"

void FPSLabelObject::RegisterLogic()
{
    ScriptRegistry::onUpdateFunctions["updatefps"] = [](Scene *scene, Object object, float dt)
    {
        std::string fps = "FPS: " + std::to_string(rl::GetFPS());
        auto *textLabel = scene->FindElement<TextLabel>(scene->objects, object);
        if (textLabel)
            textLabel->text = fps;
    };
}