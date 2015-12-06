/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire <paul.lemire350@gmail.com>
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

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import QtQml 2.2

Entity {
    id: root
    property Camera camera
    property real linearSpeed: 10.0
    property real orbitSpeed: -180.0
    property real lookSpeed: 180.0
    property bool firstPersonMode: true

    QtObject {
        id: d
        readonly property vector3d firstPersonUp: Qt.vector3d(0, 1, 0)
        property bool leftMouseButtonPressed: false
        property bool rightMouseButtonPressed: false
        property real vx: 0;
        property real vy: 0;
        property real vz: 0;
        property real dx: 0
        property real dy: 0
        property bool fineMotion: false
    }

    KeyboardController {
        id: keyboardSourceDevice
    }

    MouseController {
        id: mouseSourceDevice
        sensitivity: d.fineMotion ? 0.01 : 0.1
    }

    LogicalDevice {
        id: cameraControlDevice

        actions: [
            Action {
                name: "LMB"
                inputs: [
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        keys: [MouseController.Left]
                    }
                ]
            },
            Action {
                name: "RMB"
                inputs: [
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        keys: [MouseController.Right]
                    }
                ]
            },
            Action {
                name: "fineMotion"
                inputs: [
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_Shift]
                    }
                ]
            }

        ] // actions

        axes: [
            // Rotation
            Axis {
                name: "RX"
                inputs: [
                    AxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseController.X
                    }
                ]
            },
            Axis {
                name: "RY"
                inputs: [
                    AxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseController.Y
                    }
                ]
            },
            // Translation
            Axis {
                name: "TX"
                inputs: [
                    AxisInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_Left]
                        scale: -1.0
                    },
                    AxisInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_Right]
                        scale: 1.0
                    }
                ]
            },
            Axis {
                name: "TZ"
                inputs: [
                    AxisInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_Up]
                        scale: 1.0
                    },
                    AxisInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_Down]
                        scale: -1.0
                    }
                ]
            },
            Axis {
                name: "TY"
                inputs: [
                    AxisInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_PageUp]
                        scale: 1.0
                    },
                    AxisInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_PageDown]
                        scale: -1.0
                    }
                ]
            }
        ] // axes
    }

    components: [
        AxisActionHandler {
            id: handler
            logicalDevice: cameraControlDevice

            onAxisValueChanged: {

                switch (name) {

                case "TX": {
                    d.vx = axisValue * linearSpeed
                    break;
                }

                case "TY": {
                    d.vy = axisValue * linearSpeed
                    break;
                }

                case "TZ": {
                    d.vz = axisValue * linearSpeed
                    break;
                }

                case "RX": {
                    d.dx = axisValue;
                    break;
                }

                case "RY": {
                    d.dy = axisValue;
                    break;
                }

                }
            }

            onActionStarted: {

                switch (name) {

                case "LMB": {
                    d.leftMouseButtonPressed = true;
                    break;
                }

                case "RMB": {
                    d.rightMouseButtonPressed = true;
                    break;
                }

                case "fineMotion": {
                    console.log("fineMotion started")
                    d.fineMotion = true;
                    break;
                }

                }

            }

            onActionFinished: {

                switch (name) {

                case "LMB": {
                    d.leftMouseButtonPressed = false;
                    break;
                }

                case "RMB": {
                    d.rightMouseButtonPressed = false;
                    break;
                }

                case "fineMotion": {
                    console.log("fineMotion finished")
                    d.fineMotion = false;
                    break;
                }

                }
            }
        },

        LogicComponent {
            onFrameUpdate: {
                // The time difference since the last frame is passed in as the
                // argument dt. It is a floating point value in units of seconds.
                root.camera.translate(Qt.vector3d(d.vx, d.vy, d.vz).times(dt))

                if (d.leftMouseButtonPressed) {
                    if (root.firstPersonMode)
                        root.camera.pan(root.lookSpeed * d.dx * dt, d.firstPersonUp)
                    else
                        root.camera.pan(root.lookSpeed * d.dx * dt)
                    root.camera.tilt(root.lookSpeed * d.dy * dt)
                } else if (d.rightMouseButtonPressed) {
                    if (root.firstPersonMode)
                        root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt, d.firstPersonUp)
                    else
                        root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt)
                    root.camera.tiltAboutViewCenter(root.orbitSpeed * d.dy * dt)
                }
            }
        }
    ] // components
}
