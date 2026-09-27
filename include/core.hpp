#pragma once

/*

NOTICE:

A small majority of this code was written by ChatGPT.

I only use AI when I face a a bottleneck in my development process,
and by bottleneck I mean that I know the exact way I want to implement a feature, and I know the full design
of the feature and the systems and yada yada yada, but I run into problems just writing it, or quite frankly,
I'm just too lazy or don't have enough time to do it, and I just wanna get it out and done with.

When I do have the time, I don't use it. When I don't have the time, I still know what to do and what the code itself
does, just can't put it into full step-by-step code with the time I have.

Believe it or not, not everyone has years of experience coding in C++. I started roughly just a year or two ago,
inconsistently making small projects here and there. I come from a background in Lua, which is a much simpler
programming language, in my opinion of course.

I am NOT using AI to *DESIGN* lapEngine for me. All of the code in here I designed myself, asides from the supplemental
libraries of course (raylib, box2d, entt, etc.). Much kudos to those folks, their projects are awesome.

If I could give a rough estimate of how much of this code was written by AI,
I'd say a rough 25-33%.

Below I will give you files that of what I know are either partly or fully AI-written.
I give a star count (*) out of 5 based on how much, estimating, is written by AI.

My plans for the future are to go back into the codebase and completely write it myself.

As far as what I can remember, part of the following had AI code in it:
1. event.hpp - ****
2. reflection.hpp - ****
3. gui_sys.hpp/.cpp - ***
4. input_sys.hpp/.cpp - **
5. map.cpp - **
6. render_sys.hpp/.cpp - *
7. eutil.hpp - *
8. scene.cpp - *

As far as from what I can recall and read, nothing else was written by AI except for small little snippets (suggestions,
and such). Of course I may be wrong.

You can assume the following:
_ = No AI use
* = Light commenting/optimizations (10/90)
** = Light feature implentation, mostly optimizations, commenting (25/75)
*** = Definite feature implementation, heavy optimizations, commenting (50/50)
**** = Heavy feature implementation, heavy optimizations, commenting (67/33)
***** = Fully-written by AI (100/0)

*/

#include "system.hpp"
#include "app.hpp"
#include "event.hpp"

#include "systems.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include "jsonstructs.hpp"

using namespace lapCore;
using namespace EUTIL;
using namespace ELEMENTS;

using namespace ELEMENTS::Render;
using namespace EUTIL::Render;
using namespace ELEMENTS::Render::UI;
using namespace EUTIL::Render::UI;
using namespace ELEMENTS::Other;
using namespace EUTIL::Other;

using namespace ELEMENTS::Physics;

using namespace Functions;