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
    id : root

    GBuffer {
        id : gBuffer
    }

    components : FrameGraph {
        id : deferredFrameGraphComponent
        activeFrameGraph: DeferredRenderer {
            camera : camera
            gBuffer: gBuffer
        }
    }

    Configuration  {
        controlledCamera: camera
    }

    Entity {
        id : screenQuadEntity
        components : [
            Layer {
                names : "screenQuad"
            },
            PlaneMesh {
                width: 2.0
                height: 2.0
                meshResolution: Qt.size(2, 2)
            },
            Transform { // We rotate the plane so that it faces us
                Rotate {
                    axis : Qt.vector3d(1.0, 0.0, 0.0)
                    angle : 90
                }
            },
            Material {
                parameters : [
                    Parameter { name : "color"; value : gBuffer.color },
                    Parameter { name : "position"; value : gBuffer.position },
                    Parameter { name : "normal"; value : gBuffer.normal },
                    Parameter { name : "winSize"; value : Qt.size(1024, 1024) }
                ]
                effect : FinalEffect {}
            }
        ]

    }

    Entity {
        id : sceneEntity

        components : PointLight {
            color : "red"
            intensity : 3.0
        }

        Camera {
            id: camera
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 45
            aspectRatio: 16/9
            nearPlane : 0.01
            farPlane : 1000.0
            position: Qt.vector3d( 10.0, 10.0, -25.0 )
            upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
            viewCenter: Qt.vector3d( 0.0, 0.0, 10.0 )
        }

        Layer {
            id : sceneLayer
            names : "scene"
        }

        SphereMesh {
            id : sphereMesh
            rings: 50
            slices: 100
        }

        SceneEffect {
            id : sceneMaterialEffect
        }

        Entity {
            id : sphere1

            property Material material : Material {
                effect : sceneMaterialEffect
                parameters : Parameter { name : "meshColor"; value : "dodgerblue" }
            }

            property Transform transform : Transform {
                Translate{ dx: -10; dy: 0; dz : 25 }
            }

            property PointLight light : PointLight {
                color : "yellow"
                intensity : 1.0
            }

            components : [
                sphereMesh,
                material,
                transform,
                light,
                sceneLayer
            ]
        }

        Entity {
            id : sphere2

            property Material material : Material {
                effect : sceneMaterialEffect
                parameters : Parameter { name : "meshColor"; value : "green" }
            }

            property PointLight light : PointLight {
                color : "white"
                intensity : 3.0
            }

            components : [
                sphereMesh,
                material,
                light,
                sceneLayer
            ]
        }

    }
}
