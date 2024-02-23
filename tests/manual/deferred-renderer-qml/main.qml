// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id : root

    components : [
        RenderSettings {
            activeFrameGraph: DeferredRenderer {
                id: frameGraph
                camera : sceneEntity.camera
                gBuffer: GBuffer {}
                sceneLayer: sceneEntity.layer
                screenQuadLayer: screenQuadEntity.layer
                debugLayer: debugEntity.layer
            }
            renderPolicy: RenderSettings.Always
        },
        InputSettings {}
    ]

    FirstPersonCameraController { camera: sceneEntity.camera }

    ScreenQuadEntity {
        id: screenQuadEntity
        baseLight: sceneEntity.light
        light1Pos: sceneEntity.light1Pos
        light2Pos: sceneEntity.light2Pos
        light3Pos: sceneEntity.light3Pos
        light4Pos: sceneEntity.light4Pos
    }
    SceneEntity { id: sceneEntity }
    GBufferDebugger { id: debugEntity }
}
