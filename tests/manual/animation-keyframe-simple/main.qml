/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Animation 2.9
import Qt3D.Extras 2.0

DefaultSceneEntity {
    id: scene

    Entity {
        id: cube

        components: [
            Transform {
                id: cubeTransform

                onTranslationChanged: console.log("t = " + translation)
            },
            CuboidMesh {
            },
            PhongMaterial {
                id: cubeMaterial
                ambient: Qt.rgba(0.02, 0.02, 0.02, 1.0)
                diffuse: "blue"
                shininess: 50
            },
            ObjectPicker {
                onClicked: animator.running = true
            },
            ClipAnimator {
                id: animator
                loops: 3
                onRunningChanged: console.log("running = " + running)

                clip: AnimationClip {
                    source: "cubeanimation.json"
                    onDurationChanged: console.log("duration = " + duration)
                }

                // By default introspect parent Entity and try
                // to map fcurve groups to properties of QTransform
                // mapping: AutomaticAnimationMapping {}

                // To do more, we can be explicit
                channelMapper: ChannelMapper {
                    mappings: [
                        ChannelMapping { channelName: "Location"; target: cubeTransform; property: "translation" },
                        ChannelMapping { channelName: "Rotation"; target: cubeTransform; property: "rotation" },
                        ChannelMapping { channelName: "Diffuse Color"; target: cubeMaterial; property: "diffuse" }
                    ]
                }
            }
        ]
    }

    Entity {
        id: sphere

        components: [
            Transform {
                id: sphereTransform
                translation: Qt.vector3d(5, 0, 0)
                onTranslationChanged: console.log("t = " + translation)
            },
            SphereMesh {
            },
            PhongMaterial {
                id: sphereMaterial
                ambient: Qt.rgba(0.02, 0.02, 0.02, 1.0)
                diffuse: "red"
                shininess: 50
            },
            ObjectPicker {
                onClicked: blendedAnimator.running = true
            },
            BlendedClipAnimator {
                id: blendedAnimator
                loops: 2

                onRunningChanged: console.log("running = " + running)

                blendTree: LerpBlend {
                    blendFactor: 0.5
                    clips: [
                        AnimationClip {
                            source: "cubeanimation.json"
                            onDurationChanged: console.log("duration = " + duration)
                        },
                        AnimationClip {
                            source: "pulsing-moving-cube.json"
                            onDurationChanged: console.log("duration = " + duration)
                        }]
                }


                // By default introspect parent Entity and try
                // to map fcurve groups to properties of QTransform
                // mapping: AutomaticAnimationMapping {}

                // To do more, we can be explicit
                channelMapper: ChannelMapper {
                    mappings: [
                        ChannelMapping { channelName: "Location"; target: sphereTransform; property: "translation" },
                        ChannelMapping { channelName: "Rotation"; target: sphereTransform; property: "rotation" },
                        ChannelMapping { channelName: "Scaling"; target: sphereTransform; property: "scale3D" },
                        ChannelMapping { channelName: "Diffuse Color"; target: sphereMaterial; property: "diffuse" }
                    ]
                }
            }
        ]
    }

    Entity {
        id: cube2

        components: [
            Transform {
                id: cube2Transform
                translation: Qt.vector3d(2.5, 0, 0)
                onTranslationChanged: console.log("t = " + translation)
            },
            CuboidMesh {
            },
            PhongMaterial {
                id: cube2Material
                ambient: Qt.rgba(0.8, 0.8, 0.8, 1.0)
                diffuse: Qt.rgba(0.7, 0.7, 0.7, 1.0)
                shininess: 50
            },
            ObjectPicker {
                onClicked: blendedAnimator2.running = true
            },
            BlendedClipAnimator {
                id: blendedAnimator2
                loops: 2

                onRunningChanged: console.log("running = " + running)

                blendTree: AdditiveBlend {
                    blendFactor: 0.5
                    clips: [
                        AnimationClip {
                            source: "pulsing-moving-cube.json"
                            onDurationChanged: console.log("duration = " + duration)
                        },
                        AnimationClip {
                            source: "pulsing-cube-additive.json"
                            onDurationChanged: console.log("duration = " + duration)
                        }]
                }

                // By default introspect parent Entity and try
                // to map fcurve groups to properties of QTransform
                // mapping: AutomaticAnimationMapping {}

                // To do more, we can be explicit
                channelMapper: ChannelMapper {
                    mappings: [
                        ChannelMapping { channelName: "Location"; target: cube2Transform; property: "translation" },
                        ChannelMapping { channelName: "Rotation"; target: cube2Transform; property: "rotation" },
                        ChannelMapping { channelName: "Scaling"; target: cube2Transform; property: "scale3D" },
                        ChannelMapping { channelName: "Diffuse Color"; target: cube2Transform; property: "diffuse" }
                    ]
                }
            }
        ]
    }

    Entity {
        id: cube3

        components: [
            Transform {
                id: cube3Transform
                translation: Qt.vector3d(2.5, 0, 2)
                onTranslationChanged: console.log("t = " + translation)
            },
            CuboidMesh {
            },
            PhongMaterial {
                id: cube3Material
                ambient: Qt.rgba(0.8, 0.8, 0.8, 1.0)
                diffuse: "green"
                shininess: 50
            },
            ObjectPicker {
                onClicked: blendedAnimator3.running = true
            },
            BlendedClipAnimator {
                id: blendedAnimator3
                loops: 2

                onRunningChanged: console.log("running = " + running)

                blendTree: LerpBlend {
                    blendFactor: 0.5
                    AdditiveBlend {
                        blendFactor: 0.5
                        clips: [
                            AnimationClip {
                                source: "pulsing-moving-cube.json"
                                onDurationChanged: console.log("duration = " + duration)
                            },
                            AnimationClip {
                                source: "pulsing-cube-additive.json"
                                onDurationChanged: console.log("duration = " + duration)
                            }]
                    }
                    LerpBlend {
                        blendFactor: 0.5
                        clips: [
                            AnimationClip {
                                source: "cubeanimation.json"
                                onDurationChanged: console.log("duration = " + duration)
                            },
                            AnimationClip {
                                source: "pulsing-moving-cube.json"
                                onDurationChanged: console.log("duration = " + duration)
                            }]
                    }
                }

                // By default introspect parent Entity and try
                // to map fcurve groups to properties of QTransform
                // mapping: AutomaticAnimationMapping {}

                // To do more, we can be explicit
                channelMapper: ChannelMapper {
                    mappings: [
                        ChannelMapping { channelName: "Location"; target: cube3Transform; property: "translation" },
                        ChannelMapping { channelName: "Rotation"; target: cube3Transform; property: "rotation" },
                        ChannelMapping { channelName: "Scaling"; target: cube3Transform; property: "scale3D" },
                        ChannelMapping { channelName: "Diffuse Color"; target: cube3Transform; property: "diffuse" }
                    ]
                }
            }
        ]
    }


    camera: Camera {
        position: Qt.vector3d(10, 3, 15)
        viewCenter: Qt.vector3d(2.5, 1, 0)
    }

    OrbitCameraController {
        camera: scene.camera
        linearSpeed: 8
        lookSpeed: 180
    }
}
