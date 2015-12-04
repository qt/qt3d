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

import QtQuick 2.0 as Quick
import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity
{
    components: FrameGraph {
        activeFrameGraph: ForwardRenderer {
            clearColor: Qt.rgba(0, 0, 0, 1)
            camera: camera
        }
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 50.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, -1.0 )
    }

    Configuration  {
        controlledCamera: camera
    }

    Entity {
        components: [
            SphereMesh {
                radius: 2
            },
            Transform {
                translation: Qt.vector3d(2.0, 8.0, -2.0)
                Quick.SequentialAnimation on translation.y {
                    loops: Quick.Animation.Infinite
                    Quick.NumberAnimation { from: 8.0; to: 40.0; duration: 3000 }
                    Quick.NumberAnimation { from: 40.0; to: 8.0; duration: 3000 }
                }
            },
            PointLight {
                color: Qt.rgba(1, 0, 0, 1)
            }
        ]
    }

    Entity {
        components: [
            SphereMesh {
                radius: 2
            },
            Transform {
                translation: Qt.vector3d(0.0, 3.0, 4.0)
                Quick.SequentialAnimation on translation.z {
                    loops: Quick.Animation.Infinite
                    Quick.NumberAnimation { from: 4.0; to: 40.0; duration: 5000 }
                    Quick.NumberAnimation { from: 40.0; to: 4.0; duration: 5000 }
                }
            },
            PointLight {
                color: Qt.rgba(0, 1, 0, 1)
            }
        ]
    }

    Entity {
        components: [
            Mesh {
                source: "assets/obj/toyplane.obj"
            }
        ]
    }
}
