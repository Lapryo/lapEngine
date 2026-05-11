```text
                                                                                                                   
.---.                                                                                                              
|   |          _________   _...._            __.....__        _..._            .--.   _..._         __.....__      
|   |          \        |.'      '-.     .-''         '.    .'     '.   .--./) |__| .'     '.   .-''         '.    
|   |           \        .'```'.    '.  /     .-''"'-.  `. .   .-.   . /.''\\  .--..   .-.   . /     .-''"'-.  `.  
|   |    __      \      |       \     \/     /________\   \|  '   '  || |  | | |  ||  '   '  |/     /________\   \ 
|   | .:--.'.     |     |        |    ||                  ||  |   |  | \`-' /  |  ||  |   |  ||                  | 
|   |/ |   \ |    |      \      /    . \    .-------------'|  |   |  | /("'`   |  ||  |   |  |\    .-------------' 
|   |`" __ | |    |     |\`'-.-'   .'   \    '-.____...---.|  |   |  | \ '---. |  ||  |   |  | \    '-.____...---. 
|   | .'.''| |    |     | '-....-'`      `.             .' |  |   |  |  /'""'.\|__||  |   |  |  `.             .'  
'---'/ /   | |_  .'     '.                 `''-...... -'   |  |   |  | ||     ||   |  |   |  |    `''-...... -'    
     \ \._,\ '/'-----------'                               |  |   |  | \'. __//    |  |   |  |                     
      `--'  `"                                             '--'   '--'  `'---'     '--'   '--'                          
```
# **OVERVIEW**
### *lapEngine* is a light-weight game engine written in C++, built with a focus on performance, simplicity, and cross-platform support.
Inspired by the game engine, *Roblox Studio*, it emphasizes rapid iteration and ease of use while remaining fully open-source and flexible for developers who want lower-level control without unnecessary complexity.

## **KEY FEATURES**
- C++ Core - High performance, native engine architecture
- Entity Component System (ENTT) - Powered by EnTT
- Easy To Use Rendering & Windowing - Via raylib
- Cross-Platform - Windows, Linux, macOS-friendly
- Simple, Clean API - Designed for fast prototyping
- Open & Extensible - No vendor lock-in
- Easy To Read JSON Project Files - Makes quick edits after export easy

## **lapEditor**
A standalone editor for the game engine can be found in my repositories or you can follow the link here  →  [lapEditor](https://github.com/Lapryo/lapEditor "Standalone lapEngine Editor")

## **DEPENDENCIES**
- C++17 or newer
- CMake (recommended)

# **GETTING STARTED**
## **RELEASES**
You can grab the release binaries for Windows, Linux, or macOS on this repository page, which includes the include folder and the built
## **BUILDING**
```
git clone https://github.com/Lapryo/lapEngine.git  
cd lapEngine
```
```
mkdir build
cd build
cmake ..
cmake --build .
```
## **PROJECT STRUCTURE**
```
/source        → core engine code
/include       → engine headers (EnTT, raylib, core, etc)
/examples      → demo projects
```

# **GOALS & STATUS**
This project is currently *active in development*.<br>
APIs may change, and features are added iteratively.<br><br>
Through development the main goals are:
- Keep the engine **approachable**, not bloated
- Prioritize clarity
- Enable developers to understand *how* things work, and not just use them
- Stay fast, small, and hackable

## **LICENSE**
This project is licensed under the MIT License.<br>
See the LICENSE.md file for more details.
