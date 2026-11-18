#include "project/registration.hpp"

#include "project/project.hpp"

#include "middleman.hpp"
#include "eutil.hpp"

using namespace lapCore;
using namespace EUTIL::Render;
using namespace EUTIL::Render::UI;

void lapCore::RegisterEUTIL()
{
    Middleman::Register<HorizontalAlignment>("horizontal-alignment");
    Middleman::Register<VerticalAlignment>("vertical-alignment");
    Middleman::Register<Alignment>("alignment");
    Middleman::Register<Renderable>("renderable");
    Middleman::Register<Axis2D>("axis-2d");
    Middleman::Register<ScrollSettings>("scroll-settings");
    Middleman::Register<Animated>("animated");
    Middleman::Register<Direction2D>("direction-2d");
    Middleman::Register<FrameVector>("frame-vector");
    Middleman::Register<Padding>("padding");
    Middleman::Register<UITransform>("ui-transform");
}