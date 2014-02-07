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

// QTBUG-17408
Item {
    id: screen
    width: aTestCase.defaultWidth * 3
    height: aTestCase.defaultHeight * 4
    property variant scalingCamera: Camera {eye: Qt.vector3d(0,20,20) }

    Flow {
        anchors.fill: parent

        ScalingTestCase {
            id: aTestCase
            text: "Scaling up"
            camera: scalingCamera
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(4,4,4)
        }

        ScalingTestCase {
            text: "Scaling down"
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(0.1,0.1,0.1)
        }

        ScalingTestCase {
            text: "Scaling using real\n(scaling and inversion)"
            testTransform: Scale3D {}
            camera: scalingCamera
            scaleFactor: 1
            SequentialAnimation on scaleFactor {
                loops: Animation.Infinite
                PropertyAnimation {
                    from: -3
                    to: 3
                    duration: 1500
                }
                PropertyAnimation {
                    from: 3
                    to: -3
                    duration: 1500
                }
            }
        }

        ScalingTestCase {
            text: "Scaling in X\n(mirroring horizontally)"
            camera: scalingCamera
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(4,1,1)
        }

        ScalingTestCase {
            text: "Scaling in Y\n(stretching vertically)"
            camera: scalingCamera
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(1,4,1)
        }

        ScalingTestCase {
            text: "Scaling in Z\n(depth-wise)"
            camera: scalingCamera
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(1,1,4)
        }

        ScalingTestCase {
            text: "Negative Scaling in X\n(mirroring horizontally)"
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(-1,1,1)
        }

        ScalingTestCase {
            text: "Negative Scaling in Y\n(flipping upside-down)"
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(1,-1,1)
        }

        ScalingTestCase {
            text: "Negative Scaling in Z\n(depth-wise)"
            scaleVectorA: Qt.vector3d(1,1,1)
            scaleVectorB: Qt.vector3d(1,1,-1)
        }

        ScalingTestCase {
            text: "Zero Z\n(Probable artifacts)"
            scaleVectorA: Qt.vector3d(1,1,0)
            scaleVectorB: Qt.vector3d(1,1,0)
        }

        ScalingTestCase {
            text: "Zero Y\n(Probable artifacts)"
            scaleVectorA: Qt.vector3d(1,0,1)
            scaleVectorB: Qt.vector3d(1,0,1)
        }

        // Skipping zero X because it would be invisible  with the
        // camera position all the other tests have

        ScalingTestCase {
            text: "Zero Vector\n(Not visible and shouldn't crash)"
            scaleVectorA: Qt.vector3d(0,0,0)
            scaleVectorB: Qt.vector3d(0,0,0)
        }
    }
}
