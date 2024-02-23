// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

TechniqueFilter {
    id: root

    // Expose camera to allow user to choose which camera to use for rendering
    property alias camera: cameraSelector.camera
    property alias window: surfaceSelector.surface

    // Expose the layers we'll manage during rendering
    readonly property Layer backgroundLayer: Layer {}
    readonly property Layer waveLayer: Layer {}

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
                id: cameraSelector

                ClearBuffers {
                    buffers: ClearBuffers.ColorDepthBuffer
                    LayerFilter { layers: root.backgroundLayer }
                }

                LayerFilter { layers: root.waveLayer }
            }
        }
    }
}
