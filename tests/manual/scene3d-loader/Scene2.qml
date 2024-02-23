// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import QtQuick.Scene3D 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Scene3D {
    focus: true
    aspects: "input"

    Entity {
        id: sceneRoot

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

        RenderSettings {
            id : renderSettings
            activeFrameGraph : ForwardRenderer {
                camera: camera
                clearColor: "black"
            }
        }

        components: [renderSettings]

        TorusMesh {
            id: torusMesh
            radius: 5
            minorRadius: 1
            rings: 100
            slices: 20
        }

        Transform {
            id: transform
            scale3D: Qt.vector3d(1.5, 1, 0.5)
            rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
        }

        PhongMaterial {
            id: material
            diffuse: "red"
        }

        Entity {
            id: mainEntity
            objectName: "mainEntity"
            components: [ torusMesh, material, transform ]
        }
    }
}
