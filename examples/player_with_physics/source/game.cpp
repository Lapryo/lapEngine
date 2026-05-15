#include "game.hpp"

#include "proj_data.hpp" // THIS IS IMPORTANT!
// The proj_data.hpp file contains everything that is in the project in a string format
// This prevents others from being able to easily snoop around the project files when you distribute your game, making it easy to find exploits or assets you may not want to share freely
// If you're okay with this, you can simply load the project from a separate file instead of including it like this
// Since we already did this for the pong application, we will instead load from a file in this example

/*
TODO:
1. Finish assets for the game
2. Implement the logic for the game
3. Comment everything so other developers can look at this as a basis for their games/apps
*/

// Constructor for the application, takes in the project to run
FlappyBird::FBApp::FBApp(lapCore::Project &project) : App(project)
{}

struct PlayerAttributes
{
    float speed;
    float damage;
    float defense;
    float health;
    float maximum_health;
    float stamina;

    float look_distance;
    Vector2 input_velocity;
    Vector2 look_direction;

    PlayerAttributes(float spd = 100.f, Vector2 iv = {0.f, 0.f})
        : speed(spd), damage(10.f), defense(5.f), health(100.f), maximum_health(100.f), input_velocity(iv), stamina(10.f), look_direction({0.f, 0.f}), look_distance(25.f) {}
};

void RegisterCombatActions(InputSystem* inputSys)
{
    // Movement
    inputSys->RegisterAction("move-up", "player-move-up", {KEY_W}, true, InputSystem::InputType::KEYBOARD, InputSystem::ControlType::BUTTON);
    inputSys->RegisterAction("move-down", "player-move-down", {KEY_S}, true, InputSystem::InputType::KEYBOARD, InputSystem::ControlType::BUTTON);
    inputSys->RegisterAction("move-left", "player-move-left", {KEY_A}, true, InputSystem::InputType::KEYBOARD, InputSystem::ControlType::BUTTON);
    inputSys->RegisterAction("move-right", "player-move-right", {KEY_D}, true, InputSystem::InputType::KEYBOARD, InputSystem::ControlType::BUTTON);
}

void Move(Scene *scene, Object object, std::string eventName)
{
    auto playerAttributes = scene->FindElement<PlayerAttributes>(scene->objects, object);
    if (!playerAttributes) return;

    if (eventName == "player-move-up")
        playerAttributes->input_velocity.y -= 1;
    else if (eventName == "player-move-down")
        playerAttributes->input_velocity.y += 1;
    else if (eventName == "player-move-left")
        playerAttributes->input_velocity.x -= 1;
    else if (eventName == "player-move-right")
        playerAttributes->input_velocity.x += 1;
    else if (eventName == "player-axis-move-vertical")
        playerAttributes->input_velocity.y = scene->GetSystem<InputSystem>()->actions["controller-move-vertical"].value;
    else if (eventName == "player-axis-move-horizontal")
        playerAttributes->input_velocity.x = scene->GetSystem<InputSystem>()->actions["controller-move-horizontal"].value;
}

bool FlappyBird::FBApp::Init()
{
    // Anything specific to Flappy Bird initialization can go here (registering logic, loading settings, etc.)

    // lapEngine has a specific function for loading settings for the window
    world.LoadSettings("assets/settings.json");
    world.LoadWindow();

    Project project = world.GetProject();
    world.SetScene(project.scenes[project.main_scene_index]);

    RegisterCombatActions(world.main_scene.GetSystem<InputSystem>());


    // Anything else you may want to do is recommended to be done between these two sections of initialization

    // This is a good spot to register logic for any objects you may want
    // Since this is flappy bird, we register logic for the pipes and the bird here

    // ...

    ScriptRegistry::onCreateFunctions["player_create"] = [](lapCore::Scene *scene, lapCore::Object &object)
    {
        scene->AddElement<PlayerAttributes>(scene->objects, object, 100.f, Vector2{0.f, 0.f});

        ConnectECSEvent(scene, object, "player-move-up", Move);
        ConnectECSEvent(scene, object, "player-move-down", Move);   
        ConnectECSEvent(scene, object, "player-move-left", Move);
        ConnectECSEvent(scene, object, "player-move-right", Move);

        auto physicsSys = scene->GetSystem<PhysicsSystem>();
        b2World_SetGravity(physicsSys->worldID, b2Vec2{0, 0});
    };

    ScriptRegistry::onUpdateFunctions["player_update"] = [](lapCore::Scene *scene, lapCore::Object object, float deltaTime)
    {
        auto physics = scene->FindElement<lapCore::Physics2D>(scene->objects, object);
        if (physics)
        {
            auto attributes = scene->FindElement<PlayerAttributes>(scene->objects, object);
            Vector2 normalized_vel = Vector2Normalize(attributes->input_velocity);

            if (b2Body_IsValid(physics->bodyID))
            {
                b2Body_SetLinearVelocity(physics->bodyID, {normalized_vel.x * attributes->speed, normalized_vel.y * attributes->speed});
            }
            
            attributes->input_velocity = {0.f, 0.f};
        }

        std::string fpsText = "FPS: " + std::to_string(GetFPS());
        DrawText(fpsText.c_str(), 10, 10, 20, RED);
    };

    ScriptRegistry::onUpdateFunctions["camera-follow-player"] = [](lapCore::Scene *scene, lapCore::Object object, float deltaTime)
    {
        auto camElement = scene->FindElement<Cam2D>(scene->objects, object);
        if (!camElement) return;

        auto playerObject = scene->FindObject("player").info.object;
        if (playerObject == entt::null) return;

        auto origin = scene->FindElement<Origin2D>(scene->objects, playerObject);
        if (!origin) return;

        const float smoothness = 10.f; // higher = faster follow
        camElement->camera.target = Vector2Lerp(camElement->camera.target,
                                        origin->position,
                                        deltaTime * smoothness);
    };

    return true; // Return true if initialization was successful, false otherwise
}

float introMusicPlayed = false;

void FlappyBird::FBApp::Update(float deltaTime)
{
    // Anything you want to do every frame can go here
    // For example, checking for input to make the window fullscreen, updating the main scene, etc.
    // However, you wll most likely main do updates through scripts or other elements, so it's recommended to keep this section clean and only put things here that are necessary to be done every frame and aren't easily done through other means

    // For us, we simply update the main scene
    world.main_scene.Update(deltaTime, world.window.target);

    if (!IsSoundPlaying(world.resources.sounds["intro-main-menu-music"]))
        introMusicPlayed = true;
    
    if (introMusicPlayed)
    {
        if (!IsMusicStreamPlaying(world.resources.music["main-menu-music"]))
            PlayMusicStream(world.resources.music["main-menu-music"]);
        else
            UpdateMusicStream(world.resources.music["main-menu-music"]);
    }
}

int main()
{
    // Our actual main function, where we create the project and run the application

    Project fbProject = UnpackProject(ReadFileToString("assets/project/project.json")); /* Remember earlier? We are going to read a file into a string and unpack it here
                                                                                           You must also do the same that you did with the settings file to the project file */
    // lapEngine contains many useful functions like this to just make development easier so you can focus on creating rather than boilerplate code
    FlappyBird::FBApp app(fbProject); // Create the application with the project
    app.Run(); // Now call run on the application, not the update function you have!
    // We do this because the base App class has a run loop that handles window closing and delta time calculation for us

    return 0;
}