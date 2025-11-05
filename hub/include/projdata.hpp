#pragma once
const char proj_json[] = R"json(
{
    "name": "hub",
    "version": "1.0",
    "scenes": [
        {
            "name": "main",
            "systems": [
                "physics"
            ],
            "objects": [
                {
                    "name": "cube",
                    "components": [
                        {
                            "name": "transform",
                            "type": "transform2d",
                            "data": {
                                "position": [0,0],
                                "velocity": [0,-50],
                                "scale": [1,1],
                                "rotation": 0
                            }
                        }
                    ]
                }
            ]
        }
    ]
}
)json";
