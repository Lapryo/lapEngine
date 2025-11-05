#pragma once
const char proj_json[] = R"json(
{
    "name": "hub",
    "version": "1.0",
    "scenes": [
        {
            "name": "main",
            "systems": [
                "physics",
                "render"
            ],
            "objects": [
                {
                    "name": "cube",
                    "components": [
                        {
                            "name": "transform",
                            "type": "transform2d",
                            "data": {
                                "position": [100,20],
                                "velocity": [0,10],
                                "scale": [1,1],
                                "rotation": 0
                            }
                        },
                        {
                            "name": "rectvisualizer",
                            "type": "rectvisualizer",
                            "data": {
                                "size": [100,100],
                                "color": "red"
                            }
                        }
                    ]
                }
            ]
        }
    ]
}
)json";
