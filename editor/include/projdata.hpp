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
            "prefabs":
            [
                {
                    "name": "object-entry-gui",
                    "parent": "sidebar",
                    "child-index": -1,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0, 0, 0],
                                "size": [0.2, 0.05, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 100,
                                    "isScreenSpace": true,
                                    "visible": true,
                                    "tint": [255, 255, 255, 0]
                                }
                            }
                        },
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "Object",
                                "text-size": 15.0,
                                "bounds": [0.2, 0.05],
                                "horizontal-alignment": "left",
                                "vertical-alignment": "top",
                                "padding": [5, 0, 5, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 101,
                                    "isScreenSpace": true,
                                    "visible": true,
                                    "tint": [125, 97, 103, 255]
                                }
                            }
                        }
                    ]
                }
            ],
            "objects":
            [
                {
                    "name": "background",
                    "parent": "",
                    "child-index": -1,
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
                    "name": "topbar",
                    "parent": "",
                    "child-index": -1,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0, 0, 0],
                                "size": [1, 0.05, 0, 0],
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
                            "type": "UIList",
                            "data":
                            {
                                "scroll-size": [1, 1, 0, 0],
                                "display-size": [1, 0.05, 0, 0],
                                "horizontal-scroll-bar": "right",
                                "vertical-scroll-bar": "bottom",
                                "mask-outside-content": true,
                                "scroll-offset": 0,
                                "scroll-speed": 20,
                                "direction": "horizontal"
                            }
                        }
                    ]
                },
                {
                    "name": "file-button",
                    "parent": "topbar",
                    "child-index": 0,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0, 0, 0],
                                "size": [0, 0.05, 50, 0],
                                "renderable":
                                {
                                    "zlayer": 3,
                                    "isScreenSpace": true,
                                    "visible": true,
                                    "tint": [255, 255, 255, 0]
                                }
                            }
                        },
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "File",
                                "text-size": 15.0,
                                "bounds": [50, 30],
                                "horizontal-alignment": "middle",
                                "vertical-alignment": "middle",
                                "padding": [0, 0, 0, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 4,
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
                                "active": true,
                                "bounds":
                                {
                                    "position":
                                    {
                                        "scale": [0, 0],
                                        "offset": [0, 0]
                                    },
                                    "size":
                                    {
                                        "scale": [0, 0.05],
                                        "offset": [50, 0]
                                    }
                                },
                                "button-events":
                                [
                                    {
                                        "name": "left-click",
                                        "event": "open-file-dropdown"
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
                                        "event": "highlight-file-button"
                                    },
                                    {
                                        "name": "mouse-leave",
                                        "event": "unhighlight-file-button"
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
                                "onCreate": "setup-file-button",
                                "onUpdate": "",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "scene-button",
                    "parent": "topbar",
                    "child-index": 1,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0, 0, 0],
                                "size": [0, 0.05, 50, 0],
                                "renderable":
                                {
                                    "zlayer": 3,
                                    "isScreenSpace": true,
                                    "visible": true,
                                    "tint": [255, 255, 255, 0]
                                }
                            }
                        },
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "Scene",
                                "text-size": 15.0,
                                "bounds": [50, 30],
                                "horizontal-alignment": "middle",
                                "vertical-alignment": "middle",
                                "padding": [0, 0, 0, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 4,
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
                                "active": true,
                                "bounds":
                                {
                                    "position":
                                    {
                                        "scale": [0, 0],
                                        "offset": [0, 0]
                                    },
                                    "size":
                                    {
                                        "scale": [0, 0.05],
                                        "offset": [50, 0]
                                    }
                                },
                                "button-events":
                                [
                                    {
                                        "name": "left-click",
                                        "event": "open-scene-dropdown"
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
                                        "event": "highlight-scene-button"
                                    },
                                    {
                                        "name": "mouse-leave",
                                        "event": "unhighlight-scene-button"
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
                                "onCreate": "setup-scene-button",
                                "onUpdate": "",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "file-dropdown",
                    "parent": "",
                    "child-index": -1,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0.05, 0, 0],
                                "size": [0.15, 0.2, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 4,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [26, 24, 59, 255]
                                }
                            }
                        },
                        {
                            "type": "ui-button",
                            "data":
                            {
                                "active": false,
                                "bounds":
                                {
                                    "position":
                                    {
                                        "scale": [0, 0.05],
                                        "offset": [0, 0]
                                    },
                                    "size":
                                    {
                                        "scale": [0.15, 0.2],
                                        "offset": [0, 0]
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
                                        "event": "close-file-dropdown"
                                    },
                                    {
                                        "name": "mouse-hover",
                                        "event": ""
                                    }
                                ]
                            }
                        },
                        {
                            "type": "UIList",
                            "data":
                            {
                                "scroll-size": [1, 1, 0, 0],
                                "display-size": [1, 0.2, 0, 0],
                                "horizontal-scroll-bar": "right",
                                "vertical-scroll-bar": "bottom",
                                "mask-outside-content": true,
                                "scroll-offset": 0,
                                "scroll-speed": 20,
                                "direction": "vertical"
                            }
                        },
                        {
                            "type": "script",
                            "data":
                            {
                                "onCreate": "setup-file-dropdown",
                                "onUpdate": "",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "new-project-button",
                    "parent": "file-dropdown",
                    "child-index": 0,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0.05, 0, 0],
                                "size": [0.15, 0, 0, 15],
                                "renderable":
                                {
                                    "zlayer": 6,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [255, 255, 255, 0]
                                }
                            }
                        },
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "New Project",
                                "text-size": 15.0,
                                "bounds": [0, 0],
                                "horizontal-alignment": "left",
                                "vertical-alignment": "top",
                                "padding": [0, 0, 0, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 7,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [125, 97, 103, 255]
                                }
                            }
                        },
                        {
                            "type": "ui-button",
                            "data":
                            {
                                "active": false,
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
                                        "event": "new-project"
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
                                        "event": "highlight-new-project-button"
                                    },
                                    {
                                        "name": "mouse-leave",
                                        "event": "unhighlight-new-project-button"
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
                                "onCreate": "setup-new-project-button",
                                "onUpdate": "",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "open-project-button",
                    "parent": "file-dropdown",
                    "child-index": 1,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0.05, 0, 0],
                                "size": [0.15, 0, 0, 15],
                                "renderable":
                                {
                                    "zlayer": 6,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [255, 255, 255, 0]
                                }
                            }
                        },
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "Open Project",
                                "text-size": 15.0,
                                "bounds": [0, 0],
                                "horizontal-alignment": "left",
                                "vertical-alignment": "top",
                                "padding": [0, 0, 0, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 7,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [125, 97, 103, 255]
                                }
                            }
                        },
                        {
                            "type": "ui-button",
                            "data":
                            {
                                "active": false,
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
                                        "event": "open-project"
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
                                        "event": "highlight-open-project-button"
                                    },
                                    {
                                        "name": "mouse-leave",
                                        "event": "unhighlight-open-project-button"
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
                                "onCreate": "setup-open-project-button",
                                "onUpdate": "",
                                "onDestroy": ""
                            }
                        }
                    ]
                },
                {
                    "name": "save-project-button",
                    "parent": "file-dropdown",
                    "child-index": 2,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0.05, 0, 0],
                                "size": [0.15, 0, 0, 15],
                                "renderable":
                                {
                                    "zlayer": 7,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [255, 255, 255, 0]
                                }
                            }
                        },
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "Save Project",
                                "text-size": 15.0,
                                "bounds": [0, 0],
                                "horizontal-alignment": "left",
                                "vertical-alignment": "top",
                                "padding": [0, 0, 0, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 8,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [125, 97, 103, 255]
                                }
                            }
                        },
                        {
                            "type": "ui-button",
                            "data":
                            {
                                "active": false,
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
                                        "event": "save-project"
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
                                        "event": "highlight-save-project-button"
                                    },
                                    {
                                        "name": "mouse-leave",
                                        "event": "unhighlight-save-project-button"
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
                                "onCreate": "setup-save-project-button",
                                "onUpdate": "",
                                "onDestroy": ""
                            }
                        },
                        {
                            "type": "attribute",
                            "data":
                            {
                                "name": "uses-ui-list-visiblity",
                                "type": "bool",
                                "value": false
                            }
                        }
                    ]
                },
                {
                    "name": "close-project-button",
                    "parent": "file-dropdown",
                    "child-index": 3,
                    "components":
                    [
                        {
                            "type": "frame",
                            "data":
                            {
                                "position": [0, 0.05, 0, 0],
                                "size": [0.15, 0, 0, 15],
                                "renderable":
                                {
                                    "zlayer": 50,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [255, 255, 255, 0]
                                }
                            }
                        },
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "Close Project",
                                "text-size": 15.0,
                                "bounds": [0, 0],
                                "horizontal-alignment": "left",
                                "vertical-alignment": "top",
                                "padding": [0, 0, 0, 0],
                                "position": [0, 0, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 51,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [125, 97, 103, 255]
                                }
                            }
                        },
                        {
                            "type": "ui-button",
                            "data":
                            {
                                "active": false,
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
                                        "event": "close-project"
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
                                        "event": "highlight-close-project-button"
                                    },
                                    {
                                        "name": "mouse-leave",
                                        "event": "unhighlight-close-project-button"
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
                                "onCreate": "setup-close-project-button",
                                "onUpdate": "",
                                "onDestroy": ""
                            }
                        },
                        {
                            "type": "attribute",
                            "data":
                            {
                                "name": "uses-ui-list-visiblity",
                                "type": "bool",
                                "value": false
                            }
                        }
                    ]
                },
                {
                    "name": "sidebar",
                    "parent": "",
                    "child-index": -1,
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
                                    "zlayer": 2,
                                    "isScreenSpace": true,
                                    "visible": false,
                                    "tint": [18, 17, 41, 255]
                                }
                            }
                        },
                        {
                            "type": "ui-button",
                            "data":
                            {
                                "active": false,
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
                                "scroll-speed": 20,
                                "direction": "vertical"
                            }
                        }
                    ]
                },
                {
                    "name": "selectionMenu",
                    "parent": "",
                    "child-index": -1,
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
                                "scroll-speed": 20,
                                "direction": "vertical"
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
                    "child-index": 0,
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
                                "active": false,
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
                    "child-index": -1,
                    "components":
                    [
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "FPS: ___",
                                "text-size": 10,
                                "bounds": [0, 0],
                                "horizontal-alignment": "left",
                                "vertical-alignment": "top",
                                "padding": [0, 0, 0, 0],
                                "position": [0.2, 0.05, 0, 0],
                                "size": [0, 0, 0, 0],
                                "renderable":
                                {
                                    "zlayer": 2,
                                    "isScreenSpace": true,
                                    "visible": false,
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