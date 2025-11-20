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
                            "type": "attribute",
                            "data":
                            {
                                "name": "opensSelectionMenu",
                                "type": "bool",
                                "value": true
                            }
                        },
                        {
                            "type": "ui-button",
                            "data":
                            {
                                "bounds":
                                {
                                    "position":
                                    {
                                        "scale": [0, 0],
                                        "offset": [0, 0]
                                    },
                                    "size":
                                    {
                                        "scale": [0.2, 1],
                                        "offset": [0, 15]
                                    }
                                },
                                "button-events":
                                [
                                    {
                                        "name": "left-click",
                                        "event": ""
                                    },
                                    {
                                        "name": "right-click",
                                        "event": "open-selection-menu"
                                    },
                                    {
                                        "name": "middle-click",
                                        "event": ""
                                    },
                                    {
                                        "name": "mouse-enter",
                                        "event": ""
                                    },
                                    {
                                        "name": "mouse-leave",
                                        "event": ""
                                    },
                                    {
                                        "name": "mouse-hover",
                                        "event": ""
                                    }
                                ]
                            }
                        },
                        {
                            "type": "script",
                            "data":
                            {
                                "onCreate": "setup-sidebar",
                                "onUpdate": "",
                                "onDestroy": ""
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
                                "onCreate": "setupselectionmenu",
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
                        },
                        {
                            "type": "ui-button",
                            "data":
                            {
                                "bounds":
                                {
                                    "position":
                                    {
                                        "scale": [0, 0],
                                        "offset": [0, 0]
                                    },
                                    "size":
                                    {
                                        "scale": [0.15, 0],
                                        "offset": [0, 15]
                                    }
                                },
                                "button-events":
                                [
                                    {
                                        "name": "left-click",
                                        "event": "add-object"
                                    },
                                    {
                                        "name": "right-click",
                                        "event": ""
                                    },
                                    {
                                        "name": "middle-click",
                                        "event": ""
                                    },
                                    {
                                        "name": "mouse-enter",
                                        "event": ""
                                    },
                                    {
                                        "name": "mouse-leave",
                                        "event": ""
                                    },
                                    {
                                        "name": "mouse-hover",
                                        "event": ""
                                    }
                                ]
                            }
                        },
                        {
                            "type": "script",
                            "data":
                            {
                                "onCreate": "connect-add-object",
                                "onUpdate": "",
                                "onDestroy": ""
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
                                "padding": [0, 0, 0, 0],
                                "position": [0.2, 0, 0, 0],
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