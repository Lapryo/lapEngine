#include "eutil.hpp"

#include "portable-file-dialogs.h"
#include "raylib/raylib.h"

std::string lapCore::FileDialogs::OpenFile(std::vector<std::string> filters)
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
}