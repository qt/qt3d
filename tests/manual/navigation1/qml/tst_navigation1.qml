/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

// QTBUG-17408 TEST
Item {
    id: screen
    width: 720
    height: 480

    //ITEM 1: Simple model without materials
    Rectangle {
        id: panel1
        x: 0; y:0
        width:  screen.width/2
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            camera: Camera { eye: Qt.vector3d(0,3,15)}

            Item3D {
                scale: 2
                position: Qt.vector3d(0,-1,0)
                mesh: Mesh {source: "meshes/monkey.3ds"}
            }
        }

        Rectangle {
            border.width: 3; radius: 5
            width: parent.width
            height: text1.height + 10
            Text {
                id: text1
                width: parent.width
                wrapMode: "WordWrap"
                text: "  POSITIONAL ZOOM:\n\n" +
                "  A shaded lit monkey head should\n"+
                "  be displayed.  Left click-and-hold" +
                "  & move mouse to rotate.\n\n" +
                "  Wheel to zoom.  Zooming in should\n"+
                "  result in eventual clipping.";
                anchors.centerIn: parent
            }
        }
    }

    //ITEM 2: Simple model with materials
    Rectangle {
        id: panel2
        anchors.left: panel1.right
        anchors.top:  parent.top
        anchors.right:  parent.right
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            fovzoom: true
            camera: Camera { eye: Qt.vector3d(0,3,15)}

            Item3D {
                scale: 2
                position: Qt.vector3d(0,-1,0)
                mesh: Mesh {source: "meshes/monkey.3ds"}
            }
        }

        Rectangle {
            border.width: 3; radius: 5
            width: parent.width
            height: text2.height + 10
            Text {
                id: text2
                width: parent.width
                wrapMode: "WordWrap"
                 text: "  FIELD-OF-VIEW ZOOM:\n\n" +
                 "  A shaded lit monkey head should\n"+
                 "  be displayed.  Left click-and-hold" +
                 "  & move mouse to rotate.\n\n" +
                 "  Wheel to zoom.  Zooming in should\n"+
                 "  not result in eventual clipping.";
                anchors.centerIn: parent
            }
        }
    }    
}


