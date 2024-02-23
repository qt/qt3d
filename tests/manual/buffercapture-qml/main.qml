// Copyright (C) 2017 Juan José Casafranca.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick.Scene3D 2.0

Item {
    width: 128
    height: width
    Scene3D {
        anchors.fill: parent
        BufferSetterScene {
            id: scene
        }
    }
}
