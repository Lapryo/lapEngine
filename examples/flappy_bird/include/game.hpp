#pragma once

#include "core.hpp"

// Base application class for Flappy Bird example
namespace FlappyBird
{
    class FBApp : public lapCore::App
    {
    public:
        FBApp(lapCore::Project &project);
        void Init() override;
        void Update(float deltaTime) override;
    };
}