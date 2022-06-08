// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.1 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    // Render from the mainCamera
    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    BasicCamera {
        id: mainCamera
        position: Qt.vector3d( 0.0, 0.0, 15.0 )
    }

    FirstPersonCameraController { camera: mainCamera }

    WireframeMaterial {
        id: wireframeMaterial
        effect: WireframeEffect {}
        ambient: Qt.rgba( 0.2, 0.0, 0.0, 1.0 )
        diffuse: Qt.rgba( 0.8, 0.0, 0.0, 1.0 )

        QQ2.SequentialAnimation {
            loops: QQ2.Animation.Infinite
            running: true

            QQ2.NumberAnimation {
                target: wireframeMaterial;
                property: "lineWidth";
                duration: 1000;
                from: 0.8
                to: 1.8
            }

            QQ2.NumberAnimation {
                target: wireframeMaterial;
                property: "lineWidth";
                duration: 1000;
                from: 1.8
                to: 0.8
            }

            QQ2.PauseAnimation { duration: 1500 }
        }
    }

    TrefoilKnot {
        id: trefoilKnot
        material: wireframeMaterial
    }
}
