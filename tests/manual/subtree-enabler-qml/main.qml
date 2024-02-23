// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.14
import Qt3D.Core 2.14
import Qt3D.Render 2.14
import Qt3D.Input 2.14
import Qt3D.Extras 2.14

Entity {
    id: rootNode
    components: [frameGraph]

    FrameGraph {
        id: frameGraph
        leftCamera: camera1
        rightCamera: camera2
    }

    CameraLens {
        id: cameraLens
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane: 0.01
        farPlane: 1000.0
    }

    SimpleCamera {
        id: camera1
        lens: cameraLens
        position: Qt.vector3d(10.0, 1.0, 10.0)
        viewCenter: Qt.vector3d(0.0, 1.0, 0.0)
    }

    SimpleCamera {
        id: camera2
        lens: cameraLens
        position: Qt.vector3d(0.0, 0.0, 5.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    Entity {
        id: sceneRoot
        property real rotationAngle: 0

        SequentialAnimation {
            running: true
            loops: Animation.Infinite
            NumberAnimation { target: sceneRoot; property: "rotationAngle"; to: 360; duration: 4000; }
        }

        Entity {
            components: [
                Transform {
                    rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), -sceneRoot.rotationAngle)
                },
                CylinderMesh { },
                MetalRoughMaterial { }
            ]
        }
    } // sceneRoot
} // rootNode
