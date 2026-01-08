#ifndef APP_HPP
#define APP_HPP

#include <unordered_map>
#include "project.hpp"

// TODO: Known issue
// Because the project variable is now private, need to come up with a solution so it's accessible in a way to the developer
// Maybe after the "World" section of the architecture is implemented, this problem will fix itself

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

        void Run();
        void Shutdown();

    protected:
        virtual bool Init() = 0;
        virtual void Update(float deltaTime) = 0;

    private:
        AppState state = AppState::DEAD;
        const Project& project;
    };
}

#endif