#include "project/registration.hpp"

#include "project/project.hpp"

#include "reflection.hpp"
#include "box2d/box2d.h"

using namespace lapCore;

void lapCore::RegisterBox2D()
{
    Reflection::Register<b2BodyDef>("b2-body-def");
    Reflection::Register<b2ShapeDef>("b2-shape-def");
    Reflection::Register<b2Polygon>("b2-polygon");
    Reflection::Register<b2Filter>("b2-filter");
    Reflection::Register<b2SurfaceMaterial>("b2-surface-material");
    Reflection::Register<b2MotionLocks>("b2-motion-locks");
    Reflection::Register<b2BodyType>("b2-body-type");
    Reflection::Register<b2Vec2>("b2-vec2");
    Reflection::Register<b2Rot>("b2-rot");
}