#include "eutil.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace lapCore::EUTIL;
using namespace Other;

std::string Functions::ReadFileToString(const std::string &filePath, bool withPrefix)
{
    std::string fullPath = withPrefix ? GetApplicationDirectory() + filePath : filePath;
    std::ifstream file(fullPath);

    if (!file.is_open())
    {
        std::cerr << "Failed.\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

void Functions::WriteStringToFile(const std::string &filePath, const std::string &data)
{
    std::ofstream file(GetApplicationDirectory() + filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing: " << filePath << "\n";
        return;
    }

    file << data;
    file.close();
}

nlohmann::json Functions::ReadFileToJsonObject(const std::string &filePath, bool withPrefix)
{
    return nlohmann::json::parse(ReadFileToString(filePath, withPrefix));
}

bool Functions::IsPointInViewportSpace(
    Vector2 point,
    Vector2 logicalResolution,
    Camera2D* camera)
{
    if (!camera)
    {
        return point.x >= 0.0f &&
               point.x < logicalResolution.x &&
               point.y >= 0.0f &&
               point.y < logicalResolution.y;
    }

    Vector2 topLeft = GetScreenToWorld2D(
        { 0.0f, 0.0f },
        *camera
    );

    Vector2 bottomRight = GetScreenToWorld2D(
        {
            logicalResolution.x,
            logicalResolution.y
        },
        *camera
    );

    return point.x >= topLeft.x &&
           point.x < bottomRight.x &&
           point.y >= topLeft.y &&
           point.y < bottomRight.y;
}

bool Functions::IsRectangleInViewportSpace(
    Rectangle rect,
    Vector2 logicalResolution,
    Camera2D* camera)
{
    Rectangle viewport =
        GetCameraViewport(logicalResolution, camera);

    rect.x -= (rect.width * 0.5f);
    rect.y -= (rect.height * 0.5f);

    return CheckCollisionRecs(rect, viewport);
}

Rectangle Functions::GetCameraViewport(
    Vector2 logicalResolution,
    Camera2D* camera)
{
    if (!camera)
    {
        return {
            0.0f,
            0.0f,
            logicalResolution.x,
            logicalResolution.y
        };
    }

    float width = logicalResolution.x / camera->zoom;
    float height = logicalResolution.y / camera->zoom;

    float x = camera->target.x -
              camera->offset.x / camera->zoom;

    float y = camera->target.y -
              camera->offset.y / camera->zoom;

    return {
        x,
        y,
        width,
        height
    };
}

Vector2 Functions::GetMouseInViewportSpace(Vector2 logicalResolution)
{
    Vector2 mouse = GetMousePosition();

    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();

    float screenAspect = screenWidth / screenHeight;
    float targetAspect = (float)logicalResolution.x / logicalResolution.y;

    float scale;
    float offsetX = 0;
    float offsetY = 0;
    float drawWidth;
    float drawHeight;

    if (screenAspect > targetAspect)
    {
        // Screen is wider than target — add pillarboxes
        scale = screenHeight / logicalResolution.y;
        drawWidth = logicalResolution.x * scale;
        drawHeight = screenHeight;
        offsetX = (screenWidth - drawWidth) * 0.5f;
    }
    else
    {
        // Screen is taller — add letterboxes
        scale = screenWidth / logicalResolution.x;
        drawWidth = screenWidth;
        drawHeight = logicalResolution.y * scale;
        offsetY = (screenHeight - drawHeight) * 0.5f;
    }

    // Translate mouse to logical coordinate space
    mouse.x = (mouse.x - offsetX) / scale;
    mouse.y = (mouse.y - offsetY) / scale;

    return mouse;
}

Rectangle Functions::UITransformToRect(UITransform transform, Vector2 logicalResolution)
{
    Rectangle rect;

    auto posVec = FrameVectorToVec2(transform.position, logicalResolution);
    auto sizeVec = FrameVectorToVec2(transform.size, logicalResolution);

    rect.x = posVec.x;
    rect.y = posVec.y;
    rect.width = sizeVec.x;
    rect.height = sizeVec.y;

    return rect;
}

Vector2 Functions::FrameVectorToVec2(Functions::FrameVector vector, Vector2 logicalResolution)
{
    Vector2 vec;

    vec.x = vector.scale.x * logicalResolution.x + vector.offset.x;
    vec.y = vector.scale.y * logicalResolution.y + vector.offset.y;

    return vec;
}

void Functions::dbgln(const std::string &message, Other::LogType type)
{
    switch (type)
    {
        case Other::LogType::INFO:
            if (DEBUG_LEVEL == DebugLevel::HIGH)
                std::cout << Console::_BLUE << "[INFO] " << Console::_RESET << message << '\n';
            break;
        case Other::LogType::NOTICE:
            if (DEBUG_LEVEL >= DebugLevel::MEDIUM)
                std::cout << Console::_GREEN << "[NOTICE] " << Console::_RESET << message << '\n';
            break;
        case Other::LogType::WARNING:
            if (DEBUG_LEVEL >= DebugLevel::MEDIUM)
                std::cout << Console::_YELLOW << "[WARNING] " << Console::_RESET << message << '\n';
            break;
        case Other::LogType::ERROR:
            if (DEBUG_LEVEL >= DebugLevel::LOW)
                std::cout << Console::_RED << "[ERROR] " << Console::_RESET << message << '\n';
            break;
    }
}

void lapCore::EUTIL::Functions::dbgln(const std::string &message, const char *colorCode, Other::LogType type)
{
    switch (type)
    {
        case Other::LogType::INFO:
            if (DEBUG_LEVEL == DebugLevel::HIGH)
                std::cout << Console::_BLUE << "[INFO] " << Console::_RESET << colorCode << message << Console::_RESET << '\n';
            break;
        case Other::LogType::NOTICE:
            if (DEBUG_LEVEL >= DebugLevel::MEDIUM)
                std::cout << Console::_GREEN << "[NOTICE] " << Console::_RESET << colorCode << message << Console::_RESET << '\n';
            break;
        case Other::LogType::WARNING:
            if (DEBUG_LEVEL >= DebugLevel::MEDIUM)
                std::cout << Console::_YELLOW << "[WARNING] " << Console::_RESET << colorCode << message << Console::_RESET << '\n';
            break;
        case Other::LogType::ERROR:
            if (DEBUG_LEVEL >= DebugLevel::LOW)
                std::cerr << Console::_RED << "[ERROR] " << Console::_RESET << colorCode << message << Console::_RESET << '\n';
            break;
    }
}
