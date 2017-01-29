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
import Qt3D.Animation 2.2
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
