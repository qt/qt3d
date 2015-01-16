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

Entity {
    id: root
    objectName: "root"

    // Use the renderer configuration specified in ForwardRenderer.qml
    // and render from the mainCamera
    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
                clearColor: "black"
            }
        }
    ]

    BasicCamera {
        id: mainCamera
        position: Qt.vector3d( 0.0, 20.0, 70.0 )
    }

    Configuration  {
        controlledCamera: mainCamera
    }

    AdsEffect {
        id: adsEffect
    }

    PlaneEntity {
        id: floor
        scale: 50
        material: AdsMaterial {
            effect: adsEffect
            ambient: Qt.rgba( 1.0, 1.0, 1.0, 1.0 )
            diffuse: Qt.rgba( 1.0, 1.0, 1.0, 1.0 )
        }
    }

    AdsMaterial {
        id: wallMaterial
        effect: adsEffect
        ambient: Qt.rgba( 0.1, 0.0, 0.1, 1.0 )
        diffuse: Qt.rgba( 1.0, 0.0, 1.0, 1.0 )
    }

    BoxEntity {
        id: eastWall
        width: 3
        height: 4
        depth: floor.scale + 2 * width
        x: (floor.scale + width) / 2.0
        y: height / 2
        material: wallMaterial
    }

    BoxEntity {
        id: westWall
        width: 3
        height: 4
        depth: floor.scale + 2 * width
        x: -(floor.scale + width) / 2.0
        y: height / 2
        material: wallMaterial
    }

    BoxEntity {
        id: northWall
        width: 3
        height: 4
        depth: floor.scale
        z: (floor.scale + width) / 2.0
        y: height / 2
        angle: 90
        material: wallMaterial
    }

    BoxEntity {
        id: southWall
        width: 3
        height: 4
        depth: floor.scale
        z: -(floor.scale + width) / 2.0
        y: height / 2
        angle: 90
        material: wallMaterial
    }

    AdsMaterial {
        id: redMaterial
        effect: adsEffect
        ambient: Qt.rgba( 0.2, 0.0, 0.0, 1.0 )
        diffuse: Qt.rgba( 0.8, 0.0, 0.0, 1.0 )
    }

    AdsMaterial {
        id: greenMaterial
        effect: adsEffect
        ambient: Qt.rgba( 0.0, 0.2, 0.0, 1.0 )
        diffuse: Qt.rgba( 0.0, 0.8, 0.0, 1.0 )
    }

    AdsMaterial {
        id: blueMaterial
        effect: adsEffect
        ambient: Qt.rgba( 0.0, 0.0, 0.2, 1.0 )
        diffuse: Qt.rgba( 0.0, 0.0, 0.8, 1.0 )
    }

    BallEntity {
        id: redBall
        material: redMaterial
        x: -5
    }

    BallEntity {
        id: greenBall
        material: greenMaterial
    }

    BallEntity {
        id: blueBall
        material: blueMaterial
        x: 5
    }
}
