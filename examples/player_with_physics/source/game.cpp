#include "game.hpp"

/*
In this example we will be using option 2, described in the "game.cpp" file of the Blank Window example.
We will be using an actual project JSON directory to load into our world.

Below here we have our own custom element/struct for the player, this is where we store the speed of the
player and their input direction.
Notice below the struct, a JSON serializer for this element.
This allows for us to be able to serialize/deserialize to and from an element JSON, mainly used for project packing and unpacking.
If you plan on not storing any custom elements/structs inside the project, you do not need to do this.

I recommend looking at nlohmann's documentation if you plan to do this. I also recommend looking at MY documentation on how
to use JSON::Serializer.
Anyways, thanks nlohmann :)
*/

struct PlayerAttributes
{
    float speed;
    Vector2 inputDirection;
};
template <>
struct JSON::Serializer<PlayerAttributes>
{
    static json to_json(const PlayerAttributes& p, SerializeContext* ctx = nullptr)
    {
        return json{
            {"input-direction", JSON::Serializer<Vector2>::to_json(p.inputDirection, ctx)}, // Convert the raylib Vector2 to a JSON
            {"speed", p.speed}
        };
    }
    static void from_json(PlayerAttributes& p, const json& j)
    {
        if (j.contains("input-direction")) // Always check if it exists before getting it
            JSON::Serializer<Vector2>::from_json(p.inputDirection, j.at("input-direction"));
        p.speed = j.value("speed", p.speed); // Simply assign the value
    }
};

/*
Below we have a list of sorts of all our hashed strings, this is simply for ease of use so we don't have to call HASH(...) multiple
time. Specifically in the following Move function thats coming up. If you'd like to also do this, you can, but it is entire optional
and probably not that much of a performance boost or anything.

Below we also have two functions that setup our movement: registering the inputs, and the actual move function.

The Input System has a useful function that registers our actions for us, it essentially acts as a bridge to raylib's input stuff.
Just looking at the function be used in this case is pretty self explanatory, if you'd like more information... DOCUMENATION!!!
*/

constexpr auto plrUpEvent = HASH("player-move-up");
constexpr auto plrDownEvent = HASH("player-move-down");
constexpr auto plrLeftEvent = HASH("player-move-left");
constexpr auto plrRightEvent = HASH("player-move-right");
constexpr auto plrCreate = HASH("player_create");
constexpr auto plrUpdate = HASH("player_update");
constexpr auto plrName = HASH("player");
constexpr auto camUpdate = HASH("camera_update");

void RegisterMovementActions(InputSystem* inputSys)
{
    inputSys->RegisterAction(
        "move-up",
        plrUpEvent,
        {KEY_W}, 
        true, 
        InputSystem::InputType::KEYBOARD, 
        InputSystem::ControlType::BUTTON);
    inputSys->RegisterAction(
        "move-down", 
        plrDownEvent, 
        {KEY_S}, 
        true, 
        InputSystem::InputType::KEYBOARD, 
        InputSystem::ControlType::BUTTON);
    inputSys->RegisterAction(
        "move-left", 
        plrLeftEvent, 
        {KEY_A}, 
        true, 
        InputSystem::InputType::KEYBOARD, 
        InputSystem::ControlType::BUTTON);
    inputSys->RegisterAction(
        "move-right", 
        plrRightEvent, 
        {KEY_D}, 
        true, 
        InputSystem::InputType::KEYBOARD, 
        InputSystem::ControlType::BUTTON);
}

void Move(Scene *scene, Object object, entt::id_type eventID)
{
    auto playerAttributes = scene->FindElement<PlayerAttributes>(object);
    if (!playerAttributes) return; // Always check if it exists!

    if (eventID == plrUpEvent)
        playerAttributes->inputDirection.y -= 1;
    else if (eventID == plrDownEvent)
        playerAttributes->inputDirection.y += 1;
    else if (eventID == plrLeftEvent)
        playerAttributes->inputDirection.x -= 1;
    else if (eventID == plrRightEvent)
        playerAttributes->inputDirection.x += 1;
}

/*
Below is the Player namespace which holds their Init and Update function.
It is recommended object namespaces with their Init, Update, and Destroy functions be moved to other files to you don't just have one
giant game.cpp file, but for our case, because this example is pretty simple, we don't need to.
*/

namespace Player
{
    /*
    This function can be split into 2 parts: event connections, and gravity setup.
    Connecting events is very straightforward, just parameters for the listener you want to attach, the event ID,
    and the function tied to it.
    We actually use this same function (ConnectECSEvent()) to connect the Init and Update functions for the player later on!

    Then we set our gravity to nothing because we're in a void, pretty simple.
    */
    void Init(Scene *scene, Object object, entt::id_type eventID)
    {
        ConnectECSEvent(scene, object, plrDownEvent, Move);
        ConnectECSEvent(scene, object, plrUpEvent, Move);   
        ConnectECSEvent(scene, object, plrLeftEvent, Move);
        ConnectECSEvent(scene, object, plrRightEvent, Move);

        auto physicsSys = scene->GetSystem<PhysicsSystem>();
        b2World_SetGravity(physicsSys->worldID, b2Vec2{0, 0});
    }

    /*
    With events you can have as many parameters as you wish, but because this is specifically a script event, an update one at that
    the only extra parameter is the float delta.

    Here we are simply checking if our player exists within the PhysicsSystem, and if so, updating their linear velocity to match
    that of our input direction. Of course, normalizing that input direction vector and adding the speed and all.
    We make sure to reset the input direction at the end so our player doesn't continuously keep moving.

    We have an additional FPS counter at the top.
    */
    void Update(Scene *scene, Object object, entt::id_type eventID, float delta)
    {
        auto attributes = scene->FindElement<PlayerAttributes>(object);
        Vector2 normalized_vel = Vector2Normalize(attributes->inputDirection);
        auto physicsSys = scene->GetSystem<PhysicsSystem>();

        if (physicsSys && b2Body_IsValid(physicsSys->bodyMap[object]))
        {
            b2Body_SetLinearVelocity(physicsSys->bodyMap[object], {normalized_vel.x * attributes->speed, normalized_vel.y * attributes->speed});
        }
        
        attributes->inputDirection = {0.f, 0.f};

        std::string fpsText = "FPS: " + std::to_string(GetFPS());
        DrawText(fpsText.c_str(), 10, 10, 20, RED);
    }
}

/*
Same as before, but with a camera following the player. We just lerp the target of the camera to where the player is at
with smoothness and all.
*/

namespace PlayerCamera
{
    void Update(Scene *scene, Object object, entt::id_type eventID, float delta)
    {
        auto camElement = scene->FindElement<Camera2D>(object);
        if (!camElement) return;

        auto playerObject = *scene->FindObject(plrName);
        auto transform = scene->FindElement<Transform2D>(playerObject);
        if (!transform) return;

        const float smoothness = 10.f; // higher = faster follow
        camElement->target = Vector2Lerp(camElement->target,
                                        transform->position,
                                        delta * smoothness);
    }
}

// Same as with the Blank Window example, straightforward
namespace PlayerWithPhysics
{
    bool Game::Init()
    {
        /*
        This time, before loading our window, we will load a settings file. This settings file can be customized to whatever you
        want, but a specific part in that file named "window" can be used to customize the window settings.
        Or you can do it manually, it's up to you!
        */
        world.LoadSettings("assets/settings.json");
        world.LoadWindow();

        world.SetScene(world.GetMainSceneData());

        /*
        Here we register our inputs for this scene.
        Understand, that if we ever switch to a different scene, those inputs are now lost forever as the systems vector of
        that scene is now cleared out.
        Even if you switched back to this main scene used here, you would have to call RegisterMovementActions again!
        */
        RegisterMovementActions(world.mainScene->GetSystem<InputSystem>());

        
        // Here is where we connect our scripts to the event registry with the corresponding objects.
        auto plrObject = *world.mainScene->FindObject(plrName);
        ConnectECSEvent(world.mainScene, plrObject, plrCreate, Player::Init);
        ConnectECSEvent<float>(world.mainScene, plrObject, plrUpdate, Player::Update);
        
        auto camObject = *world.mainScene->FindObject(HASH_ID("camera"));
        ConnectECSEvent<float>(world.mainScene, camObject, camUpdate, PlayerCamera::Update);

        return true; // Return true if initialization was successful, false otherwise
    }

    void Game::Update(float delta)
    {
        world.mainScene->Update(delta, world.window.target);
    }
}

int main()
{
    /*
    For any element/struct you wish to be serialized and all, you must register it to the reflection registry.
    If it is already contained within the project you are loading, you must register it beforehand.
    */
    Reflection::Register<PlayerAttributes>("player-attributes");
    Project project = UnpackProject(ReadFileToString("assets/project/project.json"));

    PlayerWithPhysics::Game game(project);
    game.Run();

    return 0;
}