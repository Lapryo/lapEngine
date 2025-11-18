#pragma once
const char proj_json[] = R"json(
{
    "name": "lapEditor",
    "version": "0.2-alpha",
    "scenes":
    [
        {
            "name": "main",
            "assets":
            [

            ],
            "systems":
            [
                {
                    "type": "render",
                    "order": 2
                },
                {
                    "type": "script",
                    "order": 1
                },
                {
                    "type": "gui",
                    "order": 0
                }
            ],
            "objects":
            [
                {
                    "name": "background",
                    "parent": "",
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0, 0, 0],
                                "size": [1, 1, 0, 0],
                                "rotation": 0,
                                "anchor": [0, 0],
                                "renderable":
                                {
                                    "zlayer": 0,
                                    "isScreenSpace": true,
                                    "visible": true,
                                    "tint": [13, 12, 29, 255]
                                }
                            }
                        }
                    ]
                },
                {
                    "name": "sidebar",
                    "parent": "",
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0, 0, 0],
                                "size": [0.2, 1, 0, 0],
                                "rotation": 0,
                                "anchor": [0, 0],
                                "renderable":
                                {
                                    "zlayer": 1,
                                    "isScreenSpace": true,
                                    "visible": true,
                                    "tint": [18, 17, 41, 255]
                                }
                            }
                        },
                        {
                            "type": "bool-attribute",
                            "data":
                            {
                                "name": "opensSelectionMenu",
                                "value": true
                            }
                        }
                    ]
                },
                {
                    "name": "selectionMenu",
                    "parent": "",
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0, 0, 0],
                                "size": [0.15, 0.4, 0, 0],
                                "rotation": 0,
                                "anchor": [0, 0],
                                "renderable":
                                {
                                    "zlayer": 99,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [26, 24, 59, 255]
                                }
                            }
                        },
                        {
                            "type": "UIList",
                            "data":
                            {
                                "scroll-size": [1, 1, 0, 0],
                                "display-size": [1, 0.1, 0, 0],
                                "horizontal-scroll-bar": "right",
                                "vertical-scroll-bar": "bottom",
                                "mask-outside-content": true,
                                "scroll-offset": 0,
                                "scroll-speed": 20
                            }
                        },
                        {
                            "type": "script",
                            "data":
                            {
                                "onCreate": "",
                                "onUpdate": "selectionmenu",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "add-object",
                    "parent": "selectionMenu",
                    "components":
                    [
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "Add Object",
                                "text-size": 15.0,
                                "bounds": [0, 0],
                                "horizontal-alignment": "left",
                                "vertical-alignment": "top",
                                "padding": [0, 0, 0, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 100,
                                    "isScreenSpace": true,
                                    "visible": true,
                                    "tint": [125, 97, 103, 255]
                                }
                            }
                        }
                    ]
                },
                {
                    "name": "fpslabel",
                    "parent": "",
                    "components":
                    [
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "FPS: ___",
                                "size": 10,
                                "bounds": [0, 0],
                                "horizontal-alignment": "left",
                                "vertical-alignment": "top",
                                "padding": [0, 0, 160, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 2,
                                    "isScreenSpace": true,
                                    "visible": true,
                                    "tint": [179, 95, 76, 255]
                                }
                            }
                        },
                        {
                            "type": "script",
                            "data":
                            {
                                "onCreate": "",
                                "onUpdate": "updatefps",
                                "onDestroy": ""
                            }
                        }
                    ]
                }
            ]
        },
        {
            "name": "level-editor",
            "assets":
            [

            ],
            "systems":
            [
                {
                    "type": "render",
                    "order": 1
                },
                {
                    "type": "script",
                    "order": 0
                }
            ],
            "objects":
            [

            ]
        },
        {
            "name": "play-testing",
            "assets":
            [

            ],
            "systems":
            [
                {
                    "type": "render",
                    "order": 1
                },
                {
                    "type": "script",
                    "order": 0
                }
            ],
            "objects":
            [

            ]
        }
    ]
}
)json";