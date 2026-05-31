#include "eutil.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

// Known issue: On Windows, windows.h's CloseWindow and ShowCursor conflicts with Raylib's CloseWindow and ShowCursor
// Only known workaround is to go into windows.h and change the names of the functions, which is not ideal.

// Another workaround is the rename Raylib's functions that conflict with windows.h, then recompile the static library.

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
    std::ifstream file(GetApplicationDirectory() + filePath);
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
    std::ofstream file(GetApplicationDirectory() + filePath);
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

Vector2 lapCore::GetMouseInViewportSpace(Vector2 logicalResolution)
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

Rectangle lapCore::UIOriginToRect(UIOrigin origin, Vector2 logicalResolution)
{
    Rectangle rect;

    auto posVec = FrameVectorToVec2(origin.transform.position, logicalResolution);
    auto sizeVec = FrameVectorToVec2(origin.transform.size, logicalResolution);

    rect.x = posVec.x;
    rect.y = posVec.y;
    rect.width = sizeVec.x;
    rect.height = sizeVec.y;

    return rect;
}

Vector2 lapCore::FrameVectorToVec2(lapCore::FrameVector vector, Vector2 logicalResolution)
{
    Vector2 vec;

    vec.x = vector.scale.x * logicalResolution.x + vector.offset.x;
    vec.y = vector.scale.y * logicalResolution.y + vector.offset.y;

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
