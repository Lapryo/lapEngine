#include "editor.hpp"
#include "projdata.hpp"

#include <iostream>
#include <random>

#include "json.hpp"

using namespace lapEditor;
using namespace lapCore;

nlohmann::json lapEditor::loadedProjectJson;
std::string lapEditor::loadedProjectFilePath;
unsigned int lapEditor::currentSceneIndex = 0;

EditorApp::EditorApp(Project &project) : App(project)
{
}

void EditorApp::Init()
{
    project.LoadSettings("assets/settings.json");

    std::string titles_str = ReadFileToString("assets/windowtitles.txt");
    if (titles_str != "")
    {
        std::vector<std::string> windowSubTitles;
        for (const auto &line : titles_str | std::views::split('\n'))
        {
            windowSubTitles.push_back(std::string(line.begin(), line.end()));
        }

        std::random_device rd;
        std::mt19937 rng(rd());

        std::uniform_int_distribution<int> intDist(0, windowSubTitles.size() - 1);
        std::string windowTitle = "lapEditor - " + windowSubTitles[intDist(rng)];

        rl::SetWindowTitle(windowTitle.c_str());
    }

    // Setup project JSON here
    loadedProjectJson = {
        {"name", "Untitled Project"},
        {"version", "1.0.0"},
        {"scenes", nlohmann::json::array()}
    };

    // setup objects here
    SelectionMenuObject::RegisterLogic();
    FPSLabelObject::RegisterLogic();
    SidebarObject::RegisterLogic();
    AddObjectObject::RegisterLogic();
    FileButtonObject::RegisterLogic();
    FileDropdownObject::RegisterLogic();
    OpenProjectObject::RegisterLogic();
    SaveProjectObject::RegisterLogic();
    NewProjectObject::RegisterLogic();
    CloseProjectObject::RegisterLogic();
    SceneButtonObject::RegisterLogic();

    state = AppState::RUNNING;
}


void EditorApp::Update(float deltaTime)
{
    if (rl::IsKeyReleased(rl::KEY_F11))
    {
        rl::ToggleFullscreen();
        rl::SetWindowPosition(100, 100);
    }

    project.main_scene->Update(deltaTime, project.target);
}

void lapEditor::LoadProjectFromFile(const std::string &filePath)
{
    auto projJson = nlohmann::json::parse(ReadFileToString(filePath));
    std::cout << "Loaded Project JSON:\n" << projJson.dump(4) << "\n";
    loadedProjectJson = projJson;
    loadedProjectFilePath = filePath;
}

void lapEditor::SaveProjectToFile(const std::string &filePath)
{
    std::string projJsonString = loadedProjectJson.dump(4);
    std::cout << "Project JSON to save:\n" << projJsonString << "\n";

    WriteStringToFile(filePath, loadedProjectJson.dump(4));
}

int main()
{
    Project editorProject = UnpackProject(proj_json);
    EditorApp editor(editorProject);

    editor.Init();
    editor.Run();
    editor.Shutdown();

    return 0;
}