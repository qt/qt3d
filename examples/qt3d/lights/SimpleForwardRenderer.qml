// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

TechniqueFilter {
    // Expose camera to allow user to choose which camera to use for rendering
    property alias camera: cameraSelector.camera
    property alias clearColor: clearBuffer.clearColor
    property alias viewportRect: viewport.normalizedRect
    property alias window: surfaceSelector.surface
    property alias externalRenderTargetSize: surfaceSelector.externalRenderTargetSize
    property alias frustumCulling: frustumCulling.enabled

    // Select the forward rendering Technique of any used Effect
    matchAll: [ FilterKey { name: "renderingStyle"; value: "forward" } ]

    RenderSurfaceSelector {
        id: surfaceSelector

        // Use the whole viewport
        Viewport {
            id: viewport
            normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

            // Use the specified camera
            CameraSelector {
                id : cameraSelector
                FrustumCulling {
                    id: frustumCulling
                    ClearBuffers {
                        id: clearBuffer
                        clearColor: "white"
                        buffers : ClearBuffers.ColorDepthBuffer
                    }
                }
            }
        }
    }
}
