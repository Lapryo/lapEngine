#pragma once

#include "core.hpp"

namespace BlankWindow
{
    class BWApp : public lapCore::App
    {
    public:
        BWApp(lapCore::Project &project);
        bool Init() override;
        void Update(float deltaTime) override;
    };
}