#include "project/registration.hpp"

#include "project/project.hpp"

#include "middleman.hpp"
#include "elements.hpp"

using namespace lapCore;
using namespace ELEMENTS::Render;
using namespace ELEMENTS::Render::UI;
using namespace ELEMENTS::Physics;
using namespace ELEMENTS::Other;

void lapCore::RegisterElements()
{
    Middleman::Register<Transform2D>("transform-2d");
    Middleman::Register<Physics2D>("physics-2d");
    Middleman::Register<SoundPoint>("sound-point");
    Middleman::Register<UIFrame>("ui-frame");
    Middleman::Register<UIList>("ui-list");
    Middleman::Register<UIGradient>("ui-gradient");
    //Middleman::Register<UIGrid>("ui-grid");
    Middleman::Register<Sprite>("sprite");
    Middleman::Register<UIImage>("ui-image");
    Middleman::Register<UITextLabel>("ui-text-label");
    Middleman::Register<UIButton>("ui-button");
    Middleman::Register<Tags>("tags");
    Middleman::Register<Parameter>("parameter");
    Middleman::Register<Parameters>("parameters");
    Middleman::Register<Script>("script");
}