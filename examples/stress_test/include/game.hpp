#pragma once

#include "core.hpp"
using namespace lapCore;

namespace StressTest
{
    class Game : public App
    {
    public:
        Game(Project &project) : App(project) {}
        bool Init() override;
        void Update(float deltaTime) override;
    };
}