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

## **AI NOTICE**

A small majority of this code was written by AI, mostly ChatGPT.

I only use AI when I face a a bottleneck in my development process.

*What do I mean by bottleneck?*
I mean that I know the way that a feature that I designed should be implemented. However, due to various factors
(such as time constraints, deep complexity, optimizations, *laziness...*), I choose to pass on the work to my
wonderful friend, Mr. GPT.
Basically, just using it to streamline the process a little.

When I do have the time, I don't use it.

Personally, I don't have years of experience coding in C++. I started roughly just a year or two ago, inconsistently
making small projects here and there. I come from a background in Lua (which is why this project is so heavily
inspried by Roblox Studio!), which is a much simpler programming language than C++, in my opinion of course.

I am **NOT** using AI to **DESIGN** lapEngine for me.  
All of the code in here I designed myself, asides from the supplemental libraries of course (raylib, box2d, entt, etc.).  
Much kudos to those folks, their projects are awesome.

Understanding that, one of my plans for the future of this project is to go back into the codebase and completely
revise and rewrite it myself.

Below I will give you files that of what I know are either partly or fully AI-written.  
I give a star count (*) out of 5 based on how much, estimating, is written by AI.

As far as what I can remember, part of the following had AI code in it:
1. event.hpp - ☆☆☆☆
2. reflection.hpp - ☆☆☆☆
3. gui_sys.hpp/.cpp - ☆☆☆
4. input_sys.hpp/.cpp - ☆☆
5. map.cpp - ☆☆
6. render_sys.hpp/.cpp - ☆
7. eutil.hpp - ☆
8. scene.cpp - ☆

As far as from what I can recall and read, nothing else was written by AI except for small little snippets (suggestions, and such).  
Of course I may be wrong.

You can assume the following:  
☆ = Light commenting/optimizations (10/90)  
☆☆ = Light feature implentation, mostly optimizations, commenting (25/75)  
☆☆☆ = Definite feature implementation, heavy optimizations, commenting (50/50)  
☆☆☆☆ = Heavy feature implementation, heavy optimizations, commenting (67/33)  
☆☆☆☆☆ = Fully-written by AI (100/0)  
