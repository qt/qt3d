/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
import QtQuick 2.0 as QQ2

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
                    Parameter { name: "color"; value : gBuffer.color },
                    Parameter { name: "position"; value : gBuffer.position },
                    Parameter { name: "normal"; value : gBuffer.normal },
                    Parameter { name: "winSize"; value : Qt.size(1024, 1024) }
                ]
                effect : FinalEffect {}
            }
        ]

    }

    Entity {
        id : sceneEntity

        property PointLight light: PointLight {
            color : "white"
            intensity : 4.0
            QQ2.ColorAnimation on color { from: "white"; to: "blue"; duration: 4000; loops: 2 }
            QQ2.NumberAnimation on intensity { from: 0; to: 5.0; duration: 1000; loops: QQ2.Animation.Infinite }
        }

        components: sceneEntity.light

        Camera {
            id: camera
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 45
            aspectRatio: 16/9
            nearPlane : 0.01
            farPlane : 1000.0
            position: Qt.vector3d( 0.0, 0.0, -25.0 )
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
            shareable: false
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
                Translate{ dx: -10; dy: 0; dz : 5
                    id: sphere1Translate
                }
            }
            QQ2.SequentialAnimation {
                loops: QQ2.Animation.Infinite
                running: false
                QQ2.NumberAnimation {target: sphere1Translate; property: "dx"; to: 6; duration: 2000 }
                QQ2.NumberAnimation {target: sphere1Translate; property: "dx"; to: -10; duration: 2000 }
            }

            property PointLight light : PointLight {
                color : "green"
                intensity : 5.0
            }

            components : [
                sphereMesh,
                material,
                sphere1.transform,
                sphere1.light,
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
                color : "orange"
                intensity : 2.0
            }

            property Transform transform : Transform {
                Translate{ dx: 5; dy: 0; dz : 5}
            }

            components : [
                sphereMesh,
                sphere2.transform,
                material,
                sphere2.light,
                sceneLayer
            ]
        }

        Entity {
            id: light3
            property PointLight light : PointLight {
                color : "white"
                intensity : 2.0
            }

            property Material material : Material {
                effect : sceneMaterialEffect
                parameters : Parameter { name : "meshColor"; value : "red" }
            }

            property Transform transform : Transform {
                Translate{ id: light3Translate; dx: 2; dy: 2; dz : 7
                }
            }
            QQ2.SequentialAnimation {
                loops: QQ2.Animation.Infinite
                running: true
                QQ2.NumberAnimation {target: light3Translate; property: "dy"; to: 6; duration: 1000; easing.type: QQ2.Easing.InOutQuad }
                QQ2.NumberAnimation {target: light3Translate; property: "dy"; to: -6; duration: 1000; easing.type: QQ2.Easing.InOutQuint }
            }

            components: [
                sphereMesh,
                material,
                light,
                transform,
                sceneLayer
            ]
        }

        Entity {
            id: light4
            property PointLight light : PointLight {
                color : "white"
                intensity : 3.0
            }
            property Transform transform : Transform {
                Translate{ dx: 5; dy: 2; dz : 7}
            }

            components: [
                light4.light,
                light4.transform,
                sceneLayer
            ]
        }
    }
}
