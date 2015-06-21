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
import QtQuick 2.0 as QQ2

Entity {
    id: sceneRoot
    property int counter: 0

    components: FrameGraph {
        Viewport {
            rect: Qt.rect(0.0, 0.0, 1.0, 1.0)
            clearColor: "white"
            enabled: counter !== 5

            ClearBuffer {
                buffers : ClearBuffer.ColorDepthBuffer
                enabled: counter !== 6
            }

            CameraSelector {
                camera: basicCamera
                enabled: counter !== 7

                LayerFilter {
                    enabled: counter === 12
                    layers: "scene"
                }
            }
        }
    }

    QQ2.Timer {
        interval: 500
        repeat: true
        running: true
        onTriggered: {
            counter += 1;
            if (counter > 15)
                counter = 0;
        }
    }

    Entity {
        id: basicCamera

        Transform {
            id: basicCameraTransform
            LookAt {
                upVector: Qt.vector3d(0, 1, 0)
                viewCenter: Qt.vector3d(0, 0, 0)
                position: Qt.vector3d(0, 10, -10)
            }
        }
        components: [basicCameraTransform, cameraLens]
    }

    CameraLens {
        id: cameraLens
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        enabled: counter !== 4
    }

    // Sphere
    Entity {
        SphereMesh {
            id: sphereMesh
            enabled: counter !== 0
        }

        PhongMaterial {
            id: material
            ambient: "dodgerblue"
            diffuse: "chartreuse"
            specular: "ghostwhite"
            enabled: counter !== 1
        }

        Layer {
            id: layer
            enabled: counter !== 2
            names: "scene"
        }

        Transform {
            id: transform
            enabled: counter !== 3

            Translate {
                id: translate
            }
        }

        QQ2.SequentialAnimation {
            running: true
            loops: QQ2.Animation.Infinite
            QQ2.NumberAnimation {
                target: translate; property: "dx"
                from: 0; to: 5;
                duration: 550
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: translate; property: "dy"
                from: 0; to: 5;
                duration: 650
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: translate; property: "dz"
                from: 0; to: 5;
                duration: 350
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: translate; property: "dx"
                from: 5; to: 0;
                duration: 550
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: translate; property: "dy"
                from: 5; to: 0;
                duration: 350
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: translate; property: "dz"
                from: 5; to: 0;
                duration: 650
                easing.type: QQ2.Easing.InOutQuad
            }
        }

        components: [sphereMesh, material, layer, transform]
    }

    // Floor
    Entity {
        PlaneMesh {
            id: planeMesh
            width: 25.0
            height: 10
        }

        PhongMaterial {
            id: floorMaterial
            ambient: "lawngreen"
            diffuse: "forestgreen"
            specular: "floralwhite"
            shininess: 0.5
        }

        Layer {
            id: floorLayer
            names: "floor"
        }

        components: [planeMesh, floorMaterial, floorLayer]
    }
}

