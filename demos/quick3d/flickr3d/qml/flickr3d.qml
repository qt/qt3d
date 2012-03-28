/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D examples of the Qt Toolkit.
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
import Qt3D.Shapes 1.0
import "common" as Common

Viewport {
    width: 1024
    height: 768

    camera: Camera {
        id: cam
        property real viewAngle: 0.0
        eye: Qt.vector3d(Math.sin(cam.viewAngle) * 20.0, 3.0, Math.cos(cam.viewAngle) * 20.0)
    }

    NumberAnimation {
        running: true
        target: cam; property: "viewAngle";
        from: 0.0; to: 360.0; duration: 2000000;
        loops: Animation.Infinite
    }

    Skybox {
        source: "skybox"
    }

    Common.RssModel { id: rssModel }

    Item3D {
        transform: [
            Rotation3D { axis: Qt.vector3d(1, 0, 0); angle: 90 },
            Translation3D { translate: Qt.vector3d(0, 1, 0) }
        ]

        Cylinder {
            levelOfDetail: 1
            length: 2.0
            radius: 2.8
            effect: Effect {
                color: "#ccccdd"
            }
        }

    }

    Component {
        id: octoDisplayDelegate
        Item3D {
            transform: [
                Translation3D { translate: Qt.vector3d(0, 0, 2.6) },
                // index is a special variable that comes from model instancing
                Rotation3D { axis: Qt.vector3d(0, 1, 0); angle: (360 / 16) * (index * 2 + 1) }
            ]
            Item3D {
                enabled: index > -1 && index < 9
                transform: [
                    Rotation3D { axis: Qt.vector3d(1, 0, 0); angle: 90 }
                ]
                Quad {
                    effect: Effect {
                        texture: model.imagePath
                    }
                }
            }
        }
    }

    Repeater {
        delegate: octoDisplayDelegate
        model: rssModel
    }

}
