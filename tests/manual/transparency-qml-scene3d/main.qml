// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Scene3D 2.0

Window {
    visible: true
    width: 1024
    height: 768
    color: "black"

    Item {
        anchors.fill: parent
        Scene3D {
            id: scene3D
            anchors.fill: parent
            focus: true
            aspects: ["input", "logic"]
            Scene {}
        }
    }
}
