/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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

Rectangle {
    id: container
    // Parameters for editting and access
    property string text;
    property variant camera: defaultCamera
    property variant light: defaultLight // The actual Light
    // lightItem contains the defaultLight and a small white sphere at the same
    // position.
    property alias lightItem: lightItem
    // lightSphere is the sphere in lightItem.  It's intended use is to be a
    // visible object at the source of the light, to make it easier to see
    // where the light should be coming from.
    property alias lightSphere: lightSphere
    // The teapotContainer is an item containing the 5 teapots, to make it
    // easier to move them around if necessary
    property alias teapotContainer: teapotContainer
    // view is the viewport that displays the test
    property alias view: view

    // Default values:
    property variant defaultCamera: Camera { eye: Qt.vector3d(0,0,15)}
    property variant defaultLight: Light {}
    property real defaultWidth: 440
    property real defaultHeight: 300

    // Utility values
    property real theta: 0.0
    SequentialAnimation on theta {
        running: true
        loops: Animation.Infinite
        PropertyAnimation {
            from: 0.0
            to: 2.0 * Math.PI
            duration: 2000
        }
    }

    border.width: 2
    border.color: "black"
    radius: 5
    width: defaultWidth
    height: defaultHeight

    Text {
        id: textItem
        wrapMode: "WordWrap"
        horizontalAlignment: "AlignHCenter"
        text: container.text
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Rectangle {
        id: viewportContainer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 1
        anchors.topMargin: 2
        anchors.top: textItem.bottom
        anchors.bottom: container.bottom
        color: "#aaccee"

        Viewport {
            id: view
            anchors.fill: parent
            picking: true
            camera: container.camera
            light: container.light
            Item3D {
                id:lightItem
                // This sphere is small enough to be obscured by the center
                // teapot if it's not manipulated
                Sphere { id: lightSphere; scale: 0.3;
                }
                Light{ id: testLight; }
            }
            Item3D {
                id: teapotContainer
                Teapot { id: centerTeapot }
                Teapot { id: topRightTeapot; x:2; y:2 }
                Teapot { id: topLeftTeapot; x:-2; y:2 }
                Teapot { id: bottomRightTeapot; x:2; y:-2 }
                Teapot { id: bottomLeftTeapot; x:-2; y:-2 }
            }
        }
    }
}
