// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0 as Quick
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Examples 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity
{
    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController {
        camera: camera
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
                onStatusChanged: (status) => {
                    console.log("SceneLoader status: " + status);
                    if (status == SceneLoader.Ready) {
                        console.log("Scene is ready");

                        // Now find the torus and animate one of the material effect's parameters.
                        var e = sceneHelper.findEntity(sceneLoader, "Torus");
                        console.log("Found entity: " + e + " its components are:");
                        for (var i = 0; i < e.components.length; ++i) {
                            console.log("  " + e.components[i]);
                            if (e.components[i].effect !== undefined) {
                                var p = e.components[i].effect.parameters;
                                if (p !== undefined) {
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
                source: "qrc:/assets/obj/material-sphere.obj"
            }
        ]
    }
}
