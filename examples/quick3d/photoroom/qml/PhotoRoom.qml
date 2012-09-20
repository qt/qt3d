/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
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
import Qt3D 2.0

Viewport {
    id: viewport
    width: parent.width; height: parent.height
    picking: true
    //navigation: false
    //showPicking: true

    camera: Camera {
        id: main_camera
        eye: Qt.vector3d(0, 4, 12)
        center: Qt.vector3d(0, 0, -2.5)
    }

    ListModel {
        id: imagesModelLeft
        ListElement { image: "woman.jpg"; }
        ListElement { image: "niagara_falls.jpg"; }
        ListElement { image: "place.jpg"; }
        ListElement { image: "basket.jpg"; }
        ListElement { image: "qtlogo.png"; color: "#006090" }
    }

    Component {
        id: paneComponent
        PhotoPane {
            pictureLayer: index
            image: model.image
            // Items end up with the default value defined in PhotoPane if you
            // try and assign an undefined value, but this logic avoids a
            // string of warnings
            color: (model.color == undefined) ? "#ffffff" : model.color
        }
    }

    Item3D {
        // Left stack of images
        x: -2.1
        Repeater {
            delegate: paneComponent
            model: imagesModelLeft
        }
    }

    //! [0]
    ListModel {
        id: exampleModel
        ListElement { image: "niagara_falls.jpg" }
        ListElement { image: "place.jpg" }
        ListElement { image: "background.jpg" }
        ListElement { image: "basket.jpg" }
        ListElement { image: "woman.jpg" }
    }
    //! [0]

    //! [1]
    Component {
        id: exampleDelegate
        PhotoPane {
            pictureLayer: index
            image: model.image
            // If you intend on removing elements from the model,
            // include this line:
            enabled: index != -1
        }
    }
    //! [1]

    Item3D {
        // Right stack of images
        x: 2.1
        //! [2]
        Repeater {
            id: exampleRepeater
            delegate: exampleDelegate
            model: exampleModel
        }
        //! [2]
    }

    states: [
        State {
            name: "show_photo"
            PropertyChanges {
                target: main_camera
                eye.y: 0
            }
            PropertyChanges {
                target: main_camera
                eye.x: 0
            }
            PropertyChanges {
                target: main_camera
                eye.z: 12
            }
            PropertyChanges {
                target: main_camera
                upVector: Qt.vector3d(0, 1, 0)
            }
        },
        State {
            name: "show_group"
            PropertyChanges {
                target: main_camera
                eye.y: 4
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            NumberAnimation {
                targets: main_camera
                properties: "eye.y"
                easing.type: "OutBounce"
                duration: 700
            }
        }
    ]
}
