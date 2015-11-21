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

import QtQuick 2.0 as Quick
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Examples 2.0

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
        id: animRotComp
        Quick.QtObject {
            id: rotationData
            property real userAngle: 0.0
            property vector3d axis: Qt.vector3d(0, 0, 1)
            Quick.NumberAnimation on userAngle {
                from: 0
                to: 360
                duration: 5000
                loops: Quick.Animation.Infinite
            }
        }
    }

    Entity {
        components: [
            Transform {
                matrix: {
                    var m = Qt.matrix4x4();
                    m.rotate(90, Qt.vector3d(0, 1, 0));
                    return m;
                }
            },
            SceneLoader {
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

                        // Add an animated rotation transform to make Suzanne orbit around.
                        e = sceneHelper.findEntity(sceneLoader, "Suzanne");
                        var t = sceneHelper.findComponent(e, "Qt3DCore::QTransform");
                        if (!t)
                            return;

                        // Add object with animation data as a child
                        var suzanneAnim = animRotComp.createObject(t);

                        // Query the existing transformation. We will use parts of this in
                        // the replacement animation
                        var origTranslation = t.translation;
                        var origRotation = t.rotation;
                        var origScale = t.scale;
                        console.log("origTranslation = " + origTranslation);
                        console.log("origRotation    = " + origRotation);
                        console.log("origScale       = " + origScale);

                        // Make a deep copy of the original translation. This will not be updated when the
                        // transformation's translation property gets updated due to the animation.
                        var constantTranslation = Qt.vector3d(origTranslation.x, origTranslation.y, origTranslation.z);

                        t.matrix = Qt.binding(function() {
                            var m = Qt.matrix4x4();
                            m.rotate(suzanneAnim.userAngle, suzanneAnim.axis);
                            m.translate(constantTranslation);
                            m.rotate(90, Qt.vector3d(1, 0, 0));
                            m.scale(origScale);
                            return m;
                        })
                    }
                }
            }
        ]
    }

    Entity {
        components: [
            Transform {
                scale: 0.2
                translation: Qt.vector3d(0.0, -15.0, 0.0)
            },
            SceneLoader {
                source: "qrc:/assets/chest/Chest.obj"
            }
        ]
    }
}
