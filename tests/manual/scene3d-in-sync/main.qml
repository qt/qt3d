// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

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
