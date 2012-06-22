/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0

// QTBUG-17408
Item {
    id: screen
    width: aTestCase.defaultWidth * 3
    height: aTestCase.defaultHeight * 3

    Flow {
        anchors.fill: parent

        RotationTestCase {
            id: aTestCase
            text: "Rotation: X axis\n(Rolling towards you)"
            rotationAxis: Qt.vector3d(1,0,0)
        }

        RotationTestCase {
            text: "Rotation on Y Axis\n(Cube front rotating left to right)"
            rotationAxis: Qt.vector3d(0,1,0)
        }

        RotationTestCase {
            text: "Rotation: Z axis\n(Rolling to the left)"
            rotationAxis: Qt.vector3d(0,0,1)
        }


        RotationTestCase {
            text: "Rotation: -X axis\n(Rolling away)"
            rotationAxis: Qt.vector3d(-1,0,0)
        }

        RotationTestCase {
            text: "Rotation on -Y Axis\n(Cube front rotating right to left)"
            rotationAxis: Qt.vector3d(0,-1,0)
        }

        RotationTestCase {
            text: "Rotation: -Z axis\n(Rolling to the right)"
            rotationAxis: Qt.vector3d(0,0,-1)
        }

        RotationTestCase {
            text: "Rotation: 3 axis\n(top-right-front corner stationary)"
            rotationAxis: Qt.vector3d(1,1,1)
        }

        RotationTestCase {
            text: "Axis Normalization\n(Identical to previous case)"
            rotationAxis: Qt.vector3d(12.3,12.3,12.3)
        }

        RotationTestCase {
            text: "Degenerate Axis\n(Anything but a crash is ok)"
            rotationAxis: Qt.vector3d(0,0,0)
        }
    }
}
