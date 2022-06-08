// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

import QtQuick 2.1 as QQ2
import Qt3D 2.0

FrameGraphConfiguration {

    property alias viewportRect : viewport.viewportRect
    property alias camera : camera_selector.camera

    TechniqueFilter {

        Viewport {
            id : viewport

            CameraSelector {
                id : camera_selector
            }
        }
    }
}
