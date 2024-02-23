// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property vector3d pos: Qt.vector3d(0.0, 0.0, 0.0)
    property vector3d viewCenter: Qt.vector3d(0.0, 0.0, -10.0)
    property vector3d up: Qt.vector3d(0.0, 1.0, 0.0)

    components: [ transform, lens ]

    Transform {
        id: transform

        matrix: {
            var m = Qt.matrix4x4();
            m.lookAt( root.pos, root.viewCenter, root.up );
            return m;
        }
    }

    CameraLens {
        id: lens
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _view.width / _view.height
        nearPlane: 0.1
        farPlane: 1000.0
    }
}
