#include "project/registration.hpp"

#include "project/project.hpp"

#include "middleman.hpp"
#include "box2d/box2d.h"

using namespace lapCore;

void lapCore::RegisterBox2D()
{
    Middleman::Register<b2BodyDef>("b2-body-def");
    Middleman::Register<b2ShapeDef>("b2-shape-def");
    Middleman::Register<b2Polygon>("b2-polygon");
    Middleman::Register<b2Filter>("b2-filter");
    Middleman::Register<b2SurfaceMaterial>("b2-surface-material");
    Middleman::Register<b2MotionLocks>("b2-motion-locks");
    Middleman::Register<b2BodyType>("b2-body-type");
    Middleman::Register<b2Vec2>("b2-vec2");
    Middleman::Register<b2Rot>("b2-rot");
}