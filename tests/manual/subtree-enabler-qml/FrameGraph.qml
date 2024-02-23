// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.14
import Qt3D.Render 2.14
import QtQuick 2.14 as QQ2

RenderSettings {
    id: frameGraph

    property alias leftCamera: cameraSelectorLeftViewport.camera;
    property alias rightCamera: cameraSelectorRightViewport.camera;
    property alias window: surfaceSelector.surface

    activeFrameGraph: RenderSurfaceSelector {
        id: surfaceSelector

        Viewport {
            id: mainViewport
            normalizedRect: Qt.rect(0, 0, 1, 1)

            ClearBuffers {
                buffers: ClearBuffers.ColorDepthBuffer
                clearColor: Qt.rgba(0.6, 0.6, 0.6, 1.0)
                NoDraw {}
            }

            Viewport {
                id: leftViewport
                normalizedRect: Qt.rect(0, 0, 0.5, 0.5)
                CameraSelector { id: cameraSelectorLeftViewport }
            }

            SubtreeEnabler {
                id: enabler
                enabled: true

                QQ2.Timer {
                    running: true
                    repeat: true
                    interval: 1000
                    onTriggered: enabler.enabled = !enabler.enabled
                }

                Viewport {
                    id: rightViewport
                    normalizedRect: Qt.rect(0.5, 0.5, 0.5, 0.5)
                    CameraSelector { id: cameraSelectorRightViewport }
                }
            }
        }
    }
}
