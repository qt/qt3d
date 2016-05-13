/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id : root

    GBuffer {
        id : gBuffer
    }

    components : [
        RenderSettings {
            activeFrameGraph: DeferredRenderer {
                id: frameGraph
                camera : camera
                gBuffer: gBuffer
            }

            renderPolicy: RenderSettings.Always
        },
        InputSettings {}
    ]

    FirstPersonCameraController { camera: camera }

    Entity {
        id : screenQuadEntity
        PlaneMesh {
            id: mesh
            width: 2.0
            height: 2.0
            meshResolution: Qt.size(2, 2)
        }

        Transform { // We rotate the plane so that it faces us
            id: transform
            rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 90)
        }

        Material {
            id: material
            parameters : [
                Parameter { name: "color"; value : gBuffer.color },
                Parameter { name: "position"; value : gBuffer.position },
                Parameter { name: "normal"; value : gBuffer.normal },
                Parameter { name: "winSize"; value : Qt.size(1024, 1024) }
            ]
            effect : FinalEffect {}
        }

        components : [ frameGraph.screenQuadLayer, mesh, transform, material ]
    }

    Entity {
        id : sceneEntity

        property PointLight light: PointLight {
            color : "white"
            intensity : 4.0
            QQ2.ColorAnimation on color { from: "white"; to: "blue"; duration: 4000; loops: 2 }
            QQ2.NumberAnimation on intensity { from: 0; to: 5.0; duration: 1000; loops: QQ2.Animation.Infinite }
        }

        components: [ sceneEntity.light ]

        Camera {
            id: camera
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 45
            aspectRatio: 16/9
            nearPlane : 0.01
            farPlane : 1000.0
            position: Qt.vector3d( 0.0, 0.0, -25.0 )
            upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
            viewCenter: Qt.vector3d( 0.0, 0.0, 10.0 )
        }

        SphereMesh {
            id : sphereMesh
            rings: 50
            slices: 100
        }

        SceneEffect {
            id : sceneMaterialEffect
        }

        Entity {
            id : sphere1

            property Material material : Material {
                effect : sceneMaterialEffect
                parameters : Parameter { name : "meshColor"; value : "dodgerblue" }
            }

            property Transform transform: Transform {
                id: sphere1Transform
                property real x: -10.0
                translation: Qt.vector3d(x, 0, 5)
            }

            QQ2.SequentialAnimation {
                loops: QQ2.Animation.Infinite
                running: false
                QQ2.NumberAnimation { target: sphere1Transform; property: "x"; to: 6; duration: 2000 }
                QQ2.NumberAnimation { target: sphere1Transform; property: "x"; to: -10; duration: 2000 }
            }

            property PointLight light : PointLight {
                color : "green"
                intensity : 5.0
            }

            components : [
                sphereMesh,
                material,
                sphere1.transform,
                sphere1.light,
                frameGraph.sceneLayer
            ]
        }

        Entity {
            id : sphere2

            property Material material : Material {
                effect : sceneMaterialEffect
                parameters : Parameter { name : "meshColor"; value : "green" }
            }

            property PointLight light : PointLight {
                color : "orange"
                intensity : 2.0
            }

            property Transform transform: Transform {
                translation: Qt.vector3d(5, 0, 5)
            }

            components : [
                sphereMesh,
                sphere2.transform,
                material,
                sphere2.light,
                frameGraph.sceneLayer
            ]
        }

        Entity {
            id: light3
            property PointLight light : PointLight {
                color : "white"
                intensity : 2.0
            }

            property Material material : Material {
                effect : sceneMaterialEffect
                parameters : Parameter { name : "meshColor"; value : "red" }
            }

            property Transform transform: Transform {
                id: light3Transform
                property real y: 2.0
                translation: Qt.vector3d(2, y, 7)
            }

            QQ2.SequentialAnimation {
                loops: QQ2.Animation.Infinite
                running: true
                QQ2.NumberAnimation { target: light3Transform; property: "y"; to: 6; duration: 1000; easing.type: QQ2.Easing.InOutQuad }
                QQ2.NumberAnimation { target: light3Transform; property: "y"; to: -6; duration: 1000; easing.type: QQ2.Easing.InOutQuint }
            }

            components: [
                sphereMesh,
                material,
                light,
                transform,
                frameGraph.sceneLayer
            ]
        }

        Entity {
            id: light4
            property PointLight light : PointLight {
                color : "white"
                intensity : 3.0
            }
            property Transform transform: Transform {
                translation: Qt.vector3d(5, 2, 7)
            }

            components: [
                light4.light,
                light4.transform,
                frameGraph.sceneLayer
            ]
        }
    }
}
