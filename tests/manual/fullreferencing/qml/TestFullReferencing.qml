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
    id: screen
    width: 640
    height: 520
    objectName: "toplevelrect"
    color: "black"
    Item3D {
        anchors.fill: parent
        id: solo
        objectName: "soloItem"
        y: 1
        scale: 0.3
        transform: [ Rotation3D { axis: "1,0,0"; angle: -90 }, Rotation3D { axis: "0,1,0"; angle: 60 }]
        mesh: Mesh { source: "meshes/robot.3ds"; dumpInfo: true; options: "ForceFaceted" }
    }
    Rectangle {
        id: instructions
        height: 80
        width: parent.width
        anchors.bottom: parent.bottom
        color: "#3333aa"
        opacity: 0.5
        Text {
            id: instructionsText
            wrapMode: "WordWrap"
            text: "Pick a cube, check log output. Drag in the black area to move all cubes."
            anchors.centerIn: parent
        }
    }
}
