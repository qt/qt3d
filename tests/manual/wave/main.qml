// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.1 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    KeyboardDevice { id: keyboard1 }

    KeyboardHandler {
        id: input
        sourceDevice: keyboard1
        focus: true

        onTabPressed: {
            background.animateColors = !background.animateColors
        }

        onSpacePressed: {
            wave.wireframe = !wave.wireframe
        }
    }

    RenderSettings {
        id: renderSettings
        activeFrameGraph: WaveForwardRenderer {
            camera: mainCamera
        }
    }

    components: [ renderSettings, input, inputSettings ]

    // Event Source will be set by the Qt3DQuickWindow
    InputSettings { id: inputSettings }

    BasicCamera {
        id: mainCamera
        position: Qt.vector3d( 0.0, 10.0, 25.0 )
    }

    FirstPersonCameraController { camera: mainCamera }

    Background {
        id: background
        layer: renderSettings.activeFrameGraph.backgroundLayer
    }

    Wave {
        id: wave
        layer: renderSettings.activeFrameGraph.waveLayer
    }
}
