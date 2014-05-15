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

import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Render 2.0


Node {
    id : rootNode

    CameraLens {
        id : cameraLens
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.01
        farPlane : 1000.0
    } // cameraLens

    Entity {
        id : sceneRoot
        components: [frameGraph]
        property real rotationAngle : 0

        SequentialAnimation {
            running : true
            loops: Animation.Infinite
            NumberAnimation {target : sceneRoot; property : "rotationAngle"; to : 360; duration : 2000;}
        }

        FrameGraph {
            id : frameGraph
            activeFrameGraph: Viewport {
                id : mainViewport
                rect: Qt.rect(0, 0, 1, 1)

                property var cameras : [cameraViewport1, cameraViewport2, cameraViewport3, cameraViewport4]

                Timer {
                    running : true
                    interval : 2000
                    repeat : true
                    property int count : 0
                    onTriggered:
                    {
                        cameraSelectorTopLeftViewport.camera = mainViewport.cameras[count++ % 4];
                        cameraSelectorTopRightViewport.camera = mainViewport.cameras[count % 4];
                        cameraSelectorBottomLeftViewport.camera = mainViewport.cameras[(count + 1) % 4];
                        cameraSelectorBottomRightViewport.camera = mainViewport.cameras[(count + 2) % 4];
                    }
                }

                Viewport {
                    id : topLeftViewport
                    rect : Qt.rect(0, 0, 0.5, 0.5)
                    CameraSelector {id : cameraSelectorTopLeftViewport; camera : mainViewport.cameras[0]}
                }

                Viewport {
                    id : topRightViewport
                    rect : Qt.rect(0.5, 0, 0.5, 0.5)
                    CameraSelector {id : cameraSelectorTopRightViewport;camera : mainViewport.cameras[1]}
                }

                Viewport {
                    id : bottomLeftViewport
                    rect : Qt.rect(0, 0.5, 0.5, 0.5)
                    CameraSelector {id : cameraSelectorBottomLeftViewport; camera : mainViewport.cameras[2]}
                }

                Viewport {
                    id : bottomRightViewport
                    rect : Qt.rect(0.5, 0.5, 0.5, 0.5)
                    CameraSelector {id : cameraSelectorBottomRightViewport; camera : mainViewport.cameras[3]}
                }
            } // mainViewport
        } // frameGraph

        Entity {
            id : cameraViewport1
            property Transform transform : Transform {
                LookAt {
                    position: Qt.vector3d( 0.0, 0.0, -10.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
                }
            }
            components : [cameraLens, transform]
        }

        Entity {
            id : cameraViewport2
            property Transform transform : Transform {
                Rotate {
                    angle : sceneRoot.rotationAngle
                    axis : Qt.vector3d(0, 1, 0)
                }
                LookAt {
                    position: Qt.vector3d( 0.0, 0.0, 20.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 5.0, 0.0, 10.0 )
                }
            }
            components : [cameraLens, transform]
        }

        Entity {
            id : cameraViewport3
            property Transform transform : Transform {
                LookAt {
                    position: Qt.vector3d( 0.0, 0.0, -20.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( -5.0, 0.0, -10.0 )
                }
            }
            components : [cameraLens, transform]
        }

        Entity {
            id : cameraViewport4
            property Transform transform : Transform {
                LookAt {
                    position: Qt.vector3d( 0.0, 5.0, -20.0 )
                    upVector: Qt.vector3d( 0.0, 0.0, 1.0 )
                    viewCenter: Qt.vector3d( 0.0, 30.0, 20.0 )
                }
            }
            components : [cameraLens, transform]
        }


        Scene {
            id: scene
            source: ":/assets/test_scene.dae"
            components : [Transform {
                    Rotate {
                        angle : -sceneRoot.rotationAngle
                        axis : Qt.vector3d(0, 0, 1)
                    }
                }]
        }

    } // sceneRoot

} // rootNode
