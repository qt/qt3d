// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

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
            clearColor: "white"
            buffers: ClearBuffers.ColorDepthBuffer
            NoDraw {} // We just want to clear the buffers
        }

        // Draw with left eye
        Viewport {
            id: leftViewport
            normalizedRect: Qt.rect(0, 0, .5, 1)

            CameraSelector {
                id: leftCameraSelector
            }
        }

        // Draw with right eye
        Viewport {
            id: rightViewport
            normalizedRect: Qt.rect(.5, 0, .5, 1)

            CameraSelector {
                id: rightCameraSelector
            }
        }
    }
}
