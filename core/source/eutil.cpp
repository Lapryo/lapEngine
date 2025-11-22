#include "eutil.hpp"

#include "portable-file-dialogs.h"

// TODO: Known issue: On Windows, windows.h's CloseWindow and ShowCursor conflicts with Raylib's CloseWindow and ShowCursor
// Only known workaround is to go into windows.h and change the names of the functions, which is not ideal.

// Another workaround is the rename Raylib's functions that conflict with windows.h, then recompile the static library.
// Fixed! Put raylib in its own namespace 'rl' and 'rlgl' for the opengl part.

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