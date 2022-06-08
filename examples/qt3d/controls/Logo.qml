// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    readonly property double cameraZ: camera.position.z

    function viewAll() {
        camera.viewAll()
    }
    function viewLogo() {
        camera.viewEntity(logoEntity)
    }
    function setPositionZ(z) {
        camera.position = Qt.vector3d( 0.0, 0.0, z )
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 40
        aspectRatio: 4/3
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 7.5 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                camera: camera
                clearColor: "white"
            }
        }
    ]

    PhongMaterial {
        id: material
        diffuse: Qt.rgba( color_r.value/255, color_g.value/255, color_b.value/255, 1.0 )
        ambient: Qt.rgba( 0.1, 0.1, 0.1, 1.0 )
        shininess: shining.value
    }

    Transform {
        id: logoTransform
        rotation: fromEulerAngles( rotation_x.value, rotation_y.value, rotation_z.value )
    }

    Mesh {
        id: logoMesh
        source: "qrc:/Qt_logo.obj"
    }

    Entity {
        id: logoEntity
        components: [ logoMesh, material, logoTransform ]
    }

    Entity {
        components: [
            PointLight {
                color: "white"
                intensity: 0.6
            },
            Transform {
                translation: Qt.vector3d(0, 0, 10)
            }
        ]
    }
}
