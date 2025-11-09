#include "hub.hpp"
#include "projdata.hpp"
#include <iostream>

#include "json.hpp"

using namespace lapHub;
using namespace lapCore;

std::vector<std::string> windowSubTitles = {
    "For the love of the game.",
    "Don't feed the bugs!",
    "I swear it works.",
    "Mostly harmless!",
    "We ball.",
    "Don't hate the player, hate the game!",
    "Blame the coder.",
    "Lag, errors, and bugs are part of the experience!",
    "Ctrl+Z is your bsf!",
    "Wonky, but loveable."
};

HubApp::HubApp(Project &project) : App(project)
{}

RenderTexture2D target;

void HubApp::Init()
{
    // MOVE THIS TO SEPERATE FUNCTION
    auto settingsJson = nlohmann::json::parse(ReadFileToString("assets/settings.json"));
    auto &windowJson = settingsJson["window"];

    std::string windowMode = windowJson.value("mode", "windowed");
    bool resizable = windowJson.value("resizable", false);
    Vector2 windowRes{
        windowJson["resolution"].at(0),
        windowJson["resolution"].at(1)
    };
    Vector2 logicalRes{
        windowJson["logical-resolution"].at(0),
        windowJson["logical-resolution"].at(1)
    };

    bool vsync = windowJson.value("vsync", false);
    bool inf_fps = windowJson.value("inf-fps", true);

    std::string windowTitle = windowJson.value("title", "");

    InitWindow(windowRes.x, windowRes.y, windowTitle.c_str());
    for (auto& scene : project.scenes) {
        scene->LoadQueuedAssets();
    }

    if (resizable)
        SetWindowState(FLAG_WINDOW_RESIZABLE);

    if (windowMode == "fullscreen")
        SetWindowState(FLAG_FULLSCREEN_MODE);
    
    if (!vsync)
        if (inf_fps)
            SetTargetFPS(-1);
        else
            SetTargetFPS(windowJson["max-fps"].get<int>());
    else
        SetWindowState(FLAG_VSYNC_HINT);

    project.main_scene = project.GetMainScene();
    project.logicalResolution = logicalRes;

    target = LoadRenderTexture(logicalRes.x, logicalRes.y);
    SetTextureWrap(target.texture, TEXTURE_WRAP_CLAMP);

    state = AppState::RUNNING;
}

void HubApp::Update(float deltaTime)
{
    if (WindowShouldClose())
    {
        state = AppState::DEAD;
        return;
    }

    if (IsKeyDown(KEY_F11))
        ToggleFullscreen();

    project.main_scene->Update(deltaTime, SystemDrawOrder::PREDRAW);
}

void HubApp::Draw()
{
    BeginTextureMode(target);
    ClearBackground(WHITE);

    // draw game using logical coordinates here
    project.main_scene->Update(0.0f, SystemDrawOrder::DRAW);

    EndTextureMode();
    ClearBackground(BLACK);

    // Now draw render texture to the screen, scaled and letterboxed
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    float screenAspect = (float)screenW / screenH;
    float targetAspect = (float)project.logicalResolution.x / project.logicalResolution.y;

    int drawWidth, drawHeight;
    int offsetX, offsetY;

    if (screenAspect > targetAspect) {
        // window is wider than logical
        drawHeight = screenH;
        drawWidth = (int)(screenH * targetAspect);
        offsetX = (screenW - drawWidth) / 2;
        offsetY = 0;
    } else {
        // window is taller than logical
        drawWidth = screenW;
        drawHeight = (int)(screenW / targetAspect);
        offsetX = 0;
        offsetY = (screenH - drawHeight) / 2;
    }

    // Draw the render texture to the screen, scaling it
    DrawTexturePro(
        target.texture,
        { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height }, // source rect (flip y)
        { (float)offsetX, (float)offsetY, (float)drawWidth, (float)drawHeight },     // dest rect
        { 0.0f, 0.0f }, // origin
        0.0f,           // rotation
        WHITE
    );

    EndDrawing();

    /*
        TODO:
            OPTIMIZE
    */

    /*EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    float scaleX = (float)GetScreenWidth() / project.windowResolution.x;
    float scaleY = (float)GetScreenHeight() / project.windowResolution.y;
    float scale = fmin(scaleX, scaleY);

    float offsetX = (GetScreenWidth() - project.windowResolution.x * scale) / 2;
    float offsetY = (GetScreenHeight() - project.windowResolution.y * scale) / 2;

    DrawTexturePro(
        project.drawTexture.texture,
        { 0, 0, (float)project.drawTexture.texture.width, -(float)project.drawTexture.texture.height },
        { offsetX, offsetY, project.windowResolution.x * scale, project.windowResolution.y * scale },
        { 0, 0 },
        0.0f,
        WHITE
    );

    EndDrawing();*/

    project.main_scene->Update(0.0f, SystemDrawOrder::POSTDRAW);
}

int main()
{
    Project hubProject = UnpackProject(proj_json);
    std::cout << "got past here.\n";
    HubApp hub(hubProject);

    hub.Init();
    hub.Run();
    hub.Shutdown();

    UnloadRenderTexture(target);

    return 0;
}