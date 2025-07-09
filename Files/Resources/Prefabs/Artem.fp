[
    {
        "AnimatorComponent": {
            "current_state": 0,
            "current_time": 3901.30517578125,
            "parameters": [
                {
                    "first": "DirectionX",
                    "second.value": 225.0
                },
                {
                    "first": "DirectionY",
                    "second.value": -291.0
                }
            ],
            "states": [
                {
                    "animation_points": [
                        {
                            "animation": "6381951605241949636",
                            "direction": [
                                0.0,
                                1.0
                            ]
                        },
                        {
                            "animation": "586694841512290349",
                            "direction": [
                                0.0,
                                -1.0
                            ]
                        },
                        {
                            "animation": "11039116287368755249",
                            "direction": [
                                0.7071067690849304,
                                0.7071067690849304
                            ]
                        },
                        {
                            "animation": "2065295205648881081",
                            "direction": [
                                0.7071068286895752,
                                -0.7071068286895752
                            ]
                        }
                    ],
                    "looping": true,
                    "name": "Walk",
                    "parameter_name_x": "DirectionX",
                    "parameter_name_y": "DirectionY",
                    "type": "BlendTree"
                }
            ],
            "transitions": []
        },
        "CameraComponent": {
            "clear_color": [
                0.0,
                0.0,
                0.0,
                0.0
            ]
        },
        "IDComponent": {
            "id": "6792913373566533148"
        },
        "NativeScriptComponent": {
            "script_data": {
                "bullet": "1374184077757935361",
                "shotgun": "8266231953880164252"
            },
            "script_name": "PlayerController"
        },
        "PhysicsComponent": {
            "bounce_factor": 0.0,
            "is_static": false,
            "is_trigger": false,
            "layer": 0,
            "mass": 1.0,
            "position": [
                0.0,
                -9.5
            ],
            "size": [
                6.0,
                5.5
            ]
        },
        "RelationshipComponent": {
            "children": [
                8266231953880164252
            ],
            "parent": 0
        },
        "SpriteComponent": {
            "flip_x": false,
            "flip_y": false,
            "frame": [
                0.0,
                0.0,
                42.0,
                26.0
            ],
            "texture": "6717621450190303964"
        },
        "TagComponent": {
            "tag": "Artem"
        },
        "TransformComponent": {
            "auto_sort": false,
            "layer": 0,
            "origin": [
                0.0,
                0.0
            ],
            "position": [
                -30.27450942993164,
                24.449993133544922
            ],
            "rotation": 0.0,
            "scale": [
                1.0,
                1.0
            ]
        }
    },
    {
        "AnimatorComponent": {
            "current_state": 0,
            "current_time": 5.612478733062744,
            "parameters": [
                {
                    "first": "IsShooting",
                    "second.value": {
                        "trigger": false
                    }
                },
                {
                    "first": "IsReloading",
                    "second.value": {
                        "trigger": false
                    }
                },
                {
                    "first": "CanT_Shoot",
                    "second.value": {
                        "trigger": false
                    }
                }
            ],
            "states": [
                {
                    "animation_id": "15111747118094725226",
                    "looping": false,
                    "name": "Idle",
                    "type": "AnimationState"
                },
                {
                    "animation_id": "17848275348952744805",
                    "looping": false,
                    "name": "Shoot",
                    "type": "AnimationState"
                },
                {
                    "animation_id": "13019861593271216063",
                    "looping": false,
                    "name": "Reload",
                    "type": "AnimationState"
                }
            ],
            "transitions": [
                {
                    "conditions": [
                        {
                            "condition": 0,
                            "parameter_name": "IsShooting",
                            "value": 1.0
                        },
                        {
                            "condition": 0,
                            "parameter_name": "CanT_Shoot",
                            "value": 0.0
                        }
                    ],
                    "from_state": 0,
                    "to_state": 1
                },
                {
                    "conditions": [
                        {
                            "condition": 0,
                            "parameter_name": "IsShooting",
                            "value": 0.0
                        }
                    ],
                    "from_state": 1,
                    "to_state": 0
                },
                {
                    "conditions": [
                        {
                            "condition": 0,
                            "parameter_name": "IsReloading",
                            "value": 1.0
                        }
                    ],
                    "from_state": 0,
                    "to_state": 2
                },
                {
                    "conditions": [
                        {
                            "condition": 0,
                            "parameter_name": "IsReloading",
                            "value": 0.0
                        }
                    ],
                    "from_state": 2,
                    "to_state": 0
                }
            ]
        },
        "IDComponent": {
            "id": "8266231953880164252"
        },
        "RelationshipComponent": {
            "children": [],
            "parent": 6792913373566533148
        },
        "SpriteComponent": {
            "flip_x": false,
            "flip_y": false,
            "frame": [
                0.0,
                0.0,
                16.0,
                16.0
            ],
            "texture": "7339296026360857254"
        },
        "TagComponent": {
            "tag": "Shotgun"
        },
        "TransformComponent": {
            "auto_sort": false,
            "layer": 1,
            "origin": [
                0.0,
                0.0
            ],
            "position": [
                0.0,
                4.691352367401123
            ],
            "rotation": 52.2889404296875,
            "scale": [
                1.0,
                1.0
            ]
        }
    }
]