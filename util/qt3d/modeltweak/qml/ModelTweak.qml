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
    property alias targetMesh: quickFile.filename

    QuickFile {
        id: quickFile
        filename: "meshes/penguin.3ds"
    }

    Mesh {
        id: source_mesh
        source: quickFile.filename
    }

    Translation3D { id: transformTranslate; translate: Qt.vector3d(0, 0, 0);           }
    Rotation3D    { id: transformRotateX;   axis: Qt.vector3d(1, 0, 0);      angle: 0; }
    Rotation3D    { id: transformRotateY;   axis: Qt.vector3d(0, 1, 0);      angle: 0; }
    Rotation3D    { id: transformRotateZ;   axis: Qt.vector3d(0, 0, 1);      angle: 0; }
    Scale3D       { id: transformScale;     scale: Qt.vector3d(1, 1, 1);               }

    ModelViewport {
        id: mvpZY
        x: 0;
        y: 0;
        camera.eye: Qt.vector3d(20, 0, 0);
        stateName: "ZYMaximised"
        viewportName: "Z,Y axis (right)"

        property alias position: transformTranslate.translate;

        onMouseTranslateX: position = Qt.vector3d(position.x, position.y, translate.z + (down.x - mouse.x)/translateSensitivity)
        onMouseTranslateY: position = Qt.vector3d(position.x, translate.y + (down.y - mouse.y)/translateSensitivity, position.z)
        onMouseRotateX: transformRotateY.angle = rotate.y - (down.x - mouse.x)/rotateSensitivity
        onMouseRotateY: transformRotateZ.angle = rotate.z - (down.y - mouse.y)/rotateSensitivity
        onMouseScaleX: {
            var s = scale3d.z - (down.x - mouse.x)/scaleSensitivity;
            transformScale.scale = Qt.vector3d(transformScale.scale.x, transformScale.scale.y, s<0 ? 0 : s)
        }
        onMouseScaleY: {
            var s = scale3d.y + (down.y - mouse.y)/scaleSensitivity;
            transformScale.scale = Qt.vector3d(transformScale.scale.x, s<0 ? 0 : s, transformScale.scale.z)
        }
    }

    ModelViewport {
        id: mvpXY
        x: parent.width/2
        y: 0;
        camera.eye: Qt.vector3d(0, 0, 20);
        stateName: "XYMaximised"
        viewportName: "X,Y axis (front)"

        property alias position: transformTranslate.translate;

        onMouseTranslateX: position = Qt.vector3d(translate.x - (down.x - mouse.x)/translateSensitivity, position.y, position.z)
        onMouseTranslateY: position = Qt.vector3d(position.x, translate.y + (down.y - mouse.y)/translateSensitivity, position.z)
        onMouseRotateX: transformRotateY.angle = rotate.y - (down.x - mouse.x)/rotateSensitivity
        onMouseRotateY: transformRotateX.angle = rotate.x - (down.y - mouse.y)/rotateSensitivity
        onMouseScaleX: {
            var s = scale3d.x - (down.x - mouse.x)/scaleSensitivity
            transformScale.scale = Qt.vector3d(s<0 ? 0 : s, transformScale.scale.y, transformScale.scale.z)
        }
        onMouseScaleY: {
            var s = scale3d.y + (down.y - mouse.y)/scaleSensitivity
            transformScale.scale = Qt.vector3d(transformScale.scale.x, s<0 ? 0 : s, transformScale.scale.z)
        }
    }

    ModelViewport {
        id: mvpXZ
        x: parent.width/2;
        y: parent.height/2;
        camera.eye: Qt.vector3d(0, 20, 0);
        camera.upVector: Qt.vector3d(0, 0, -1);
        stateName: "XZMaximised"
        viewportName: "X,Z axis (top)"

        property alias position: transformTranslate.translate;

        onMouseTranslateX: position = Qt.vector3d(translate.x - (down.x - mouse.x)/translateSensitivity, position.y, position.z)
        onMouseTranslateY: position = Qt.vector3d(position.x, position.y, translate.z - (down.y - mouse.y)/translateSensitivity)
        onMouseRotateX: transformRotateZ.angle = rotate.z + (down.x - mouse.x)/rotateSensitivity
        onMouseRotateY: transformRotateX.angle = rotate.x - (down.y - mouse.y)/rotateSensitivity
        onMouseScaleX: {
            var s = scale3d.x - (down.x - mouse.x)/scaleSensitivity;
            transformScale.scale = Qt.vector3d(s<0 ? 0 : s, transformScale.scale.y, transformScale.scale.z)
        }
        onMouseScaleY: {
            var s = scale3d.z + (down.y - mouse.y)/scaleSensitivity;
            transformScale.scale = Qt.vector3d(transformScale.scale.x, transformScale.scale.y, s<0 ? 0 : s)
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

    Item {
        width: parent.width / 2
        anchors.bottom: meshName.top
        anchors.bottomMargin: 8
        height: buttonBarPane.height

        ButtonBarPane {
            id: buttonBarPane
            anchors.centerIn: parent
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
            PropertyChanges { target: mvpZY; x: 0;                  y: 0;                 width: mainwindow.width;   height: mainwindow.height;   }
            PropertyChanges { target: mvpXY; x: mainwindow.width;   y: 0;                 width: 0;                  height: mainwindow.height/2; }
            PropertyChanges { target: mvpXZ; x: mainwindow.width;   y: mainwindow.height; width: 0;                  height: 0;                   }
        },
        State {
            name: "XYMaximised"
            PropertyChanges { target: mvpZY; x: 0;                  y: 0;                 width: 0;                  height: mainwindow.height/2; }
            PropertyChanges { target: mvpXY; x: 0;                  y: 0;                 width: mainwindow.width;   height: mainwindow.height;   }
            PropertyChanges { target: mvpXZ; x: mainwindow.width/2; y: mainwindow.height; width: mainwindow.width/2; height: 0;                   }
        },
        State {
            name: "XZMaximised"
            PropertyChanges { target: mvpZY; x: 0;                  y: 0;                 width: 0;                  height: 0;                   }
            PropertyChanges { target: mvpXY; x: mainwindow.width/2; y: 0;                 width: mainwindow.width/2; height: 0;                   }
            PropertyChanges { target: mvpXZ; x: 0;                  y: 0;                 width: mainwindow.width;   height: mainwindow.height;   }
        }
    ]

    state: "3Views"
}
