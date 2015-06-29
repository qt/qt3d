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

import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Renderer 2.0


Entity {
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

        property var cameras : [cameraViewport1, cameraViewport2, cameraViewport3, cameraViewport4]

        Timer {
            running : true
            interval : 10000
            repeat : true
            property int count : 0
            onTriggered:
            {
                cameraSelectorTopLeftViewport.camera = sceneRoot.cameras[count++ % 4];
                cameraSelectorTopRightViewport.camera = sceneRoot.cameras[count % 4];
                cameraSelectorBottomLeftViewport.camera = sceneRoot.cameras[(count + 1) % 4];
                cameraSelectorBottomRightViewport.camera = sceneRoot.cameras[(count + 2) % 4];
            }
        }

        FrameGraph {
            id : frameGraph

            Viewport {
                id : mainViewport
                rect: Qt.rect(0, 0, 1, 1)

                ClearBuffer {
                    buffers : ClearBuffer.ColorDepthBuffer
                }

                Viewport {
                    id : topLeftViewport
                    rect : Qt.rect(0, 0, 0.5, 0.5)
                    CameraSelector {id : cameraSelectorTopLeftViewport; camera : sceneRoot.cameras[0]}
                }

                Viewport {
                    id : topRightViewport
                    rect : Qt.rect(0.5, 0, 0.5, 0.5)
                    CameraSelector {id : cameraSelectorTopRightViewport; camera : sceneRoot.cameras[1]}
                }

                Viewport {
                    id : bottomLeftViewport
                    rect : Qt.rect(0, 0.5, 0.5, 0.5)
                    CameraSelector {id : cameraSelectorBottomLeftViewport; camera : sceneRoot.cameras[2]}
                }

                Viewport {
                    id : bottomRightViewport
                    rect : Qt.rect(0.5, 0.5, 0.5, 0.5)
                    CameraSelector {id : cameraSelectorBottomRightViewport; camera : sceneRoot.cameras[3]}
                }

            } // mainViewport
        } // frameGraph

        Entity {
            id : cameraViewport1
            property Transform transform : Transform {
                LookAt {
                    position: Qt.vector3d( 0.0, 0.0, -20.0 )
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
                    viewCenter: Qt.vector3d( -3.0, 0.0, 10.0 )
                }
            }
            components : [cameraLens, transform]
        }

        Entity {
            id : cameraViewport3
            property Transform transform : Transform {
                LookAt {
                    position: Qt.vector3d( 0.0, 30.0, 30.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( -5.0, -20.0, -10.0 )
                }
            }
            components : [cameraLens, transform]
        }

        Entity {
            components : [
                Transform {
                    Rotate {
                        angle : -sceneRoot.rotationAngle
                        axis : Qt.vector3d(0, 0, 1)
                    }
                },
                SceneLoader {
                    source: "qrc:/assets/test_scene.dae"
                }]
        }

        Entity {
            id : cameraViewport4
            property Transform transform : Transform {
                LookAt {
                    position: Qt.vector3d( 0.0, 15.0, 20.0 )
                    upVector: Qt.vector3d( 0.0, 0.0, 1.0 )
                    viewCenter: Qt.vector3d( 0.0, -15.0, -20.0 )
                }
            }
            components : [cameraLens, transform]
        }


    } // sceneRoot

} // rootNode
