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

    void LoadProjectFromFile(const std::string &filePath);
    void SaveProjectToFile(const std::string &filePath);

    extern nlohmann::json loadedProjectJson;
}

#endif