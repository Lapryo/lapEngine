#pragma once

/*
This example will be a empty white void with a red controllable player with 9 floating green blocks that you can bash
your little red body into. Unfortunately, those green blocks will likely continue moving into nothingness when you come
into contact with them. :(

This specific example essentially covers everything on how to use lapEngine

Everything here is essentially the same from the Blank Window example, except for the name of the namespace.
*/

#include "core.hpp"
using namespace lapCore;

namespace PlayerWithPhysics
{
    class Game : public App
    {
    public:
        Game(Project &project) : App(project) {}
        bool Init() override;
        void Update(float deltaTime) override;
    };
}