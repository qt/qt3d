/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import Qt3D 1.0
import ModelTweak 1.0

Rectangle {
    id: mainwindow
    width: 1024
    height: 768
    color: "#444444"
    border.color: "black"
    property alias targetMesh: quickLoad.filename

    QuickSave {
        id: quickSave;
    }

    QuickLoad {
        id: quickLoad
        filename: "meshes/monkey.3ds"
    }

    Translation3D {
        id: transformTranslate
        translate: Qt.vector3d(0, 0, 0)
    }

    Rotation3D {
        id: transformRotateX
        angle: 0
        axis: Qt.vector3d(1, 0, 0)
    }

    Rotation3D {
        id: transformRotateY
        angle: 0
        axis: Qt.vector3d(0, 1, 0)
    }

    Rotation3D {
        id: transformRotateZ
        angle: 0
        axis: Qt.vector3d(0, 0, 1)
    }

    Scale3D {
        id: transformScale
        scale: Qt.vector3d(1, 1, 1)
    }

    Mesh {
        id: source_mesh
        source: quickLoad.filename
    }

    ModelViewport {
        id: mvpZY
        x: 0;
        y: 0;
        camera.eye: Qt.vector3d(10, 0, 0);
        stateName: "ZYMaximised"

        function translateMouseX(mouse) {
            transformTranslate.translate =
                    Qt.vector3d(transformTranslate.translate.x,
                                transformTranslate.translate.y,
                                translateZ + (downX - mouse.x)/translateSensitivity)
        }
        function translateMouseY(mouse) {
            transformTranslate.translate =
                    Qt.vector3d(transformTranslate.translate.x,
                                translateY + (downY - mouse.y)/translateSensitivity,
                                transformTranslate.translate.z)
        }
        function rotateMouseX(mouse) {
            transformRotateY.angle = rotateY - (downX - mouse.x)/rotateSensitivity
        }
        function rotateMouseY(mouse) {
            transformRotateZ.angle = rotateZ - (downY - mouse.y)/rotateSensitivity
        }
        function scaleMouseX(mouse) {
            var s = scaleZ - (downX - mouse.x)/scaleSensitivity;
            if (s<0)
                return;
            transformScale.scale = Qt.vector3d(transformScale.scale.x, transformScale.scale.y, s)
        }
        function scaleMouseY(mouse) {
            var s = scaleY + (downY - mouse.y)/scaleSensitivity;
            if (s<0)
                return;
            transformScale.scale = Qt.vector3d(transformScale.scale.x, s, transformScale.scale.z)
        }
    }

    ModelViewport {
        id: mvpXY
        x: parent.width/2
        y: 0;
        camera.eye: Qt.vector3d(0, 0, 10);
        stateName: "XYMaximised"

        function translateMouseX(mouse) {
            transformTranslate.translate =
                    Qt.vector3d(translateX - (downX - mouse.x)/translateSensitivity,
                                transformTranslate.translate.y,
                                transformTranslate.translate.z)
        }
        function translateMouseY(mouse) {
            transformTranslate.translate =
                    Qt.vector3d(transformTranslate.translate.x,
                                translateY + (downY - mouse.y)/translateSensitivity,
                                transformTranslate.translate.z)
        }
        function rotateMouseX(mouse) {
            transformRotateY.angle = rotateY - (downX - mouse.x)/rotateSensitivity
        }
        function rotateMouseY(mouse) {
            transformRotateX.angle = rotateX - (downY - mouse.y)/rotateSensitivity
        }
        function scaleMouseX(mouse) {
            var s = scaleX - (downX - mouse.x)/scaleSensitivity
            if (s<0)
                return;
            transformScale.scale = Qt.vector3d(s, transformScale.scale.y, transformScale.scale.z)
        }
        function scaleMouseY(mouse) {
            var s = scaleY + (downY - mouse.y)/scaleSensitivity
            if (s<0)
                return;
            transformScale.scale = Qt.vector3d(transformScale.scale.x, s, transformScale.scale.z)
        }
    }

    ModelViewport {
        id: mvpXZ
        x: parent.width/2;
        y: parent.height/2;
        camera.eye: Qt.vector3d(0, 10, 0);
        camera.upVector: Qt.vector3d(0, 0, -1);
        stateName: "XZMaximised"

        function translateMouseX(mouse) {
            transformTranslate.translate =
                    Qt.vector3d(translateX - (downX - mouse.x)/translateSensitivity,
                                transformTranslate.translate.y,
                                transformTranslate.translate.z)
        }
        function translateMouseY(mouse) {
            transformTranslate.translate =
                    Qt.vector3d(transformTranslate.translate.x,
                                transformTranslate.translate.y,
                                translateZ - (downY - mouse.y)/translateSensitivity)
        }
        function rotateMouseX(mouse) {
            transformRotateZ.angle = rotateZ + (downX - mouse.x)/rotateSensitivity
        }
        function rotateMouseY(mouse) {
            transformRotateX.angle = rotateX - (downY - mouse.y)/rotateSensitivity
        }
        function scaleMouseX(mouse) {
            var s = scaleX - (downX - mouse.x)/scaleSensitivity;
            if (s<0)
                return;
            transformScale.scale = Qt.vector3d(s, transformScale.scale.y, transformScale.scale.z)
        }
        function scaleMouseY(mouse) {
            var s = scaleZ + (downY - mouse.y)/scaleSensitivity;
            if (s<0)
                return;
            transformScale.scale = Qt.vector3d(transformScale.scale.x, transformScale.scale.y, s)
        }
    }

    Rectangle {
        id: meshName
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 10
        anchors.bottom: parent.bottom
        radius: 10
        border.width: 1
        border.color: "black"
        color: "white"
        width: parent.width - 20
        height: 20

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: source_mesh.source
        }
    }

    ModelPropertiesPane {
        id: modelPropertiesPane
        x: 16;
        y: parent.height / 2 + 16

    }

    ButtonBarPane {
        anchors {
            top: modelPropertiesPane.top;
            left: modelPropertiesPane.right; leftMargin: 16
        }
    }

    HelpOverlay {
        id: helpOverlay
        visible: false
    }

    states: [
        State {
            name: "3Views"
        },
        State {
            name: "ZYMaximised"
            PropertyChanges { target: mvpZY; x: 0;                  y: 0;                 width: mainwindow.width;    height: mainwindow.height;   }
            PropertyChanges { target: mvpXY; x: mainwindow.width;   y: 0;                 width: 0;                   height: mainwindow.height/2; }
            PropertyChanges { target: mvpXZ; x: mainwindow.width;   y: mainwindow.height; width: 0;                   height: 0;                   }
        },
        State {
            name: "XYMaximised"
            PropertyChanges { target: mvpZY; x: 0;                  y: 0;                 width: 0;                   height: mainwindow.height/2; }
            PropertyChanges { target: mvpXY; x: 0;                  y: 0;                 width: mainwindow.width;    height: mainwindow.height;   }
            PropertyChanges { target: mvpXZ; x: mainwindow.width/2; y: mainwindow.height; width: mainwindow.width/2;  height: 0;                   }
        },
        State {
            name: "XZMaximised"
            PropertyChanges { target: mvpZY; x: 0;                  y: 0;                 width: 0;                   height: 0;                   }
            PropertyChanges { target: mvpXY; x: mainwindow.width/2; y: 0;                 width: mainwindow.width/2;  height: 0;                   }
            PropertyChanges { target: mvpXZ; x: 0;                  y: 0;                 width: mainwindow.width;    height: mainwindow.height;   }
        }
    ]

    state: "3Views"
}
