#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "core.hpp"
#include "objects.hpp"
#include "json.hpp"

namespace lapEditor
{
    class EditorApp : public lapCore::App
    {
    public:
        EditorApp(lapCore::Project &project);

        void Init() override;
        void Update(float deltaTime) override;
    };

    static nlohmann::json loadedProjectJson;
}

#endif