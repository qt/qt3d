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

Node {
    id: root
    objectName: "root"

    QQ2.Timer
    {
        property bool test : false;
        interval : 1500
        repeat : true
        running: true
        onTriggered:
        {
            console.log("Triggered <<<<<<<<<<<<<<<<<<<<< " + test);
            camera2.transform = test ? transform_2 : transform_1
            ball2.mesh = test ? null : ballMesh
            ball1.mesh = test ? cubeMesh : ballMesh
            test = !test
        }
    }

    // Scene graph
    Entity {
        id: sceneRoot
        objectName: "sceneRootTest"

        FrameGraph {
            id : external_forward_renderer
            activeFrameGraph : ForwardRenderer {
                objectName : "externalRenderer"
                cameraViewportBottomLeft: camera1
                cameraViewportTopRight: camera2
            }
        }

        components: [external_forward_renderer]

        Transform {
            id : transform_0
            objectName : "transform_0"

            Translate {
                id : translate_0
            }
            LookAt {
                position: Qt.vector3d( -2.0, -1.0, -18.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 5.0 )
            }
        }
        QQ2.SequentialAnimation {
            running : true
            loops: QQ2.Animation.Infinite
            QQ2.NumberAnimation { target : translate_0; property : "dz"; to : -360; duration : 2000 }
            QQ2.NumberAnimation { target : translate_0; property : "dz"; to : -20; duration : 2000 }
        }

        Transform {
            id : transform_1
            objectName : "transform_1"

            Rotate {
                id : rotate
                axis : Qt.vector3d(0, 1, 0)
                angle: 0
            }
            LookAt {
                position: Qt.vector3d( -2.0, -1.0, -18.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 5.0 )
            }
        }
        Transform {
            id : transform_2
            objectName : "transform_2"

            Rotate {
                axis : Qt.vector3d(1, 0, 0)
                angle: rotate.angle
            }
            LookAt {
                position: Qt.vector3d( -2.0, -1.0, -18.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 5.0 )
            }
        }
        QQ2.SequentialAnimation {
            running : true
            loops: QQ2.Animation.Infinite
            QQ2.NumberAnimation { target : rotate; property : "angle"; to : 360; duration : 2000 }
        }

        CameraLens {
            id : default_lens
            objectName : "default_lens"
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 45
            aspectRatio: 16/9
            nearPlane : 0.01
            farPlane : 1000.0
        }

        Camera {
            id: camera1
            objectName: "mainCamera"
            lens : CameraLens {
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: 16/9
                nearPlane : 0.01
                farPlane : 1000.0
            }

            transform : Transform {
                LookAt {
                    position: Qt.vector3d( 10.0, 10.0, -25.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 0.0, 0.0, 10.0 )
                }
                Rotate {
                    angle : 0
                    axis : Qt.vector3d(0, 1, 0)
                }
            }
        }

        Configuration  {
            controlledCamera: camera1
        }

        Entity {
            id: camera2
            objectName: "camera2"

            property Transform transform : transform_1
            components : [default_lens, transform]
        }
        Entity {
            id: ball2
            objectName: "ball2"

            property Transform transform : ball2Transform;
            property Mesh mesh: ballMesh
            property Material material: ballMaterial
            components : [mesh, ballMaterial, transform]
        }


        //        Camera {
        //            id : camera2
        //            objectName : "camera2"
        //            lens : CameraLens {
        //                projectionType: CameraLens.PerspectiveProjection
        //                fieldOfView: 45
        //                aspectRatio: 16/9
        //                nearPlane : 0.01
        //                farPlane : 1000.0
        //            }

        //            transform : transform_1

        //        }

        //        AdsEffect {
        //            id: adsEffect
        //            objectName: "adsEffect"
        //        }

        Mesh {
            id: ballMesh
            objectName: "ballMesh"
            source: ":/assets/ball.obj"
        }

        Mesh {
            id : cubeMesh
            objectName: "cubeMesh"
            source : ":/assets/cube.obj"
        }

        Material {
            id: ballMaterial
            objectName: "ballMaterial"
            //            effect: adsEffect

            parameters : [
                Parameter { name : "ambient"; datatype: Parameter.FloatVec4; value : Qt.vector4d(255, 0, 0, 1) },
                Parameter { name : "position"; datatype: Parameter.FloatVec4; value : Qt.vector4d(0, 0, 0, 0); meshAttributeName: "position" }
            ]

            effect : Effect {

                techniques : [

                    Technique {
                        criteria : [
                             TechniqueCriterion { criterionType : TechniqueCriterion.RenderingStyle; criterionValue : "forward"}
                        ]

                        renderPasses : [

                            RenderPass {

                                criteria : [

                                ]

                            }

                        ]
                    }
                ]
            }


            // Custom properties go here
        }

        Scene
        {
            id: scene
            source: ":/assets/test_scene.dae"
            objectName: "dae_scene"
        }

        Entity {
            id: ball
            objectName: "ball"

            property Transform transform: Transform {
                Translate{ dx: 0; dy: -10; dz : 25 }
                Scale {scale : 0.3}
            }
            property Mesh mesh: ballMesh
            property Material material: ballMaterial

            components : [
                transform,
                ballMesh,
                ballMaterial
            ]
        }

        Entity {
            id: ball1
            objectName: "ball1"

            property Transform transform: Transform {
                Scale { scale : 0.5 }
                Translate{ dx: 8; dy: 8; dz : 30 }
            }
            property Mesh mesh: ballMesh
            property Material material: ballMaterial

            components: [
                transform,
                mesh,
                material
            ]
        }

        QQ2.SequentialAnimation {
            running: true
            loops: QQ2.Animation.Infinite

            QQ2.NumberAnimation {
                target: ball2Translation
                property: "dx"
                duration: 1000
                easing.type: QQ2.Easing.InOutQuad
                from: 0; to: 10
            }
            QQ2.NumberAnimation {
                target: ball2Translation
                property: "dx"
                duration: 1000
                easing.type: QQ2.Easing.InOutQuad
                from: 10; to: 0
            }
        }

        Transform {
            id: ball2Transform
            Scale { scale : 0.35 }
            Translate {
                id: ball2Translation
                dx: 0; dy: 0; dz : 40
            }
        }
    }
}
