// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.14
import Qt3D.Render 2.14

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
            // 1024 x 1024 particles
            // We will launch 1024 local workgroup that will work
            // on 1024 particles each
            workGroupX: 1024; workGroupY: 1; workGroupZ: 1
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

