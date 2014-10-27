/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0
import Qt3D.Render 2.0

// For Qt.vector3d() and friends. For some reason this is provided by
// QQuickValueTypeProvider in QtQuick rather than the default value
// type provider in QtQml. So we will need to replicate this in Qt3D
// for the types that we wish to support. Otherwise we'll have to import
// QtQuick 2.1 all over the place.
import QtQuick 2.1 as QQ2

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -20.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration  {
        controlledCamera: camera
    }

    FrameGraph {
        id : external_forward_renderer
        activeFrameGraph : ForwardRenderer {
            camera: camera
        }
    }

    components: [external_forward_renderer]

    CylinderMesh {
        id: mesh
        radius: 1
        length: 3
        rings: 100
        slices: 20
    }

    Transform {
        id: transform
        Scale { scale3D: Qt.vector3d(1.5, 1.5, 1.5) }
        Rotate {
            angle: 45
            axis: Qt.vector3d(1, 0, 0)
        }
    }

    Material {
        id: material
        effect : Effect {
        }
    }

    Entity {
        id: mainEntity
        objectName: "mainEntity"
        components: [ mesh, material, transform ]
    }
}
