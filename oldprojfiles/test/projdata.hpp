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
                "render",
                "script"
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
                                "position": [100,20],
                                "velocity": [0,0],
                                "scale": [1,1],
                                "rotation": 0
                            }
                        },
                        {
                            "name": "rectvisualizer",
                            "type": "rectvisualizer",
                            "data": {
                                "size": [50,50],
                                "tint": [230, 41, 55, 255],
                                "zlayer": 2,
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
                                "size": 100,
                                "color": [255, 0, 0, 255],
                                "zlayer": 3,
                                "isScreenSpace": true
                            }
                        },
                        {
                            "name": "updatefps",
                            "type": "script",
                            "data":
                            {
                                "onCreate": "",
                                "onUpdate": "updatefps",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "maxfpslabel",
                    "components":
                    [
                        {
                            "name": "textlabel",
                            "type": "textlabel",
                            "data": {
                                "text": "MAX FPS: ___",
                                "position": [0, 150],
                                "size": 100,
                                "color": [255, 0, 0, 255],
                                "zlayer": 3,
                                "isScreenSpace": true
                            }
                        },
                        {
                            "name": "updatemaxfps",
                            "type": "script",
                            "data":
                            {
                                "onCreate": "",
                                "onUpdate": "updatemaxfps",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "velocitylabel",
                    "components":
                    [
                        {
                            "name": "textlabel",
                            "type": "textlabel",
                            "data": {
                                "text": "BOX'S Y VEL: ___",
                                "position": [0, 300],
                                "size": 100,
                                "color": [255, 0, 0, 255],
                                "zlayer": 3,
                                "isScreenSpace": true
                            }
                        },
                        {
                            "name": "updatevelocitylabel",
                            "type": "script",
                            "data":
                            {
                                "onCreate": "",
                                "onUpdate": "updatevelocitylabel",
                                "onDestroy": ""
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
                        },
                        {
                            "name": "followbox",
                            "type": "script",
                            "data":
                            {
                                "onCreate": "",
                                "onUpdate": "followbox",
                                "onDestroy": ""
                            }
                        }
                    ]
                }
            ]
        }
    ]
}
)json";
