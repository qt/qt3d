/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.4 as QQ2

Entity {
    id: root

    components: FrameGraph {
        StereoFrameGraph {
            id: stereoFrameGraph
            leftCamera: stereoCamera.leftCamera
            rightCamera: stereoCamera.rightCamera
        }
    }

    // Camera
    StereoCamera {
        id: stereoCamera
        property real circleRotation: 0
        readonly property real cameraRadius: obstaclesRepeater.radius - 50
        readonly property vector3d circlePosition: Qt.vector3d(cameraRadius * Math.cos(circleRotation), 0.0, cameraRadius * Math.sin(circleRotation))
        readonly property vector3d tan: circlePosition.crossProduct(Qt.vector3d(0, 1, 0).normalized())
        viewCenter: planeTranslation.translation
        position: circlePosition.plus(Qt.vector3d(0, 45 * Math.sin(circleRotation * 2), 0)).plus(tan.times(-2))

        QQ2.NumberAnimation {
            target: stereoCamera
            property: "circleRotation"
            from: 0; to: Math.PI * 2
            duration: 10000
            loops: QQ2.Animation.Infinite
            running: true
        }
    }

    // Skybox
    SkyboxEntity {
        cameraPosition: stereoCamera.position
        baseName: "qrc:/assets/cubemaps/miramar/miramar"
        extension: ".webp"
    }

    // Cylinder
    Entity {
        property CylinderMesh cylinder: CylinderMesh {
            radius: 1
            length: 3
            rings: 100
            slices: 20
        }
        property Transform transform: Transform {
            Scale { id: cylinderScale; scale: 5 }
            Rotate {
                id: cylinderRotationTheta
                angle: 45
                axis: Qt.vector3d(1, 0, 0)
            }
            Rotate {
                id: cylinderRotationPhi
                angle: 30
                axis: Qt.vector3d(1, 0, 0)
            }
        }
        property Material phong: PhongMaterial {}

        QQ2.ParallelAnimation {
            loops: QQ2.Animation.Infinite
            running: true
            QQ2.SequentialAnimation {
                QQ2.NumberAnimation {
                    target: cylinderScale
                    property: "scale"
                    from: 5; to: 45
                    duration: 2000
                    easing.type: QQ2.Easing.OutInQuad
                }
                QQ2.NumberAnimation {
                    target: cylinderScale
                    property: "scale"
                    from: 45; to: 5
                    duration: 2000
                    easing.type: QQ2.Easing.InOutQuart
                }
            }
            QQ2.NumberAnimation {
                target: cylinderRotationPhi
                property: "angle"
                from: 0; to: 360
                duration: 4000
            }
            QQ2.NumberAnimation {
                target: cylinderRotationTheta
                property: "angle"
                from: 0; to: 720
                duration: 4000
            }
        }

        components: [cylinder, transform, phong]
    }

    // AirPlane
    Entity {
        components: [
            Mesh {
                source: "assets/obj/toyplane.obj"
            },
            Transform {
                Rotate { // roll
                    id: roll
                    axis : Qt.vector3d(1, 0, 0)
                    angle : 30
                }
                Rotate {
                    axis: Qt.vector3d(0, 1, 0)
                    angle: stereoCamera.circleRotation * -2 * 180 / Math.PI + 180
                }
                Translate {
                    id: planeTranslation
                    dx: Math.sin(stereoCamera.circleRotation * -2) * obstaclesRepeater.radius
                    dy: 0
                    dz: Math.cos(stereoCamera.circleRotation * -2) * obstaclesRepeater.radius
                }
            },
            PhongMaterial {
                shininess: 20.0
                diffuse: "#ba1a02" // Inferno Orange
            }
        ]

        QQ2.SequentialAnimation {
            running: true
            loops: QQ2.Animation.Infinite

            QQ2.NumberAnimation {
                target: roll
                property: "angle"
                from: 30; to: 45
                duration: 750
            }
            QQ2.NumberAnimation {
                target: roll
                property: "angle"
                from: 45; to: 25
                duration: 500
            }
            QQ2.NumberAnimation {
                target: roll
                property: "angle"
                from: 25; to: 390
                duration: 800
            }
        }
    }

    // Torus obsctacles
    NodeInstantiator {
        id: obstaclesRepeater
        model: 4
        readonly property real radius: 130.0;
        readonly property real det: 1.0 / model
        delegate: Entity {
            components: [
                TorusMesh {
                    radius: 35
                    minorRadius: 5
                    rings: 100
                    slices: 20
                },
                Transform {
                    id: transform
                    readonly property real angle: Math.PI * 2.0 * index * obstaclesRepeater.det
                    Rotate {
                        angle: transform.angle * 180 / Math.PI
                        axis: Qt.vector3d(0.0, 1.0, 0.0)
                    }
                    Translate {
                        dx: obstaclesRepeater.radius * Math.cos(transform.angle)
                        dz: obstaclesRepeater.radius * Math.sin(transform.angle)
                    }
                },
                PhongMaterial {
                    diffuse: Qt.rgba(Math.abs(Math.cos(transform.angle)), 204 / 255, 75 / 255, 1)
                    specular: "white"
                    shininess: 20.0
                }
            ]
        }
    }
}

