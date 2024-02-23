// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.1 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root
    property Material material

    Mesh {
        id: toyplaneMesh
        source: "qrc:///assets/obj/toyplane.obj"
    }

    Transform {
        id: toyplaneTransform

        property real rollAngle: 0
        property real pitchAngle: 15
        property real altitude: 5
        property real angle: 0
        property real scaleFactor: 10

        QQ2.Behavior on rollAngle { QQ2.SpringAnimation { spring: 2; damping: 0.2} }

        matrix: {
            var m = Qt.matrix4x4();
            m.translate(Qt.vector3d(Math.sin(angle * Math.PI / 180) * scaleFactor,
                                    altitude,
                                    Math.cos(angle * Math.PI / 180) * scaleFactor));
            m.rotate(angle, Qt.vector3d(0, 1, 0));
            m.rotate(pitchAngle, Qt.vector3d(0, 0, 1));
            m.rotate(rollAngle, Qt.vector3d(1, 0, 0));
            m.scale(1.0 / toyplaneTransform.scaleFactor);
            return m;
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
