/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

import Qt3D.Core 2.16
import Qt3D.Render 2.16
import Qt3D.Input 2.0
import Qt3D.Extras 2.16

import QtQuick 2.0 as QQ2


Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController { camera: camera }

    components: [
        RenderSettings {
            id: renderSettings
            activeFrameGraph: ForwardRenderer {
                camera: camera
                clearColor: "transparent"
            }
        },
        InputSettings { }
    ]

    // normal entity, BV computed from geometry
    Entity {
        id: sphereEntity1
        components: [
            SphereMesh { radius: 4 },
            PhongMaterial { diffuse: "blue" },
            Transform { translation: Qt.vector3d(mainAnimation.x, 9, 0) }
        ]
    }

    // override BV of the entity with one from another (non-rendered) geometry
    Entity {
        id: sphereEntity2
        components: [
            SphereMesh { radius: 4 },
            PhongMaterial { diffuse: "red" },
            Transform { translation: Qt.vector3d(mainAnimation.x, -9, 0) },
            BoundingVolume { view: SphereGeometryView { radius: 2 } }
        ]
    }

    // override BV with known specific values (can also be done by using a separate BoundingVolume)
    Entity {
        id: sphereEntity3
        components: [
            SphereMesh {
                radius: 4
                minPoint: Qt.vector3d(-.5, -.5, -.5)
                maxPoint: Qt.vector3d(.5, .5, .5)
            },
            PhongMaterial { diffuse: "green" },
            Transform { translation: Qt.vector3d(mainAnimation.x, 0 , 0) }
        ]
    }

    QQ2.SequentialAnimation {
        id: mainAnimation

        property real x: 0
        loops: QQ2.Animation.Infinite
        running: true

        QQ2.NumberAnimation {
            target: mainAnimation
            property: "x"
            duration: 2000
            from: -25
            to: 25
            easing.type: Easing.InOutQuad
        }

        QQ2.NumberAnimation {
            target: mainAnimation
            property: "x"
            duration: 2000
            from: 25
            to: -25
            easing.type: Easing.InOutQuad
        }
    }
}
