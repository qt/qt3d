// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
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
            animator.running = true
        }
    }

    SkinnedEntity {
        id: riggedSimple
        effect: skinnedPbrEffect
        source: "qrc:/assets/gltf/2.0/RiggedSimple/RiggedSimple.gltf"
        baseColor: "blue"
        transform.scale: 0.05

        components: [
            ClipAnimator {
                id: animator
                running: true
                loops: Animation.Infinite
                clip: AnimationClipLoader {
                    source: "qrc:/assets/gltf/2.0/RiggedSimple/RiggedSimple.gltf?animationIndex=1"
                }
                channelMapper: ChannelMapper {
                    mappings: [
                        SkeletonMapping{ skeleton: riggedSimple.skeleton }
                    ]
                }
            }

        ]
    }
}
