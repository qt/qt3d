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
    property string text:
        "This example combines a rotation, scale and translate in one " +
        "Item3D.  It should show a teapot spinning anti-clockwise, growing " +
        "and moving up-right relative to the initial camera, and then " +
        "reversing.";
    property variant camera: defaultCamera

    // Default values:
    property variant defaultCamera: Camera { eye: Qt.vector3d(0,5,30) }
    property real defaultWidth: 440
    property real defaultHeight: 300
    property real animationFactor: 0.0
    property real animationDuration: 2000

    SequentialAnimation on animationFactor {
        loops: Animation.Infinite
        PropertyAnimation {
            from: 0.0
            to: 1.0
            duration: animationDuration
        }
        PropertyAnimation {
            from: 1.0
            to: 0.0
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

            Teapot
            {
                id: testModel
                property real scaleX: 1 + (2.0 * animationFactor);
                property real scaleY: 1 + (2.0 * animationFactor);
                property real scaleZ: 1 + (2.0 * animationFactor);

                property real rotationAngle: 360.0 *  animationFactor;
                property real rotationAxisX: 0.0;
                property real rotationAxisY: 0.0;
                property real rotationAxisZ: 1.0;

                property real translationX: 4 * (2.0 * (animationFactor  -0.5));
                property real translationY: 3 * (2.0 * (animationFactor - 0.5));
                property real translationZ: 0;

                transform: [
                    Scale3D {
                        scale: Qt.vector3d(testModel.scaleX,
                                           testModel.scaleY,
                                           testModel.scaleZ)
                    },
                    Rotation3D {
                        axis: Qt.vector3d(testModel.rotationAxisX,
                                          testModel.rotationAxisY,
                                          testModel.rotationAxisZ)
                        angle: testModel.rotationAngle
                    },
                    Translation3D {
                        translate: Qt.vector3d(testModel.translationX,
                                               testModel.translationY,
                                               testModel.translationZ)
                    }
                ]
            }
        }
    }
}
