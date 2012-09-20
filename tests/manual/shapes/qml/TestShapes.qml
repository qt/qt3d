/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

Item {
    id: screen
    width: 720
    height: 480
    Rectangle {
        id: description
        border.width: 3
        radius: 5
        width: parent.width
        height: text.height + 10
        Text {
            id: text
            width: parent.width
            wrapMode: "WordWrap"
            text: "There should be a green cylinder on the left, a beige sphere in the middle,\n" +
                  "and a red capsule on the right hand side.\n\n" +
                  "They should be the same diameter, but the capsule & cylinder twice the height.\n" +
                  "The capsule and cylinder should be oriented with their length vertically.\n" +
                  "Click & drag in the window to rotate the objects to confirm."
            anchors.centerIn: parent
        }
    }
    Rectangle {
        width: parent.width
        anchors.top: description.bottom
        anchors.bottom: screen.bottom
        anchors.left: screen.left
        anchors.right: screen.right
        color: "#aaccee"

        Viewport  {
            id: view
            anchors.fill: parent
            showSceneGraph: true
            Cylinder {
                x: -2.2
                y: 1.5
                transform: Rotation3D { axis: "1, 0, 0"; angle: 90 }
                length: 3
                effect: Effect { color: "green" }
            }
            Sphere {
                effect: Effect { color: "burlywood" }
            }
            Capsule {
                x: 2.2
                length: 3
                transform: Rotation3D { axis: "1, 0, 0"; angle: 90 }
                effect: Effect { color: "red" }
            }
        }
    }
}
