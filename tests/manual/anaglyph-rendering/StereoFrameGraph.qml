// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Viewport {

    property alias leftCamera: leftCameraSelector.camera
    property alias rightCamera: rightCameraSelector.camera
    property alias window: surfaceSelector.surface

    RenderSurfaceSelector {
        id: surfaceSelector

        // ColorMask is reset by default
        // By default reset to the default if not specified
        ClearBuffers {
            buffers: ClearBuffers.ColorDepthBuffer
            NoDraw {} // We just want to clear the buffers
        }

        // Draw with left eye
        CameraSelector {
            id: leftCameraSelector
            RenderStateSet {
                renderStates: [
                    ColorMask { redMasked: true; greenMasked: false; blueMasked: false; alphaMasked: false },
                    DepthTest { depthFunction: DepthTest.Less }
                ]
            }
        }

        // Draw with right eye
        ClearBuffers {
            buffers: ClearBuffers.DepthBuffer
            CameraSelector {
                id: rightCameraSelector
                RenderStateSet {
                    renderStates: [
                        ColorMask { redMasked: false; greenMasked: true; blueMasked: true; alphaMasked: false },
                        DepthTest { depthFunction: DepthTest.Less }
                    ]
                }
            }
        }
    }
}
