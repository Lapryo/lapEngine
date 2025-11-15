#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "core.hpp"

namespace lapEditor
{
    class EditorApp : public lapCore::App
    {
    public:
        EditorApp(lapCore::Project &project);

        void Init() override;
        void Update(float deltaTime) override;
        void Draw() override;
    };
}

#endif