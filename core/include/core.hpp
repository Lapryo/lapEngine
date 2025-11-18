#ifndef CORE_HPP
#define CORE_HPP

#include "systems.hpp"
#include "app.hpp"
#include "scriptregistry.hpp"
#include "raylib/rlgl.h"
#include "event.hpp"

namespace lapCore
{
    std::string ReadFileToString(const std::string &filePath);
    Vector2 GetMouseInViewportSpace(int logicalWidth, int logicalHeight);

    Rectangle UIOriginToRect(UIOrigin origin, int logicalWidth, int logicalHeight);
}

#endif