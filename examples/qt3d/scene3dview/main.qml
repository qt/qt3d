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

import QtQuick 2.14
import QtQuick.Scene3D 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Item {

    //! [0]
    Scene3D {
        id: scene3dInstance
        anchors.fill: parent
        focus: true
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
        multisample: true
    }
    //! [0]

    ColumnLayout {
        anchors.fill: parent
        Grid {
            id: grid
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            rows: 2
            spacing: 50

            readonly property int cellWidth: (width - spacing ) * 0.5
            readonly property int cellHeight: (height -spacing) * 0.5
            readonly property var colors: ["green", "gray", "orange", "blue"]
            readonly property int maxInstances: grid.rows * grid.columns

            Repeater {
                id: gridRepeater
                model: grid.rows * grid.columns

                //! [1]
                Rectangle {
                    width: grid.cellWidth
                    height: grid.cellHeight
                    color: "darkRed"
                    scale: ma.pressed || ma.containsMouse ? 0.8 : 1
                    Behavior on scale { NumberAnimation { duration: 750 } }

                    Scene3DView {
                        onXChanged: console.log(("X (%1): %2").arg(model.index).arg(x))
                        scene3D: scene3dInstance
                        anchors.fill: parent
                        AnimatedEntity {
                            sceneColor: grid.colors[model.index]
                            //! [1.1]
                            // Only clear the color for the first Scene3DView
                            clearColor: model.index === 0
                            //! [1.1]
                        }
                    }
                    Text {
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            bottom: parent.bottom
                        }

                        color: "white"
                        font.bold: true
                        text: "Scene3DView " + (model.index + 1)
                    }

                    MouseArea {
                        id: ma
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
                //! [1]
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Button {
                text: "-"
                onClicked: gridRepeater.model = Math.max(1, gridRepeater.count - 1);
            }
            Text { text: "Instances (%1/%2)".arg(gridRepeater.count).arg(grid.maxInstances) }
            Button {
                text: "+"
                onClicked: gridRepeater.model = Math.min(grid.maxInstances, gridRepeater.count + 1);
            }
        }
    }
}
