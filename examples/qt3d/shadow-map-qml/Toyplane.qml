/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2

Entity {
    id: root
    property Material material

    Mesh {
        id: toyplaneMesh
        source: "assets/obj/toyplane.obj"
    }

    Transform {
        id: toyplaneTransform

        property real rollAngle : 0
        property real pitchAngle : 15
        property real altitude : 5
        property real angle: 0
        property real scaleFactor: 10

        QQ2.Behavior on rollAngle { QQ2.SpringAnimation { spring: 2; damping: 0.2} }

        Scale {
            scale: 1.0 / toyplaneTransform.scaleFactor
        }

        Rotate { // roll
            axis : Qt.vector3d(1, 0, 0)
            angle : toyplaneTransform.rollAngle
        }

        Rotate { // pitch
            axis : Qt.vector3d(0, 0, 1)
            angle : toyplaneTransform.pitchAngle
        }

        Rotate {
            id: toyplaneRotation
            axis: Qt.vector3d(0, 1, 0)
            angle: toyplaneTransform.angle
        }

        Translate {
            property real translation: 1

            dx: Math.sin(toyplaneTransform.angle * Math.PI / 180) * translation * toyplaneTransform.scaleFactor
            dy: toyplaneTransform.altitude
            dz: Math.cos(toyplaneTransform.angle * Math.PI / 180) * translation * toyplaneTransform.scaleFactor
        }
    }

    QQ2.NumberAnimation {
        target: toyplaneTransform

        running: true
        loops: QQ2.Animation.Infinite

        property: "angle"
        duration: 10000
        from: 0
        to: 360
    }

    // Altitude / Pitch animation
    QQ2.SequentialAnimation {
        running: true
        loops: QQ2.Animation.Infinite
        QQ2.ParallelAnimation {
            QQ2.SequentialAnimation {
                QQ2.NumberAnimation { target: toyplaneTransform; property: "pitchAngle"; from: 0; to: 30; duration: 2000; easing.type: QQ2.Easing.OutQuad }
                QQ2.NumberAnimation { target: toyplaneTransform; property: "pitchAngle"; from: 30; to: 0; duration: 2000; easing.type: QQ2.Easing.OutSine }
            }
            QQ2.NumberAnimation { target: toyplaneTransform; property: "altitude"; to: 5; duration: 4000; easing.type: QQ2.Easing.InOutCubic }
        }
        QQ2.PauseAnimation { duration: 1500 }
        QQ2.ParallelAnimation {
            QQ2.SequentialAnimation {
                QQ2.NumberAnimation { target: toyplaneTransform; property: "pitchAngle"; from: 0; to: -30; duration: 1000; easing.type: QQ2.Easing.OutQuad }
                QQ2.NumberAnimation { target: toyplaneTransform; property: "pitchAngle"; from: -30; to: 0; duration: 5000; easing.type: QQ2.Easing.OutSine }
            }
            QQ2.NumberAnimation { target: toyplaneTransform; property: "altitude"; to: 0; duration: 6000; easing.type: QQ2.Easing.InOutCubic}
        }
        QQ2.PauseAnimation { duration: 1500 }
    }

    // Roll Animation
    QQ2.SequentialAnimation {
        running: true
        loops: QQ2.Animation.Infinite
        QQ2.NumberAnimation { target: toyplaneTransform; property: "rollAngle"; to: 360; duration: 1500; easing.type: QQ2.Easing.InOutQuad }
        QQ2.PauseAnimation { duration: 1000 }
        QQ2.NumberAnimation { target: toyplaneTransform; property: "rollAngle"; from: 0; to: 30; duration: 1000; easing.type: QQ2.Easing.OutQuart }
        QQ2.PauseAnimation { duration: 1500 }
        QQ2.NumberAnimation { target: toyplaneTransform; property: "rollAngle"; from: 30; to: -30; duration: 1000; easing.type: QQ2.Easing.OutQuart }
        QQ2.PauseAnimation { duration: 1500 }
        QQ2.NumberAnimation { target: toyplaneTransform; property: "rollAngle"; from: -30; to: 0; duration: 750; easing.type: QQ2.Easing.OutQuart }
        QQ2.PauseAnimation { duration: 2000 }
    }

    components: [
        toyplaneMesh,
        toyplaneTransform,
        material
    ]
}
