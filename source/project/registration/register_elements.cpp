#include "project/registration.hpp"

#include "project/project.hpp"

#include "reflection.hpp"
#include "elements.hpp"

using namespace lapCore;
using namespace ELEMENTS::Render;
using namespace ELEMENTS::Render::UI;
using namespace ELEMENTS::Physics;
using namespace ELEMENTS::Other;

void lapCore::RegisterElements()
{
    Reflection::Register<Transform2D>("transform-2d");
    Reflection::Register<Physics2D>("physics-2d");
    Reflection::Register<SoundPoint>("sound-point");
    Reflection::Register<UIFrame>("ui-frame");
    Reflection::Register<UIList>("ui-list");
    Reflection::Register<UIGradient>("ui-gradient");
    //Reflection::Register<UIGrid>("ui-grid");
    Reflection::Register<Sprite>("sprite");
    Reflection::Register<UIImage>("ui-image");
    Reflection::Register<UITextLabel>("ui-text-label");
    Reflection::Register<UIButton>("ui-button");
    Reflection::Register<Tags>("tags");
    Reflection::Register<Parameter>("parameter");
    Reflection::Register<Parameters>("parameters");
    Reflection::Register<Script>("script");
}