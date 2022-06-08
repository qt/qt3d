// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: rootNode
    components: [quadViewportFrameGraph, inputSettings]

    QuadViewportFrameGraph {
        id: quadViewportFrameGraph
        topLeftCamera: cameraSet.cameras[0]
        topRightCamera: cameraSet.cameras[1]
        bottomLeftCamera: cameraSet.cameras[2]
        bottomRightCamera: cameraSet.cameras[3]
    }

    // Event Source will be set by the Qt3DQuickWindow
    InputSettings { id: inputSettings }

    Entity {
        id: cameraSet
        property var cameras: [camera1, camera2, camera3, camera4]

        CameraLens {
            id: cameraLens
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 45
            aspectRatio: 16/9
            nearPlane: 0.01
            farPlane: 1000.0
        }
        CameraLens {
            id: cameraLens2
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 15
            aspectRatio: 16/9
            nearPlane: 0.01
            farPlane: 1000.0
        }
        CameraLens {
            id: cameraLens3
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 5
            aspectRatio: 16/9
            nearPlane: 0.01
            farPlane: 1000.0
        }

        SimpleCamera {
            id: camera1
            lens: cameraLens2
            position: Qt.vector3d(10.0, 1.0, 10.0)
            viewCenter: Qt.vector3d(0.0, 1.0, 0.0)
        }

        SimpleCamera {
            id: camera2
            lens: cameraLens
            position: Qt.vector3d(0.0, 0.0, 5.0)
            viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
        }

        SimpleCamera {
            id: camera3
            lens: cameraLens2
            position: Qt.vector3d(30.0, 30.0, 20.0)
            viewCenter: Qt.vector3d(0.0, 0.0, -8.0)
        }

        SimpleCamera {
            id: camera4
            lens: cameraLens3
            position: Qt.vector3d(100.0, 0.0, -6.0)
            viewCenter: Qt.vector3d(0.0, 0.0, -6.0)
        }
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
                SceneLoader {
                    source: "qrc:/Gear_scene.dae"
                }
            ]
        }
    } // sceneRoot
} // rootNode
