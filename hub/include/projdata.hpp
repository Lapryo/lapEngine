#pragma once
const char proj_json[] = R"json(
{
    "name": "lapHub",
    "version": "0.1-alpha",
    "scenes":
    [
        {
            "name": "main",
            "assets":
            [

            ],
            "systems":
            [ 
                "render",
                "script"
            ],
            "objects":
            [
                {
                    "name": "opLabel",
                    "components":
                    [
                        {
                            "name": "transform",
                            "type": "transform2d",
                            "data":
                            {
                                "position": [100, 2000],
                                "velocity": [0,0],
                                "scale": [1,1],
                                "rotation": 0
                            }
                        },
                        {
                            "name": "textlabel",
                            "type": "textlabel",
                            "data": {
                                "text": "Open Project?",
                                "offset": [0, 0],
                                "size": 250,
                                "color": [255, 0, 0, 255],
                                "zlayer": 2,
                                "isScreenSpace": true
                            }
                        },
                        {
                            "name": "hitbox",
                            "type": "rectvisualizer",
                            "data":
                            {
                                "size": [250, 250],
                                "offset": [-30, -20],
                                "tint": [0, 255, 0, 255],
                                "zlayer": 1,
                                "isScreenSpace": true
                            }
                        },
                        {
                            "name": "openproject",
                            "type": "script",
                            "data":
                            {
                                "onCreate": "adjustopenproject",
                                "onUpdate": "openproject",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "background",
                    "components":
                    [
                        {
                            "name": "background",
                            "type": "rectvisualizer",
                            "data":
                            {
                                "size": [6400, 4800],
                                "offset": [0, 0],
                                "tint": [13, 12, 29, 255],
                                "zlayer": 0,
                                "isScreenSpace": true
                            }
                        }
                    ]
                },
                {
                    "name": "fpslabel",
                    "components":
                    [
                        {
                            "name": "fpslabel",
                            "type": "textlabel",
                            "data":
                            {
                                "text": "FPS: ___",
                                "offset": [0, 0],
                                "size": 100,
                                "color": [255, 0, 0, 255],
                                "zlayer": 1,
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
                    "name": "cpLabel",
                    "components":
                    [
                        {
                            "name": "transform",
                            "type": "transform2d",
                            "data":
                            {
                                "position": [4050, 2000],
                                "velocity": [0,0],
                                "scale": [1,1],
                                "rotation": 0
                            }
                        },
                        {
                            "name": "textlabel",
                            "type": "textlabel",
                            "data": {
                                "text": "Create Project?",
                                "offset": [0, 0],
                                "size": 250,
                                "color": [255, 0, 0, 255],
                                "zlayer": 4,
                                "isScreenSpace": true
                            }
                        },
                        {
                            "name": "hitbox",
                            "type": "rectvisualizer",
                            "data":
                            {
                                "size": [250, 250],
                                "offset": [-30, -20],
                                "tint": [0, 255, 0, 255],
                                "zlayer": 3,
                                "isScreenSpace": true
                            }
                        },
                        {
                            "name": "createproject",
                            "type": "script",
                            "data":
                            {
                                "onCreate": "adjustcreateproject",
                                "onUpdate": "createproject",
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
