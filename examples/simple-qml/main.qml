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

    // Scene graph
    Node {
        id: sceneRoot
        objectName: "sceneRoot"

        Entity {
            id: mainCamera
            objectName: "mainCamera"

            property Transform transform: Transform {
                transforms: LookAt {
                    position:   Qt.vector3d(0.0, 2.0, 0.0)
                    viewCenter: Qt.vector3d(0.0, 0.0, 0.5)
                    upVector:   Qt.vector3d(0.0, 1.0, 0.0)
                }
            }

            property Camera camera: Camera {
                objectName: "cameraComponent"
                projectionType: Camera.PerspectiveProjection
                fieldOfView: 22.5
                aspectRatio: 1920.0 / 1080.0
            }

            QQ2.Timer {
                id: timer
                objectName: "timer"
            }
        }

        AdsEffect {
            id: adsEffect
            objectName: "adsEffect"
        }

        Mesh {
            id: ballMesh
            objectName: "ballMesh"
            source: ":/assets/ball.obj"
        }

        Material {
            id: ballMaterial
            objectName: "ballMaterial"
            effect: adsEffect

            // Custom properties go here
        }

        Entity {
            id: ball
            objectName: "ball"

            property Transform transform: Transform { Translate{ dx: 0; dy: 100 } }
            property Mesh mesh: ballMesh
            property Material material: ballMaterial
        }

        Entity {
            id: ball1
            objectName: "ball1"

            property Transform transform: Transform { Translate{ dx: 0; dy: 50 } }
            property Mesh mesh: ballMesh
            property Material material: ballMaterial
        }

        Entity {
            id: ball2
            objectName: "ball2"

            property Transform transform: Transform {
                Translate {
                    id: ball2Translation
                    dx: 0; dy: 0

                    QQ2.SequentialAnimation {
                        running: true
                        loops: QQ2.Animation.Infinite

                        QQ2.NumberAnimation {
                            target: ball2Translation
                            property: "dx"
                            duration: 1000
                            easing.type: Easing.InOutQuad
                            from: 0; to: 100
                        }
                        QQ2.NumberAnimation {
                            target: ball2Translation
                            property: "dx"
                            duration: 1000
                            easing.type: Easing.InOutQuad
                            from: 100; to: 0
                        }
                    }
                }
            }
            property Mesh mesh: ballMesh
            property Material material: ballMaterial
        }
    }
}

//Object {
//    id: root

//    Object {
//        id: scene // 3D Scene

//        property var camera: Camera{
//            id: mainCamera
//            position: Qt.vector3d( 0.0, 1.0, 5.0 )
//            upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
//            viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
//            effect: simpleEffect
//        }

//        Mesh {
//            id: mesh
//            source: ":/meshes/toyplane.obj"
//        }

//        SimpleEffect {
//            id: simpleEffect
//        }
//    }

//    Object {
//        id: ui // 2D UI
//    }

//    Object {
//        id: simulationLogic // Other random "stuff"
//    }
//}

