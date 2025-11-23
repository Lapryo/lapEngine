#ifndef CORE_HPP
#define CORE_HPP

#include "systems.hpp"
#include "app.hpp"
#include "scriptregistry.hpp"
#include "event.hpp"

#include "raylib/raylib_namespace.h"
#include "raylib/rlgl_namespace.h"

namespace lapCore
{
    std::string ReadFileToString(const std::string &filePath);
    void WriteStringToFile(const std::string &filePath, const std::string &data);

    rl::Vector2 GetMouseInViewportSpace(int logicalWidth, int logicalHeight);

    rl::Rectangle UIOriginToRect(UIOrigin origin, int logicalWidth, int logicalHeight);
}

#endif