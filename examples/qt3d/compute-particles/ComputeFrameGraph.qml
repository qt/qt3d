// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.9

Viewport {
    property alias camera: selector.camera

    RenderSurfaceSelector {
        id: surfaceSelector

        // Clear Buffer
        ClearBuffers {
            buffers: ClearBuffers.ColorDepthBuffer
            NoDraw {}
        }

        // Compute Pass
        DispatchCompute {
            workGroupX: 50; workGroupY: 1; workGroupZ: 1
            TechniqueFilter {
                matchAll: [
                    FilterKey { name: "type"; value: "compute"}
                ]
            }
        }

        // Draw particles from buffer computed in the Compute Pass
        CameraSelector {
            id: selector
            TechniqueFilter {
                MemoryBarrier { waitFor: MemoryBarrier.VertexAttributeArray }
                matchAll: [
                    FilterKey { name: "type"; value: "draw"}
                ]
            }
        }
    }
}

