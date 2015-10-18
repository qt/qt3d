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

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Examples 2.0
import QtQuick 2.0 as Quick

Entity
{
    components: FrameGraph {
        activeFrameGraph: ForwardRenderer {
            clearColor: Qt.rgba(0, 0.5, 1, 1)
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
        position: Qt.vector3d( 0.0, 0.0, -40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration  {
        controlledCamera: camera
    }

    // test_scene.dae contains three named nodes. Once the asynchronous loading of the
    // scene completes, we look up some of them.
    SceneHelper {
        id: sceneHelper
    }

    Quick.Component {
        id: animParamComp
        Quick.SequentialAnimation {
            id: seqAnim
            loops: Quick.Animation.Infinite
            property variant target: null
            Quick.ColorAnimation {
                target: seqAnim.target
                property: "value"
                from: Qt.rgba(0, 0, 0, 1)
                to: Qt.rgba(1, 1, 1, 1)
                duration: 2000
            }
            Quick.ColorAnimation {
                target: seqAnim.target
                property: "value"
                from: Qt.rgba(1, 1, 1, 1)
                to: Qt.rgba(0, 0, 0, 1)
                duration: 1000
            }
        }
    }

    Quick.Component {
        id: rot90Comp
        Rotate { axis: Qt.vector3d(1, 0, 0); angle: 90 }
    }

    Quick.Component {
        id: animRotComp
        Rotate {
            axis: Qt.vector3d(0, 0, 1)
            Quick.NumberAnimation on angle {
                from: 0
                to: 360
                duration: 5000
                loops: Quick.Animation.Infinite
            }
        }
    }

    Entity {
        components : [
            Transform {
                Rotate {angle : 90; axis : Qt.vector3d(0, 1, 0)}
            },
            SceneLoader
            {
                id: sceneLoader
                source: "qrc:/assets/test_scene.dae"
                onStatusChanged: {
                    console.log("SceneLoader status: " + status);
                    if (status == SceneLoader.Loaded) {
                        console.log("Scene is ready");

                        // Now find the torus and animate one of the material effect's parameters.
                        var e = sceneHelper.findEntity(sceneLoader, "Torus");
                        console.log("Found entity: " + e + " its components are:");
                        for (var i = 0; i < e.components.length; ++i) {
                            console.log("  " + e.components[i]);
                            if (e.components[i].effect !== undefined) {
                                var p = e.components[i].effect.parameters;
                                for (var j = 0; j < p.length; ++j) {
                                    if (p[j].name === "kd") {
                                        var anim = animParamComp.createObject(p[j]);
                                        anim.target = p[j];
                                        anim.running = true;
                                        break;
                                    }
                                }

                            }
                        }

                        // Add an animated rotation transform.
                        e = sceneHelper.findEntity(sceneLoader, "Suzanne");
                        for (var i = 0; i < e.components.length; ++i) {
                            if (e.components[i].transforms !== undefined) {
                                var t = e.components[i].transforms;
                                sceneHelper.addListEntry(t, rot90Comp.createObject());
                                sceneHelper.addListEntry(t, animRotComp.createObject());
                                break;
                            }
                        }
                    }
                }
            }]
    }

    Entity {
        components : [
            Transform {
                Scale { scale: 0.2 }
                Translate { dy: -15 }
            },
            SceneLoader
            {
                source: "qrc:/assets/chest/Chest.obj"
            }]
    }
}
