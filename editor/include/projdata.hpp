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
                    "order": 1
                },
                {
                    "type": "script",
                    "order": 0
                }
            ],
            "objects":
            [
                {
                    "name": "background",
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
                    "name": "fpslabel",
                    "components":
                    [
                        {
                            "type": "textlabel",
                            "data":
                            {
                                "text": "FPS: ___",
                                "size": 20,
                                "bounds": [800, 30],
                                "horizontal-alignment": "middle",
                                "vertical-alignment": "middle",
                                "renderable":
                                {
                                    "zlayer": 1,
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
        }
    ]
}
)json";
