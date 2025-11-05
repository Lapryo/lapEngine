#ifndef HUB_HPP
#define HUB_HPP

#include "core.hpp"

namespace lapHub
{
    class HubApp : public lapCore::App
    {
    public:
        HubApp(lapCore::Project &project);

        void Init() override;
        void Update(float deltaTime) override;
        void Draw() override;
    };
}

#endif