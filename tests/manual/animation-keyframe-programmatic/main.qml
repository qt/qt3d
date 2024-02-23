// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

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
                    clipData: _animationFactory.clipData
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
