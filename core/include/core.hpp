#ifndef CORE_HPP
#define CORE_HPP

#include "systems.hpp"
#include "app.hpp"
#include "scriptregistry.hpp"
#include "raylib/rlgl.h"

namespace lapCore
{
    std::string ReadFileToString(const std::string &filePath);
}

#endif