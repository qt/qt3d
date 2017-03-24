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
                translation: Qt.vector3d(5, 0, 0)
                onTranslationChanged: console.log("t = " + translation)
            },
            CuboidMesh {
            },
            PhongMaterial {
                id: cubeMaterial
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

                blendTree: LerpClipBlend {
                    blendFactor: 0.2
                    startClip: ClipBlendValue {
                        clip: AnimationClip { source: "sliding-cube.json" }
                    }
                    endClip: ClipBlendValue {
                        clip: AnimationClip { source: "sliding-pulsing-cube.json" }
                    }
                }

                channelMapper: ChannelMapper {
                    mappings: [
                        ChannelMapping { channelName: "Location"; target: cubeTransform; property: "translation" },
                        ChannelMapping { channelName: "Scale"; target: cubeTransform; property: "scale3D" }
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
