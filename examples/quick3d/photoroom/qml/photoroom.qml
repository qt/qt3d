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

import Qt 4.7
import Qt3D 1.0

Viewport {
    id: viewport
    width: 800; height: 480
    picking: true
    //navigation: false
    //showPicking: true

    camera: Camera {
        id: main_camera
        eye: Qt.vector3d(0, 4, 12)
        center: Qt.vector3d(0, 0, -2.5)
    }

    PhotoPane {
        offset: -2.1
        layer: 0
        image: "textures/button/woman.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 1
        image: "textures/photos/niagara_falls.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 2
        image: "textures/photos/place.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 3
        image: "textures/photos/background.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 4
        image: "textures/basket.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 5
        image: "textures/qtlogo.png"
        effect.color: "#006090"
    }

    PhotoPane {
        offset: 2.1
        layer: 0
        image: "textures/photos/niagara_falls.jpg"
    }
    PhotoPane {
        offset: 2.1
        layer: 1
        image: "textures/photos/place.jpg"
    }
    PhotoPane {
        offset: 2.1
        layer: 2
        image: "textures/photos/background.jpg"
    }
    PhotoPane {
        offset: 2.1
        layer: 3
        image: "textures/basket.jpg"
    }
    PhotoPane {
        offset: 2.1
        layer: 4
        image: "textures/qtlogo.png"
        effect.color: "#006090"
    }
    PhotoPane {
        offset: 2.1
        layer: 5
        image: "textures/button/woman.jpg"
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
