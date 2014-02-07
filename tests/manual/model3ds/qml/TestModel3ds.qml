/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0

// QTBUG-17392 / QTBUG-17391
Item {
    id: screen
    width: 720
    height: 480

    //ITEM 1: Simple model without materials
    Rectangle {
        id: panel1
        x: 0; y:0
        width:  screen.width/4
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
                text: "  MULTI-COMPONENT:  No\n  MATERIALS:  None\n\n  A grey shaded monkey head\n  should be visible.\n\n  Use mouse to rotate/zoom."
                anchors.centerIn: parent
            }
        }
    }

    //ITEM 2: Simple model with materials
    Rectangle {
        id: panel2
        anchors.left: panel1.right
        anchors.top:  parent.top
        width:  screen.width/4
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            camera: Camera { eye: Qt.vector3d(0,3,15)}

            Item3D {
                scale: 0.8
                position: Qt.vector3d(0,-1,0)
                mesh: Mesh {source: "meshes/penguin.3ds"}
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
                 text: "  MULTI-COMPONENT:  No\n  MATERIALS:  Yes\n\n  A coloured, lit penguin\n  should be visible.\n\n  Use mouse to rotate/zoom."
                anchors.centerIn: parent
            }
        }
    }

    //ITEM 3: A complex model without materials
    Rectangle {
        id: panel3
        anchors.left: panel2.right
        anchors.top:  parent.top
        width:  screen.width/4
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            camera: Camera { eye: Qt.vector3d(0,0,10)}

            Item3D {
                scale: 0.4
                position: Qt.vector3d(0,-1,0)
                mesh: Mesh {source: "meshes/lunar-lander.3ds"}
            }
        }
        //text for item3
        Rectangle {
            border.width: 3; radius: 5
            width: parent.width
            height: text3.height + 10
            Text {
                id: text3
                width: parent.width
                wrapMode: "WordWrap"
                text: "  MULTI-COMPONENT:  Yes\n  MATERIALS:  None\n\n  A grey shaded lunar lander\n  should be visible.\n\n  Use mouse to rotate/zoom."
                anchors.centerIn: parent
            }
        }
    }
    //ITEM 4: A Complex model with materials
    Rectangle {
        id: panel4
        anchors.left: panel3.right
        anchors.top:  parent.top
        width:  screen.width/4
        height: screen.height
        border.width:  3
        color: "#3333aa"

        Viewport  {
            anchors.fill: parent
            camera: Camera { eye: Qt.vector3d(0,0,10)}

            Item3D {
                scale: 0.4
                position: Qt.vector3d(0,-2,0)

                transform: [
                    Rotation3D {id: correct; angle: 90; axis: Qt.vector3d(0,1,0)},
                    Rotation3D {id: pitch; angle: -90; axis: Qt.vector3d(0,0,1)},
                    Rotation3D {id: roll; angle: 0; axis: Qt.vector3d(1,0,0)},
                    Rotation3D {id: yaw; angle: 0; axis: Qt.vector3d(0,1,0)}
                ]

                mesh: Mesh {source: "meshes/robot.3ds"}
            }
        }
        //text for item4
        Rectangle {
            border.width: 3; radius: 5
            width: parent.width
            height: text4.height + 10
            Text {
                id: text4
                width: parent.width
                wrapMode: "WordWrap"
                text: "  MULTI-COMPONENT:  Yes\n  MATERIALS:  Yes\n\n  A lit, coloured, textured robot\n  should be visible.\n\n  Use mouse to rotate/zoom."
                anchors.centerIn: parent
            }
        }
    }
}


