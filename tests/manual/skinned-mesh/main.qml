// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Animation 2.10
import Qt3D.Extras 2.10
import QtQuick 2.9

DefaultSceneEntity {
    id: scene

    SkinnedPbrEffect {
        id: skinnedPbrEffect
    }

    Timer {
        interval: 2000
        running: true
        repeat: false
        onTriggered: {
            animator1.running = true
            animator2.running = true
            animator3.running = true
            animator4.running = true
            animator5.running = true
        }
    }

    SkinnedEntity {
        id: riggedFigure1
        effect: skinnedPbrEffect
        source: "qrc:/assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
        baseColor: "red"

        components: [
//            ClipAnimator {
//                loops: 3
//                clip: AnimationClipLoader { source: "qrc:/jump.json" }
//                channelMapper: ChannelMapper {
//                    mappings: [
//                        SkeletonChannelMapping { target: riggedFigure.skeleton }
//                    ]
//                }
//            }
            BlendedClipAnimator {
                id: animator1
                loops: 5
                blendTree: ClipBlendValue {
                    clip: AnimationClipLoader { source: "qrc:/jump.json" }
                }
                channelMapper: ChannelMapper {
                    mappings: [
                        SkeletonMapping { skeleton: riggedFigure1.skeleton }
                    ]
                }

                onRunningChanged: console.log("running = " + animator1.running)
            }
        ]
    }

    SkinnedEntity {
        id: riggedFigure2
        effect: skinnedPbrEffect
        source: "qrc:/assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
        baseColor: "purple"
        transform.translation: Qt.vector3d(0.0, 0.0, -1.0);

        components: [
            BlendedClipAnimator {
                id: animator2
                loops: 5
                blendTree: ClipBlendValue {
                    clip: AnimationClipLoader { source: "qrc:/jump.json" }
                }
                channelMapper: ChannelMapper {
                    mappings: [
                        SkeletonMapping { skeleton: riggedFigure2.skeleton }
                    ]
                }

                onRunningChanged: console.log("running = " + animator2.running)
            }
        ]
    }

    SkinnedEntity {
        id: riggedFigure3
        effect: skinnedPbrEffect
        source: "qrc:/assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
        baseColor: "blue"
        transform.translation: Qt.vector3d(0.0, 0.0, -2.0);

        components: [
            BlendedClipAnimator {
                id: animator3
                loops: 5
                blendTree: ClipBlendValue {
                    clip: AnimationClipLoader { source: "qrc:/jump.json" }
                }
                channelMapper: ChannelMapper {
                    mappings: [
                        SkeletonMapping { skeleton: riggedFigure3.skeleton }
                    ]
                }

                onRunningChanged: console.log("running = " + animator3.running)
            }
        ]
    }

    SkinnedEntity {
        id: riggedFigure4
        effect: skinnedPbrEffect
        source: "qrc:/assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
        baseColor: "green"
        transform.translation: Qt.vector3d(0.0, 0.0, -3.0);

        components: [
            BlendedClipAnimator {
                id: animator4
                loops: 5
                blendTree: ClipBlendValue {
                    clip: AnimationClipLoader { source: "qrc:/jump.json" }
                }
                channelMapper: ChannelMapper {
                    mappings: [
                        SkeletonMapping { skeleton: riggedFigure4.skeleton }
                    ]
                }

                onRunningChanged: console.log("running = " + animator4.running)
            }
        ]
    }

    SkinnedEntity {
        id: riggedFigure5
        effect: skinnedPbrEffect
        source: "qrc:/assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
        baseColor: "orange"
        transform.translation: Qt.vector3d(0.0, 0.0, -4.0);

        components: [
            BlendedClipAnimator {
                id: animator5
                loops: 5
                blendTree: ClipBlendValue {
                    clip: AnimationClipLoader { source: "qrc:/jump.json" }
                }
                channelMapper: ChannelMapper {
                    mappings: [
                        SkeletonMapping { skeleton: riggedFigure5.skeleton }
                    ]
                }

//                onRunningChanged: console.log("running = " + running)
            }
        ]
    }

    SkinnedEntity {
        id: riggedSimple
        effect: skinnedPbrEffect
        source: "qrc:/assets/gltf/2.0/RiggedSimple/RiggedSimple.gltf"
        baseColor: "blue"
        transform.scale: 0.05
        transform.translation: Qt.vector3d(0.5, 0.25, 0.0)
        createJointsEnabled: true

        onRootJointChanged: {
            var animation = animationComp.createObject(rootJoint)
            var targetJoint = rootJoint.childJoints[0]
            animation.target = targetJoint
            animation.running = true
        }

        Component {
            id: animationComp
            SequentialAnimation {
                id: sequentialAnimation
                property variant target: null
                property real dz: 30.0
                loops: Animation.Infinite

                NumberAnimation {
                    target: sequentialAnimation.target
                    property: "rotationZ"
                    from: -dz
                    to: dz
                    duration: 600
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    target: sequentialAnimation.target
                    property: "rotationZ"
                    from: dz
                    to: -dz
                    duration: 600
                    easing.type: Easing.OutCubic
                }
            }
        }
    }
}
