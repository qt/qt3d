/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

import QtQuick 2.0
import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.9
import QtQuick.Scene3D 2.0

Item {
    id: root
    property int _trackingPosition : 0
    readonly property int squareSize: 100

    Timer {
        running: true
        interval: 16
        repeat: true
        onTriggered: {
            _trackingPosition += 1
            if ((_trackingPosition + squareSize) >= root.width)
                _trackingPosition = 0
        }
    }

    // Scene3D + Qt3D content
    Scene3D {
        id: sceneThreeD
        readonly property double halfWidth: width * 0.5

        focus: true
        anchors.fill: parent
        // anchors.margins: -15
        // Make sure to define the input aspect if we want to handle inputs
        aspects: ["render", "input"]
        multisample: false

        Entity { // Root
            id: sceneRoot
            components: [
                RenderSettings {
                    activeFrameGraph: ForwardRenderer {
                        id: forwardRenderer
                        camera: planeCamera
                        clearColor: "yellow"
                    }
                },
                // Event Source is the Scene3D in that case
                InputSettings { }
            ]

            Camera {
                id: planeCamera
                left: -sceneThreeD.halfWidth
                right: sceneThreeD.halfWidth
                bottom: -(sceneThreeD.height * 0.5)
                top: (sceneThreeD.height * 0.5)
                nearPlane: -100
                farPlane: 100
                projectionType: CameraLens.OrthographicProjection
                position: Qt.vector3d(0, 0, 10)
                viewCenter: Qt.vector3d(0, 0, 0)
            }

                Entity {
                    id: trackingCube
                    components: [
                        CuboidMesh {
                            xExtent: 100
                            yExtent: 100
                            zExtent: 2
                        },

                        PhongMaterial {
                            diffuse: "orange"
                            ambient: "orange"
                        },

                        Transform {
                            translation: Qt.vector3d(-sceneThreeD.halfWidth + squareSize * 0.5 + _trackingPosition, -50, 0)
                        }
                    ]
                }
        }
    }

    // QtQuick Content
    Rectangle {
        id: qtQuickTracer
        x: _trackingPosition
        onXChanged: {
            console.info("Tracking position is now:" + _trackingPosition);
        }

        y: root.height * 0.5 - height
        width: squareSize; height: squareSize
        color: "red"
        //transformOrigin: Item.Center
        Text {
            color: "white"
            text: "Rendered with QtQuick"
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WrapAnywhere
        }

        Text {
            color: "white"
            text: "Rendered with Qt3D"
            width: parent.width
            height: parent.height
            anchors.top: parent.bottom
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WrapAnywhere
        }

        Rectangle {
            width: 1
            height: 200
            x: 100
            color: "black"
        }
    }
}
