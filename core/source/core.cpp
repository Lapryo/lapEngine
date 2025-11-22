#include "core.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


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

rl::Vector2 lapCore::GetMouseInViewportSpace(int logicalWidth, int logicalHeight)
{
    rl::Vector2 mouse = rl::GetMousePosition();

    float screenWidth = (float)rl::GetScreenWidth();
    float screenHeight = (float)rl::GetScreenHeight();

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

rl::Rectangle lapCore::UIOriginToRect(UIOrigin origin, int logicalWidth, int logicalHeight)
{
    rl::Rectangle rect;

    rect.x = origin.position.scale.x * logicalWidth + origin.position.offset.x;
    rect.y = origin.position.scale.y * logicalHeight + origin.position.offset.y;

    rect.width = origin.size.scale.x * logicalWidth + origin.size.offset.x;
    rect.height = origin.size.scale.y * logicalHeight + origin.size.offset.y;

    return rect;
}
