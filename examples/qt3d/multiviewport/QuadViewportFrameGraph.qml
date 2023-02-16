// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

RenderSettings {
    id: quadViewportFrameGraph

    property alias topLeftCamera: cameraSelectorTopLeftViewport.camera;
    property alias topRightCamera: cameraSelectorTopRightViewport.camera;
    property alias bottomLeftCamera: cameraSelectorBottomLeftViewport.camera;
    property alias bottomRightCamera: cameraSelectorBottomRightViewport.camera;
    property alias window: surfaceSelector.surface

    activeFrameGraph: RenderSurfaceSelector {
        id: surfaceSelector
        //! [0]
        Viewport {
            id: mainViewport
            normalizedRect: Qt.rect(0, 0, 1, 1)

            ClearBuffers {
                buffers: ClearBuffers.ColorDepthBuffer
                clearColor: Qt.rgba(0.6, 0.6, 0.6, 1.0)
            }

            Viewport {
                id: topLeftViewport
                normalizedRect: Qt.rect(0, 0, 0.5, 0.5)
                CameraSelector { id: cameraSelectorTopLeftViewport }
            }

            Viewport {
                id: topRightViewport
                normalizedRect: Qt.rect(0.5, 0, 0.5, 0.5)
                CameraSelector { id: cameraSelectorTopRightViewport }
            }

            Viewport {
                id: bottomLeftViewport
                normalizedRect: Qt.rect(0, 0.5, 0.5, 0.5)
                CameraSelector { id: cameraSelectorBottomLeftViewport }
            }

            Viewport {
                id: bottomRightViewport
                normalizedRect: Qt.rect(0.5, 0.5, 0.5, 0.5)
                CameraSelector { id: cameraSelectorBottomRightViewport }
            }
        }
        //! [0]
    }
}
