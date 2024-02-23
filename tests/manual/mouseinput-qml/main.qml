// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    OrbitCameraController { camera: camera }

    components: [
        RenderSettings {
            ForwardRenderer {
                camera: camera
                clearColor: "black"
            }
            renderPolicy: RenderSettings.Always
        },
        InputSettings {}
    ]

    SphereMesh{
        id: sphereMesh
        radius: 3
    }

    PhongMaterial {
        id: material
        diffuse: "green"
    }

    MouseDevice {
        id: mouseDevice
    }

    Entity {
        id: sphere1

        property real scaleFactor: 1.0

        property Transform transform: Transform {
            scale: sphere1.scaleFactor
                rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
        }

        property MouseHandler mouseHandler : MouseHandler {
            sourceDevice: mouseDevice

            onClicked: {
                //Signal not implemented yet
            }

            onReleased: {
                switch (mouse.button) {
                case Qt.LeftButton:
                    sphere1.scaleFactor += 0.5;
                    break;

                case Qt.RightButton:
                    sphere1.scaleFactor -= 0.5;
                    break;
                }
            }
        }

        components: [sphereMesh, material, transform, mouseHandler]
    }
}
