// Copyright (C) 2017 Juan José Casafranca.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.2 as QQ2
import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    Camera {
        id: sceneCamera
        projectionType: CameraLens.PerspectiveProjection
        upVector: Qt.vector3d(0, 1, 0)
        position: Qt.vector3d(0, 0, -800)
        viewCenter: Qt.vector3d(0, 0, 0)
        nearPlane: 0.1
        farPlane: 1000
        fieldOfView: 25
        aspectRatio: 1.33
    }

    components: [
        RenderSettings {
            ComputeFrameGraph {
            }
            // explicitly set RenderingPolicy to AlwaysRender, as changes in the
            // scene won't be reflected in actual Qt scene-graph changes (due to
            // GPU compute calls)
            renderPolicy: RenderSettings.Always
        }
    ]

    function initGraphBuffer() {
        var bufferData = new Uint32Array(1024);
        for (var i = 0; i < 1024; ++i) {
            bufferData[i] = 1;
        }
        return bufferData
    }


    Buffer {
        id: graphBuffer
        objectName: "buffer"
        accessType: Buffer.ReadWrite
        data: initGraphBuffer()
    }

    ComputeMaterial {
        id: computeMaterial
        dataBuffer: graphBuffer
        inputSize: 1024
    }

    Entity {
        id: particleComputeEntity
        readonly property ComputeCommand particlesComputeJob: ComputeCommand {}
        components: [
            particlesComputeJob,
            computeMaterial
        ]
    }
}
