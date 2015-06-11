/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
import QtQuick 2.0

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 40
        aspectRatio: 4/3
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, viewCenter_z.value )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration  {
        controlledCamera: camera
    }

    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                camera: camera
                clearColor: "white"
            }
        }
    ]

    PhongMaterial {
        id: material
        ambient: Qt.rgba( color_r.value/255, color_g.value/255, color_b.value/255, 1.0 )
        diffuse: Qt.rgba( 0.1, 0.1, 0.1, 0.5 )
        shininess: shining.value
    }

    Transform {
        id: logoTransform

        Rotate {
            id: rotate_x
            angle: rotation_x.value
            axis: Qt.vector3d(1, 0, 0)
        }
        Rotate {
            id: rotate_y
            angle: rotation_y.value
            axis: Qt.vector3d(0, 1, 0)
        }
        Rotate {
            id: rotate_z
            angle: rotation_z.value
            axis: Qt.vector3d(0, 0, 1)
        }
    }

    Mesh {
        id: logoMesh
        source: "Qt_logo.obj"
    }

    Entity {
        id: logoEntity
        components: [ logoMesh, material, logoTransform ]
    }
}
