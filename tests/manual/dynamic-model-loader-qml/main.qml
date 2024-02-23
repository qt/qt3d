// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.2 as QQ2
import Qt3D.Core 2.12
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
            activeFrameGraph: ForwardRenderer {
                camera: camera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    QQ2.Timer {
        id: timer
        interval: 1000
        running: true
        repeat: true
        property bool addMore: true
        property bool odd: false

        onTriggered: {
            if (instantiator.model > 10 || instantiator.model < 2)
                addMore = !addMore
            instantiator.model += (addMore ? 1 : -1)
            odd = !odd
        }
    }

    PhongMaterial {
        id: material
        diffuse: "yellow"
    }

    NodeInstantiator {
        id: instantiator
        model: 2
        readonly property real angleStep: 360.0 / Math.max(count, 1)
        EntityLoader {
            components: Transform {
                id: revolutionTransform
                matrix: {
                    var m = Qt.matrix4x4();
                    m.rotate(model.index * instantiator.angleStep, Qt.vector3d(0, 1, 0));
                    m.translate(Qt.vector3d(20, 0, 0));
                    return m;
                }
            }
            source: model.index % 2 === 0 ? "qrc:/CuboidEntity.qml" : "qrc:/SphereEntity.qml"
        }
    }


    EntityLoader {
        sourceComponent: timer.odd ? cylEntityCmp : torusEntityCmp
    }

    QQ2.Component {
        id: cylEntityCmp
        Entity {
            CylinderMesh {
                id: cylMesh
            }
            PhongMaterial {
                id: phong
            }
            components: [cylMesh, phong]
        }

    }

    QQ2.Component {
        id: torusEntityCmp
        Entity {
            TorusMesh {
                id: torusMesh
                radius: 5
                minorRadius: 1
                rings: 100
                slices: 20
            }

            Transform {
                id: torusTransform
                scale3D: Qt.vector3d(1.5, 1, 0.5)
                rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
            }

            components: [ torusMesh, material, torusTransform ]
        }
    }

}
