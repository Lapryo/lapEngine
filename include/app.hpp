#ifndef APP_HPP
#define APP_HPP

#include <unordered_map>
#include "world.hpp"

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

        World world;

    protected:
        virtual bool Init() = 0;
        virtual void Update(float deltaTime) = 0;

    private:
        AppState state = AppState::DEAD;
    };
}

#endif