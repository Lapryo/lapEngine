#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "core.hpp"

namespace lapEditor
{
    class Editor : public lapCore::App
    {
    public:
        Editor(lapCore::Project &project);

        void Init() override;
        void Update(float deltaTime) override;
        void Draw() override;
    };

    void PackLevel();
    void UnpackLevel();

};

#endif