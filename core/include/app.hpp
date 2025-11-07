#ifndef APP_HPP
#define APP_HPP

#include <unordered_map>
#include "project.hpp"

namespace lapCore
{
    enum class AppState
    {
        DEAD,
        RUNNING,
        ERROR
    };

    class App
    {
    public:
        virtual ~App() = default;
        App(Project &project);

        virtual void Init() = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Draw() = 0;

        void Run();
        void Shutdown();

        AppState state = AppState::DEAD;
        Project project;
    };

}

#endif