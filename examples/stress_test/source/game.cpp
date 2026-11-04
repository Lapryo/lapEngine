
#include "game.hpp"

#include <algorithm>
#include <iostream>
#include <string>

/*
generally try to avoid coding like this, there's a reason why Projects exist.
:v)
but thank you AI for this.
*/

using namespace std;

namespace StressTest
{
    constexpr int GRID_SIZE = 50;
    constexpr float BOX_FILL = 0.9f;
    constexpr float GROUND_HEIGHT = 20.f;
    constexpr float GROUND_MARGIN = 20.f;
    constexpr float GRID_CLEARANCE = 60.f;

    int row = 0;
    bool finished = false;

    // ---------------------------------------------------------
    // HELPERS
    // ---------------------------------------------------------

    float GetGroundY(const Vector2& resolution)
    {
        return resolution.y - GROUND_MARGIN - GROUND_HEIGHT;
    }

    float GetCellSize(const Vector2& resolution)
    {
        const float availableHeight =
            GetGroundY(resolution) - GRID_CLEARANCE;

        return min(
            resolution.x,
            availableHeight
        ) / static_cast<float>(GRID_SIZE);
    }

    // Creates a UIFrame whose visual geometry is centered
    // around its physics body's position.
    //
    // RenderSystem currently uses:
    //
    // rect.x += Transform2D.position.x;
    // rect.y += Transform2D.position.y;
    //
    // Therefore, a negative half-size offset is required.
    UIFrame MakePhysicsFrame(float width, float height, Color tint)
    {
        UIFrame result{};

        result.renderable.tint = tint;
        result.renderable.isScreenSpace = false;

        result.transform.anchor.offset = {
            width * 0.5f, height * 0.5f
        };

        result.transform.size.scale = {0.f, 0.f};
        result.transform.size.offset = {width, height};

        result.transform.anchor.scale = {0.f, 0.f};
        result.transform.dirty = true;

        return result;
    }

    // ---------------------------------------------------------
    // SPAWN DYNAMIC BOX
    // ---------------------------------------------------------

    void SpawnBox(Scene* scene, int column, int rowIndex)
    {
        const Vector2 resolution =
            scene->world->window.logical_resolution;

        const float cellSize = GetCellSize(resolution);
        const float boxSize = cellSize * BOX_FILL;
        const float halfSize = boxSize * 0.5f;

        const float left = column * cellSize;
        const float top = rowIndex * cellSize;

        // Body positions use the CENTER of the box.
        // 1 logical pixel = 1 Box2D meter.
        const Vector2 center = {
            left + halfSize,
            top + halfSize
        };

        const string name =
            "object_" +
            to_string(column) + "_" +
            to_string(rowIndex);

        const Object entity =
            scene->AddObject(HASH_ID(name.c_str()));

        if (entity == null)
            return;

        // Physics2D defines the initial physical state.
        Physics2D physics{};

        auto bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {
            center.x,
            center.y
        };

        physics.bodyDef = bodyDef;
        physics.shapeDef = b2DefaultShapeDef();
        physics.polygon = b2MakeBox(
            halfSize,
            halfSize
        );

        // Transform2D is populated by PhysicsSystem.
        Transform2D transform{};

        // UIFrame provides appearance and dimensions.
        UIFrame visual = MakePhysicsFrame(
            boxSize,
            boxSize,
            {255, 0, 0, 255}
        );

        scene->AddElement<Transform2D>(entity, transform);
        scene->AddElement<Physics2D>(entity, physics);
        scene->AddElement<UIFrame>(entity, visual);
    }

    constexpr float CAMERA_SPEED = 100.f;

    void CameraUpdate(Scene* scene, Object object, id_type eventID, float delta)
    {
        auto cam = scene->FindElement<Camera2D>(object);
        if (!cam) return;

        if (IsKeyDown(KEY_A))
            cam->target.x -= CAMERA_SPEED * delta;
        if (IsKeyDown(KEY_D))
            cam->target.x += CAMERA_SPEED * delta;
        if (IsKeyDown(KEY_W))
            cam->target.y -= CAMERA_SPEED * delta;
        if (IsKeyDown(KEY_S))
            cam->target.y += CAMERA_SPEED * delta;
    }

    // ---------------------------------------------------------
    // RUNNER SCRIPT
    // ---------------------------------------------------------

    void RunnerUpdate(
        Scene* scene,
        Object object,
        id_type eventID,
        float delta
    )
    {
        if (!finished)
        {
            // Create exactly one row per update.
            for (int i = 0; i < GRID_SIZE; ++i)
            {
                SpawnBox(scene, i, row);
            }

            // Register newly created bodies once per row.
            // RegisterBodies must skip already registered bodies.
            if (auto* physicsSys =
                    scene->FindSystem<PhysicsSystem>())
            {
                physicsSys->RegisterBodies();
            }

            // Update the render list after adding UIFrames.
            if (auto* renderSys =
                    scene->FindSystem<RenderSystem>())
            {
                renderSys->needsResort = true;
            }

            ++row;

            if (row >= GRID_SIZE)
            {
                finished = true;
                cout << "Finished spawning "
                     << GRID_SIZE * GRID_SIZE
                     << " physics objects.\n";
            }
        }

        // Console FPS reporting avoids drawing text before
        // RenderSystem has finished rendering the scene.
        static double lastReport = GetTime();

        const double now = GetTime();

        if (now - lastReport >= 1.0)
        {
            cout << "FPS: " << GetFPS() << '\n';
            lastReport = now;
        }
    }

    // ---------------------------------------------------------
    // INITIALIZE
    // ---------------------------------------------------------

    bool Game::Init()
    {
        row = 0;
        finished = false;

        world.LoadSettings("");

        world.window.logical_resolution = {
            1000.f,
            1000.f
        };

        world.LoadWindow();

        auto* mainScene = new Scene(&world);

        mainScene->id = HASH_ID("main");
        mainScene->backgroundColor = {
            255, 255, 255, 255
        };

        // System execution order:
        // 0. Scripts (spawn objects)
        // 1. Physics (simulate and update Transform2D)
        // 2. GUI (resolve visual rectangles)
        // 3. Rendering
        mainScene->AddSystem<ScriptSystem>(0);

        auto* physicsSystem =
            mainScene->AddSystem<PhysicsSystem>(1);

        mainScene->AddSystem<GUISystem>(2);
        mainScene->AddSystem<RenderSystem>(3);

        b2World_SetGravity(
            physicsSystem->worldID,
            {0.f, 9.8f}
        );

        // -----------------------------------------------------
        // ROOT UI CANVAS
        // -----------------------------------------------------

        // The scene root is for layout only.
        // Keep it visible for inheritance, but transparent
        // so it cannot cover world-space objects.
        auto* rootFrame =
            mainScene->FindElement<UIFrame>(mainScene->root);

        if (!rootFrame)
        {
            rootFrame = &mainScene->AddElement<UIFrame>(
                mainScene->root
            );
        }

        rootFrame->renderable.tint = {0, 0, 0, 0};
        rootFrame->renderable.isScreenSpace = true;
        rootFrame->renderable.visible = true;
        rootFrame->transform.dirty = true;

        // -----------------------------------------------------
        // CAMERA
        // -----------------------------------------------------

        const Object cameraObject =
            mainScene->AddObject(HASH_ID("camera"));

        Camera2D camera{};

        camera.target = {0.f, 0.f};
        camera.offset = {0.f, 0.f};
        camera.rotation = 0.f;
        camera.zoom = 1.f;

        Script cameraScript{};
        cameraScript.onUpdateFunctions.push_back(HASH_ID("camera_update"));

        mainScene->AddElement<Camera2D>(
            cameraObject,
            camera
        );

        mainScene->AddElement<Script>(
            cameraObject,
            cameraScript
        );

        // -----------------------------------------------------
        // STATIC GROUND
        // -----------------------------------------------------

        const Vector2 resolution =
            world.window.logical_resolution;

        const float groundY = GetGroundY(resolution);

        const float groundWidth = resolution.x * 10;
        const float halfWidth = groundWidth * 0.5f;
        const float halfHeight = GROUND_HEIGHT * 0.5f;

        const Object groundObject =
            mainScene->AddObject(HASH_ID("ground"));

        Physics2D groundPhysics{};

        auto groundBody = b2DefaultBodyDef();

        groundBody.type = b2_staticBody;
        groundBody.position = {
            0,
            groundY + halfHeight
        };

        groundPhysics.bodyDef = groundBody;
        groundPhysics.shapeDef = b2DefaultShapeDef();

        groundPhysics.polygon = b2MakeBox(
            halfWidth,
            halfHeight
        );

        Transform2D groundTransform{};

        UIFrame groundFrame = MakePhysicsFrame(
            groundWidth,
            GROUND_HEIGHT,
            {0, 255, 0, 255}
        );

        groundFrame.renderable.zlayer = 2;

        mainScene->AddElement<Transform2D>(
            groundObject,
            groundTransform
        );

        mainScene->AddElement<Physics2D>(
            groundObject,
            groundPhysics
        );

        mainScene->AddElement<UIFrame>(
            groundObject,
            groundFrame
        );

        // -----------------------------------------------------
        // RUNNER SCRIPT
        // -----------------------------------------------------

        Object scriptObject =
            mainScene->AddObject(HASH_ID("runner"));

        Script script{};
        script.active = true;
        script.initiated = false;

        script.onUpdateFunctions.push_back(
            HASH_ID("runner_update")
        );

        mainScene->AddElement<Script>(
            scriptObject,
            script
        );

        ConnectECSEvent<float>(
            scriptObject,
            HASH("runner_update"),
            RunnerUpdate
        );
        ConnectECSEvent<float>(
            cameraObject,
            HASH("camera_update"),
            CameraUpdate
        );

        // -----------------------------------------------------
        // ACTIVATE SCENE
        // -----------------------------------------------------

        world.mainScene = mainScene;

        // Register the ground body before the simulation.
        physicsSystem->RegisterBodies();

        return true;
    }

    // ---------------------------------------------------------
    // UPDATE
    // ---------------------------------------------------------

    void Game::Update(float delta)
    {
        if (world.mainScene)
        {
            world.mainScene->Update(
                delta,
                world.window.target
            );
        }

        if (WindowShouldClose())
        {
            Shutdown();
        }
    }
}

// -------------------------------------------------------------
// ENTRY POINT
// -------------------------------------------------------------

int main()
{
    Project project = MakeProject(
        json::parse("{}")
    );

    StressTest::Game game(project);

    game.Run();

    return 0;
}
