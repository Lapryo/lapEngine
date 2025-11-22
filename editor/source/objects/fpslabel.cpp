#include "objects/fpslabel.hpp"

void FPSLabelObject::RegisterLogic()
{
    ScriptRegistry::onUpdateFunctions["updatefps"] = [](Scene *scene, Object object, float dt)
    {
        std::string fps = "FPS: " + std::to_string(GetFPS());
        auto *textLabel = scene->FindElement<TextLabel>(object);
        if (textLabel)
            textLabel->text = fps;
    };
}