// Copyright (C) 2023 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0
import QtQuick.Scene3D 2.0

Scene3D {
    id: scene3d

    focus: true
    aspects: ["input", "logic", "render"]
    cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

    TextScene {
        id: textScene
    }
}
