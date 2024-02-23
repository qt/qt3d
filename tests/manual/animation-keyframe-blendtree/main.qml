// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Animation 2.9
import Qt3D.Extras 2.0

DefaultSceneEntity {
    id: scene

    KeyboardDevice { id: kyb }
    KeyboardHandler {
        id: keyboardHandler
        sourceDevice: kyb
        focus: true
        onLeftPressed:blendedAnimator.normalizedTime -= 0.02
        onRightPressed: blendedAnimator.normalizedTime += 0.02
    }

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
                onClicked: {
                    if (blendedAnimator.running == false) {
                        blendedAnimator.running = true;
                    } else {
                        switch (pick.button) {
                        case PickEvent.RightButton:
                            animatorClock.playbackRate *= 2.0;
                            break;
                        case PickEvent.LeftButton:
                            animatorClock.playbackRate /= 2.0;
                            break;
                        default:
                            break;
                        }
                    }
                }
            },
            BlendedClipAnimator {
                id: blendedAnimator
                loops: 3
                clock: Clock {
                    id: animatorClock
                    playbackRate: 0.5
                }

                onRunningChanged: console.log("running = " + running)

                blendTree: LerpClipBlend {
                    blendFactor: 0.8
                    startClip: ClipBlendValue {
                        clip: AnimationClipLoader { source: "sliding-cube.json" }
                    }
                    endClip: ClipBlendValue {
                        clip: AnimationClipLoader { source: "sliding-pulsing-cube.json" }
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
}
