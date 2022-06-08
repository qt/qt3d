// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.1 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    objectName: "root"

    // Use the renderer configuration specified in ForwardRenderer.qml
    // and render from the mainCamera
    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
                clearColor: "black"
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    BasicCamera {
        id: mainCamera
        position: Qt.vector3d( 0.0, 0.0, 10.0 )
    }

    OrbitCameraController { camera: mainCamera }

    TessellatedWireframeMaterial {
        id: quadMaterial
        ambient: Qt.rgba( 0.2, 0.0, 0.0, 1.0 )
        diffuse: Qt.rgba( 0.8, 0.0, 0.0, 1.0 )

        property real innerLevel : 1.0
        property real outerLevel : 1.0

        innerTessLevel : [innerLevel, innerLevel]
        outerTessLevel : [outerLevel, outerLevel, outerLevel, outerLevel]

        effect: TessellatedWireframeEffect {
            tessellationControlShaderCode: shaderProgram.loadSource("qrc:/shaders/quads.tcs")
            tessellationEvaluationShaderCode: shaderProgram.loadSource("qrc:/shaders/quads.tes")
        }

        QQ2.SequentialAnimation {
            loops: QQ2.Animation.Infinite
            running: true

            QQ2.NumberAnimation {
                target: quadMaterial;
                properties: "innerLevel,outerLevel";
                duration: 1000;
                from: 1.0
                to: 20.0
            }

            QQ2.NumberAnimation {
                target: quadMaterial;
                properties: "innerLevel,outerLevel";
                duration: 1000;
                from: 20.0
                to: 1.0
            }
        }
    }

    TessellatedQuad {
        id: quad
        material: quadMaterial
    }

    // TODO: Add examples for triangle and isoline tessellation modes
}
