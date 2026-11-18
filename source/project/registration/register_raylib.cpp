#include "project/registration.hpp"

#include "project/project.hpp"

#include "middleman.hpp"
#include "raylib.h"

using namespace lapCore;

void lapCore::RegisterRaylib()
{
    Middleman::Register<Camera2D>("rl-camera-2d");
    Middleman::Register<Vector2>("rl-vector2");
    Middleman::Register<Color>("rl-color");
    Middleman::Register<Rectangle>("rl-rectangle");
}