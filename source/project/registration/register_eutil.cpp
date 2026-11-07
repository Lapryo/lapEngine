#include "project/registration.hpp"

#include "project/project.hpp"

#include "reflection.hpp"
#include "eutil.hpp"

using namespace lapCore;
using namespace EUTIL::Render;
using namespace EUTIL::Render::UI;

void lapCore::RegisterEUTIL()
{
    Reflection::Register<HorizontalAlignment>("horizontal-alignment");
    Reflection::Register<VerticalAlignment>("vertical-alignment");
    Reflection::Register<Alignment>("alignment");
    Reflection::Register<Renderable>("renderable");
    Reflection::Register<Axis2D>("axis-2d");
    Reflection::Register<ScrollSettings>("scroll-settings");
    Reflection::Register<Animated>("animated");
    Reflection::Register<Direction2D>("direction-2d");
    Reflection::Register<FrameVector>("frame-vector");
    Reflection::Register<Padding>("padding");
    Reflection::Register<UITransform>("ui-transform");
}