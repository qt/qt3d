/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item {
    id: screen
    width: 640
    height: 520

    Viewport  {
        width: 640
        height: 480
        fillColor: "black"

        camera: Camera {eye: Qt.vector3d(0,3,20) }
        picking: true
        //showPicking: true

        ListModel {
            id: positionModel
            ListElement {
                x: -2.0; y: 1.8;
                color: "#CC2200";
                name_string: "top left"
            }
            ListElement {
                x: 0.0; y: 1.8;
                color: "#8822aa";
                name_string: "top middle"
            }
            ListElement {
                x: 2.0; y: 1.8;
                color: "#1122ca";
                name_string: "top right"
            }
            ListElement {
                x: -2.0; y: 0.0;
                color: "#CC8800";
                name_string: "mid left"
            }
            ListElement {
                x: 0.0; y: 0.0;
                color: "#8888aa";
                name_string: "middle"
            }
            ListElement {
                x: 2.0; y: 0.0;
                color: "#118800";
                name_string: "mid right"
            }
            ListElement {
                x: -2.0; y: -1.8;
                color: "#CCBB00";
                name_string: "bottom left"
            }
            ListElement {
                x: 0.0; y: -1.8;
                color: "#88BB00";
                name_string: "bottom middle"
            }
            ListElement {
                x: 2.0; y: -1.8;
                color: "#11BB00";
                name_string: "bottom right"
            }
        }

        Component {
            id: cubeComponent
            Cube {
                position: Qt.vector3d(model.x,model.y, 0.0)
                onClicked: console.log(model.name_string)
                effect: Effect {
                    color: model.color
                }
            }
        }

        Item3D {
            Repeater {
                delegate: cubeComponent
                model: positionModel
            }
        }
    }
    Rectangle {
        id: instructions
        x: 0; y: 480
        width: screen.width
        height: 40
        color: "#3333aa"
        Text {
            id: instructionsText
            wrapMode: "WordWrap"
            text: "Pick a cube, check log output. Drag in the black area to move all cubes."
            anchors.centerIn: parent
        }
    }
}

