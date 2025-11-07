#pragma once
const char proj_json[] = R"json(
{
    "name": "hub",
    "version": "1.0",
    "scenes":
    [
        {
            "name": "main",
            "assets":
            [
                {
                    "name": "box",
                    "type": "texture",
                    "path": "assets/textures/box.png"
                }
            ],
            "systems":
            [
                "physics",
                "render"
            ],
            "objects":
            [
                {
                    "name": "cube",
                    "components":
                    [
                        {
                            "name": "transform",
                            "type": "transform2d",
                            "data":
                            {
                                "position": [0,0],
                                "velocity": [0,10],
                                "scale": [1,1],
                                "rotation": 0
                            }
                        },
                        {
                            "name": "rectvisualizer",
                            "type": "rectvisualizer",
                            "data": {
                                "size": [10,10],
                                "tint": [230, 41, 55, 255],
                                "isScreenSpace": false
                            }
                        },
                        {
                            "name": "sprite",
                            "type": "sprite",
                            "data":
                            {
                                "texture": "box",
                                "tint": [255, 255, 255, 255],
                                "zlayer": 1,
                                "isScreenSpace": false
                            }
                        }
                    ]
                },
                {
                    "name": "fpslabel",
                    "components":
                    [
                        {
                            "name": "textlabel",
                            "type": "textlabel",
                            "data": {
                                "text": "FPS: ___",
                                "position": [0, 0],
                                "size": 20,
                                "color": [255, 0, 0, 255],
                                "isScreenSpace": true
                            }
                        }
                    ]
                },
                {
                    "name": "camera",
                    "components":
                    [
                        {
                            "name": "camera",
                            "type": "cam2d",
                            "data":
                            {
                                "offset": [0, 0],
                                "target": [100, 20],
                                "rotation": 0.0,
                                "zoom": 1.0,
                                "exclude": []
                            }
                        }
                    ]
                }
            ]
        }
    ]
}
)json";
