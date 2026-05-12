#include "eutil.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

// Known issue: On Windows, windows.h's CloseWindow and ShowCursor conflicts with Raylib's CloseWindow and ShowCursor
// Only known workaround is to go into windows.h and change the names of the functions, which is not ideal.

// Another workaround is the rename Raylib's functions that conflict with windows.h, then recompile the static library.
// Fixed! Put raylib in its own namespace 'rl' and 'rlgl' for the opengl part.

/*std::string lapCore::FileDialogs::OpenFile(std::vector<std::string> filters)
{
    pfd::open_file ofd("Open File", "", filters, pfd::opt::none);
    auto results = ofd.result();
    if (!results.empty())
        return results[0];
    return "";
}

std::string lapCore::FileDialogs::SaveFile(std::vector<std::string> filters)
{
    pfd::save_file sfd("Save File", "", filters, pfd::opt::none);
    return sfd.result();
}*/

std::string lapCore::ReadFileToString(const std::string &filePath)
{
    std::ifstream file(filePath);
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

void lapCore::WriteStringToFile(const std::string &filePath, const std::string &data)
{
    std::ofstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing: " << filePath << "\n";
        return;
    }

    file << data;
    file.close();
}

nlohmann::json_abi_v3_12_0::json lapCore::ReadFileToJsonObject(const std::string &filePath)
{
    return nlohmann::json::parse(ReadFileToString(filePath));
}

Vector2 lapCore::GetMouseInViewportSpace(int logicalWidth, int logicalHeight)
{
    Vector2 mouse = GetMousePosition();

    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();

    float screenAspect = screenWidth / screenHeight;
    float targetAspect = (float)logicalWidth / logicalHeight;

    float scale;
    float offsetX = 0;
    float offsetY = 0;
    float drawWidth;
    float drawHeight;

    if (screenAspect > targetAspect)
    {
        // Screen is wider than target — add pillarboxes
        scale = screenHeight / logicalHeight;
        drawWidth = logicalWidth * scale;
        drawHeight = screenHeight;
        offsetX = (screenWidth - drawWidth) * 0.5f;
    }
    else
    {
        // Screen is taller — add letterboxes
        scale = screenWidth / logicalWidth;
        drawWidth = screenWidth;
        drawHeight = logicalHeight * scale;
        offsetY = (screenHeight - drawHeight) * 0.5f;
    }

    // Translate mouse to logical coordinate space
    mouse.x = (mouse.x - offsetX) / scale;
    mouse.y = (mouse.y - offsetY) / scale;

    return mouse;
}

Rectangle lapCore::UIOriginToRect(UIOrigin origin, int logicalWidth, int logicalHeight)
{
    Rectangle rect;

    rect.x = origin.position.scale.x * logicalWidth + origin.position.offset.x;
    rect.y = origin.position.scale.y * logicalHeight + origin.position.offset.y;

    rect.width = origin.size.scale.x * logicalWidth + origin.size.offset.x;
    rect.height = origin.size.scale.y * logicalHeight + origin.size.offset.y;

    return rect;
}

Vector2 lapCore::FrameVectorToVec2(lapCore::FrameVector vector, int logicalWidth, int logicalHeight)
{
    Vector2 vec;

    vec.x = vector.scale.x * logicalWidth + vector.offset.x;
    vec.y = vector.scale.y * logicalHeight + vector.offset.y;

    return vec;
}

void lapCore::dbgln(const std::string &message, LogType type)
{
    switch (type)
    {
        case LogType::INFO:
            std::cout << "[INFO] " << message << "\n";
            break;
        case LogType::NOTICE:
            std::cout << "[NOTICE] " << message << "\n";
            break;
        case LogType::WARNING:
            std::cout << "[WARNING] " << message << "\n";
            break;
        case LogType::ERROR:
            std::cerr << "[ERROR] " << message << "\n";
            break;
    }
}

// Default constructors for structs
lapCore::Renderable::Renderable()
    : zlayer(0), isScreenSpace(false), visible(true), tint(RAYWHITE), usesUIListVisiblity(false)
{
    space = {0, 0, 0, 0};
}

lapCore::Alignment::Alignment()
    : horizontal(HorizontalAlignment::LEFT), vertical(VerticalAlignment::TOP)
{
}

lapCore::FrameVector::FrameVector()
    : scale({0, 0}), offset({0, 0})
{
}

lapCore::Padding::Padding()
    : top(0), bottom(0), left(0), right(0)
{
}

lapCore::UIOrigin::UIOrigin()
    : position(FrameVector()), size(FrameVector())
{
}