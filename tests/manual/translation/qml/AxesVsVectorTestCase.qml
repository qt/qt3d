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

Rectangle {
    id: container
    // Parameters for editting
    property string text;
    property variant translationVectorA: Qt.vector3d(1,1,1)
    property variant translationVectorB: Qt.vector3d(1,1,1)
    property variant camera: defaultCamera
    property real itemX: 0
    property real itemY: 0
    property real itemZ: 0
    property int animationDuration: 2000

    // Default values:
    property variant defaultCamera: Camera {eye: Qt.vector3d(0,5,30)}
    property real defaultWidth: 220
    property real defaultHeight: 150
    property variant itemPosition:
        Qt.vector3d((translationVectorB.x * animationFactor +
                     translationVectorA.x * (1.0 - animationFactor)),
                    translationVectorB.y * animationFactor +
                    translationVectorA.y * (1.0 - animationFactor),
                    translationVectorB.z * animationFactor +
                    translationVectorA.z * (1.0 - animationFactor))
    property real animationFactor: 0.0

    SequentialAnimation on animationFactor {
        loops: Animation.Infinite
        PropertyAnimation {
            from: 0.0
            to: 1.0
            duration: animationDuration
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

            Material {
                // Distinctive red Color
                id: red
                ambientColor: "#884422"
                diffuseColor: "#EE4422"
                specularColor: "#FF6644"
                shininess: 200
            }

            Material {
                // Distinctive blue Color
                id: blue
                ambientColor: "#224488"
                diffuseColor: "#2244EE"
                specularColor: "#4466FF"
                shininess: 200
            }

            Effect {
                id: redEffect; material: red
                objectName: "redEffect"
            }

            Effect {
                id: blueEffect; material: blue
                objectName: "blueEffect"
            }

            Teapot
            {
                id: vectorModel
//                effect: redEffect
                position: itemPosition
            }
            Teapot
            {
                id: axesModel
//                effect: blueEffect
                x: itemX
                y: itemY
                z: itemZ
            }
        }
    }
}
