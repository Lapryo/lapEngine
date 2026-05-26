#pragma once

/*
Here is that second option that was mentioned in game.cpp.
I very much recommend reading the documentation for all the syntax for project structure in JSON for lapEngine,
the documentation can be found here: *insert here*
*/

const char proj_data[] = R"json(
{
    "name": "Blank Window",
    "version": "1.0.0",
    "assets": [],
    "scenes": [
        {
            "name": "main",
            "systems": [],
            "instances": []
        }
    ],
    "prefabs": []
}
)json";