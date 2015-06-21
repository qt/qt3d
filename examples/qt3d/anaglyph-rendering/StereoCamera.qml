/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

Entity {
    id: root
    property real convergence: 2000.0
    property real eyeSeparation: 35.0
    property real aspectRatio: _window.width / _window.height
    property real fieldOfView: 60.0
    property real nearPlane: 10.0
    property real farPlane: 10000.0

    property alias viewCenter: eyeLookAt.viewCenter
    property alias position: eyeLookAt.position

    readonly property real _fov2: Math.tan(fieldOfView * Math.PI / 180 * 0.5)
    readonly property real top: nearPlane * _fov2
    readonly property real a: aspectRatio * _fov2 * convergence

    CameraLens {
        id: leftEyeLens
        projectionType: CameraLens.FrustumProjection
        nearPlane : root.nearPlane
        farPlane : root.farPlane
        left: -(a - eyeSeparation * 0.5) * nearPlane / convergence
        right: (a + eyeSeparation * 0.5) * nearPlane / convergence
        top: root.top
        bottom: -root.top
    }

    CameraLens {
        id: rightEyeLens
        projectionType: CameraLens.FrustumProjection
        nearPlane : root.nearPlane
        farPlane : root.farPlane
        left: -(a + eyeSeparation * 0.5) * nearPlane / convergence
        right: (a - eyeSeparation * 0.5) * nearPlane / convergence
        top: root.top
        bottom: -root.top
    }

    Transform {
        id: eyeTransform
        LookAt {
            id: eyeLookAt
            upVector: Qt.vector3d(0.0, 1.0, 0.0)
            viewCenter: root.viewCenter
            position: root.position
        }
    }

    property Entity leftCamera: Entity {
        components: [
            leftEyeLens,
            eyeTransform
        ]
    }

    property Entity rightCamera: Entity {
        id: rightCameraEntity
        components: [
            rightEyeLens,
            eyeTransform
        ]
    }
}
