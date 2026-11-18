#include "game.hpp"

#include <algorithm>

// ============================================================
// Custom Elements
// ============================================================

struct PlayerAttributes
{
    float speed{0.0f};
    Vector2 inputDirection{0.0f, 0.0f};
};

template<>
struct lapCore::mmJSON::Serializer<PlayerAttributes>
{
    static json to_json(
        const PlayerAttributes& p,
        SerializeContext* ctx = nullptr)
    {
        return json{
            {
                "input-direction",
                mmJSON::Serializer<Vector2>::to_json(
                    p.inputDirection,
                    ctx
                )
            },
            {"speed", p.speed}
        };
    }

    static void from_json(PlayerAttributes& p, const json& j)
    {
        if (j.contains("input-direction"))
        {
            mmJSON::Serializer<Vector2>::from_json(
                p.inputDirection,
                j.at("input-direction")
            );
        }

        p.speed = j.value("speed", p.speed);
    }
};

// ============================================================
// Hashed IDs
// ============================================================

const auto plrUpEvent    = HASH_ID("player-move-up");
const auto plrDownEvent  = HASH_ID("player-move-down");
const auto plrLeftEvent  = HASH_ID("player-move-left");
const auto plrRightEvent = HASH_ID("player-move-right");

const auto plrCreate = HASH_ID("player_create");
const auto plrUpdate = HASH_ID("player_update");
const auto plrName   = HASH_ID("player");

const auto camUpdate = HASH_ID("camera_update");
const auto camName   = HASH_ID("camera");

// ============================================================
// Input
// ============================================================

ActionMap RegisterMovementActions()
{
    ActionMap movementActions;

    movementActions.RegisterAction(
        "move-up",
        plrUpEvent,
        {KEY_W},
        true,
        ActionMap::InputType::KEYBOARD,
        ActionMap::ControlType::BUTTON
    );

    movementActions.RegisterAction(
        "move-down",
        plrDownEvent,
        {KEY_S},
        true,
        ActionMap::InputType::KEYBOARD,
        ActionMap::ControlType::BUTTON
    );

    movementActions.RegisterAction(
        "move-left",
        plrLeftEvent,
        {KEY_A},
        true,
        ActionMap::InputType::KEYBOARD,
        ActionMap::ControlType::BUTTON
    );

    movementActions.RegisterAction(
        "move-right",
        plrRightEvent,
        {KEY_D},
        true,
        ActionMap::InputType::KEYBOARD,
        ActionMap::ControlType::BUTTON
    );

    return movementActions;
}

void Move(Scene* scene, Object object, id_type eventID)
{
    auto* attributes = scene->FindElement<PlayerAttributes>(object);

    if (!attributes)
        return;

    if (eventID == plrUpEvent)
        attributes->inputDirection.y -= 1.0f;
    else if (eventID == plrDownEvent)
        attributes->inputDirection.y += 1.0f;
    else if (eventID == plrLeftEvent)
        attributes->inputDirection.x -= 1.0f;
    else if (eventID == plrRightEvent)
        attributes->inputDirection.x += 1.0f;
}

// ============================================================
// Player
// ============================================================

namespace Player
{
    void Init(Scene* scene, Object object, id_type eventID)
    {
        ConnectECSEvent(scene, object, plrUpEvent, Move);
        ConnectECSEvent(scene, object, plrDownEvent, Move);
        ConnectECSEvent(scene, object, plrLeftEvent, Move);
        ConnectECSEvent(scene, object, plrRightEvent, Move);

        auto* physicsSys = scene->FindSystem<PhysicsSystem>();

        if (physicsSys)
        {
            b2World_SetGravity(
                physicsSys->worldID,
                b2Vec2{0.0f, 0.0f}
            );
        }
    }

    void Update(
        Scene* scene,
        Object object,
        id_type eventID,
        float delta)
    {
        auto* attributes = scene->FindElement<PlayerAttributes>(object);

        if (!attributes)
            return;

        const Vector2 direction =
            Vector2Normalize(attributes->inputDirection);

        auto* physicsSys = scene->FindSystem<PhysicsSystem>();

        if (physicsSys)
        {
            auto it = physicsSys->bodyMap.find(object);

            if (it != physicsSys->bodyMap.end() &&
                b2Body_IsValid(it->second))
            {
                b2Body_SetLinearVelocity(
                    it->second,
                    b2Vec2{
                        direction.x * attributes->speed,
                        direction.y * attributes->speed
                    }
                );
            }
        }

        // Reset movement for the next frame.
        attributes->inputDirection = {0.0f, 0.0f};
    }
}

// ============================================================
// Player Camera
// ============================================================

namespace PlayerCamera
{
    void Update(
        Scene* scene,
        Object object,
        id_type eventID,
        float delta)
    {
        auto* camera = scene->FindElement<Camera2D>(object);

        if (!camera)
            return;

        auto playerObject = scene->FindFirstObject(plrName);

        if (playerObject == entt::null)
            return;

        auto* transform =
            scene->FindElement<Transform2D>(playerObject);

        if (!transform)
            return;

        constexpr float smoothness = 10.0f;

        camera->target = Vector2Lerp(
            camera->target,
            transform->position,
            std::clamp(delta * smoothness, 0.0f, 1.0f)
        );
    }
}

// ============================================================
// Game
// ============================================================

namespace PlayerWithPhysics
{
    bool Game::Init()
    {
        world.LoadSettings("assets/settings.json");
        world.LoadWindow();

        world.LoadScenes(HASH_ID("main"));

        if (!world.mainScene)
            return false;

        world.actionMaps["movement"] = RegisterMovementActions();
        world.currentActionMap = &world.actionMaps.at("movement");

        auto playerObject = world.mainScene->FindFirstObject(plrName);
        auto cameraObject = world.mainScene->FindFirstObject(camName);

        if (playerObject == entt::null ||
            cameraObject == entt::null)
        {
            return false;
        }

        ConnectECSEvent(
            world.mainScene,
            playerObject,
            plrCreate,
            Player::Init
        );

        ConnectECSEvent<float>(
            world.mainScene,
            playerObject,
            plrUpdate,
            Player::Update
        );

        ConnectECSEvent<float>(
            world.mainScene,
            cameraObject,
            camUpdate,
            PlayerCamera::Update
        );

        return true;
    }

    void Game::Update(float delta)
    {
        if (WindowShouldClose())
        {
            Shutdown();
            return;
        }

        if (world.mainScene)
        {
            world.mainScene->Update(delta, world.window.target);
        }
    }
}

// ============================================================
// Entry Point
// ============================================================

int main()
{
    Middleman::Register<PlayerAttributes>("player-attributes");

    Project project = LoadProject("assets/project/project.json");

    PlayerWithPhysics::Game game(project);
    game.Run();

    return 0;
}