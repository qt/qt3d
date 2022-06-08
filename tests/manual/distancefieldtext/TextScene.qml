// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.9
import QtQuick 2.0 as QQ2;

Entity {
    id: sceneRoot

    property alias fontFamily : text.font.family
    property alias fontBold : text.font.bold
    property alias fontItalic : text.font.italic

    signal clicked()

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 20.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController {
        camera: camera
        linearSpeed: 0.1 + camera.position.z
    }

    components: [
        RenderSettings {
            activeFrameGraph : ForwardRenderer {
                camera: camera
                clearColor: "black"
            }
        },
        InputSettings {
        }
    ]

    function getChars(n) {
        var s = "";
        for (var i = 0; i < n; i++) {
            s += String.fromCharCode(32+i);
            if (i % 16 == 15)
                s += "\n";
        }
        return s;
    }

    property int strLen : 28

    onClicked: {
        strLen += 1
        if (strLen > 96)
            strLen = 10
    }

    Entity {
        components: [
            Transform {
                id: rot
                translation: Qt.vector3d(-5, -5, 0)
            }
        ]

        QQ2.SequentialAnimation {
            loops: QQ2.Animation.Infinite
            running: true
            QQ2.NumberAnimation {
                target: rot
                property: "rotationY"
                from: 0; to: -80;
                duration: 2000
            }
            QQ2.NumberAnimation {
                target: rot
                property: "rotationY"
                from: -80; to: 80;
                duration: 4000
            }
            QQ2.NumberAnimation {
                target: rot
                property: "rotationY"
                from: 80; to: 0;
                duration: 2000
            }
        }

        Text2DEntity {
            id: text
            font.pointSize: 1
            text: getChars(strLen)
            width: 20
            height: 10
        }
    }
}
