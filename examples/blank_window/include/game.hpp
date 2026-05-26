#pragma once

/*
This example is as the name suggests, just a blank window.

All games should have a class that inherits from App, as this is where you override the Init and Update functions for your game
or application or whatever you want to call it.
I won't go into too much detail as from what you can see, it's pretty self explanatory.
For all intensive purposes, we are calling this class "Game", even though this example won't be much of a real game.

Always include "core.hpp" in your game files, this gives you all the necessary functions, structs, or whatever you need.
Having "using namespace lapCore;" is optional, but saves you from the hassle of doing "lapCore::" before everything.
*/

#include "core.hpp"
using namespace lapCore;

namespace BlankWindow
{
    class Game : public App
    {
    public:
        Game(lapCore::Project &project) : App(project) {}
        bool Init() override;
        void Update(float deltaTime) override;
    };
}