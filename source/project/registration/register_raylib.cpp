#include "project/registration.hpp"

#include "project/project.hpp"

#include "reflection.hpp"
#include "raylib.h"

using namespace lapCore;

void lapCore::RegisterRaylib()
{
    Reflection::Register<Camera2D>("rl-camera-2d");
    Reflection::Register<Vector2>("rl-vector2");
    Reflection::Register<Color>("rl-color");
    Reflection::Register<Rectangle>("rl-rectangle");
}