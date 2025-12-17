// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core
import Qt3D.Render
import Qt3D.Input
import Qt3D.Extras
import QtQuick

Entity {
    id: sceneRoot

    RenderSettings {
        id : external_forward_renderer
        activeFrameGraph: RenderSurfaceSelector {
            CameraSelector {
                camera: camera

                Viewport {
                    normalizedRect: Qt.rect(0., 0., 1., 1)

                    ClearBuffers {
                        clearColor: "black"
                        buffers: ClearBuffers.ColorDepthBuffer

                        LayerFilter {
                            layers: [ mainLayer ]

                            // DebugOverlay { }
                        }
                    }
                }
            }

            CameraSelector {
                camera: Camera {
                    // projectionType: CameraLens.OrthographicProjection
                    // left: -2.0
                    // right: 2.0
                    // top: 2.0
                    // bottom: -2.0
                    projectionType: CameraLens.PerspectiveProjection
                    fieldOfView: 45
                    aspectRatio: 16/9
                    nearPlane : 0.1
                    farPlane : 1000.0
                    position: Qt.vector3d( 0.0, 2.0, 5.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
                }

                Viewport {
                    normalizedRect: Qt.rect(0.7, 0.7, .3, .3)

                    LayerFilter {
                        layers: [ boxLayer ]

                        // NoPicking { enabled: true }
                    }
                }
            }
        }

        pickingSettings.pickMethod: PickingSettings.TrianglePicking
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 2.0, 20.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    components: [ external_forward_renderer, inputSettings, mouseHandler, kbHandler ]

    MouseHandler {
        id: mouseHandler
        sourceDevice:  MouseDevice {}
        onReleased: (mouse) => { console.log("trigger screenRayCasters"); screenRayCaster1.trigger(Qt.point(mouse.x, mouse.y)); screenRayCaster2.trigger(Qt.point(mouse.x, mouse.y)) }
    }

    KeyboardDevice { id: kbDevice }
    KeyboardHandler {
        id: kbHandler
        focus: true
        sourceDevice: kbDevice
        onPressed: (event) => {
                        if (event.text.toLowerCase() == "a") { raycaster.origin.x -= .1; raycaster.trigger() }
                        if (event.text.toLowerCase() == "f") { raycaster.origin.x += .1; raycaster.trigger() }
                        if (event.text.toLowerCase() == "s") { raycaster.origin.y += .1; raycaster.trigger() }
                        if (event.text.toLowerCase() == "d") { raycaster.origin.y -= .1; raycaster.trigger() }
                    }
        onSpacePressed: {
            var syncHits = raycaster.pick(raycaster.origin, raycaster.direction, raycaster.length)
            printHits("Synchronous hits", syncHits)
        }
    }

    function printHits(desc, hits) {
        console.log(desc, ":" + hits.length)
        for (var i=0; i<hits.length; i++) {
            console.log("  " + hits[i].entity.objectName, hits[i].distance,
                        hits[i].worldIntersection.x, hits[i].worldIntersection.y, hits[i].worldIntersection.z)
        }
    }

    Entity { id: mainScene
        objectName: "mainScene"
        components: [ headLight, raycaster, screenRayCaster1, mainLayer ]

        Layer { id: mainLayer; recursive: true }

        OrbitCameraController { camera: camera }
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { id: inputSettings }

        DirectionalLight {
            id: headLight
            worldDirection: camera.viewVector
        }

        RayCaster {
            id: raycaster
            origin: Qt.vector3d(0, 0, 4)
            direction: Qt.vector3d(0., 0., -1.)
            length: 5
            layers: [ mainLayer ]

            onHitsChanged: (hits) => { printHits("Model hits", hits) }
        }

        ScreenRayCaster {
            id: screenRayCaster1
            position: Qt.point(200, 200)
            layers: [ mainLayer ]

            onHitsChanged: (hits) => { printHits("Screen hits 1", hits) }
        }

        CylinderEntity {
            objectName: "CRed"
            id: leftEntity
            translation: Qt.vector3d(-5, 0, 0)
            color: "red"
        }

        CylinderEntity {
            objectName: "CGreen"
            id: mainEntity
            translation: Qt.vector3d(0, 0, 0)
            color: "green"
        }

        CylinderEntity {
            objectName: "CBlue"
            id: rightEntity
            translation: Qt.vector3d(5, 0, 0)
            color: "blue"
        }

        LineEntity {
            objectName: "Line"
            id: raydisplay
            origin: raycaster.origin
            direction: raycaster.direction
            length: raycaster.length
        }
    }

    Entity { id: boxScene
        objectName: "CBox"
        components: [ boxLayer, boxMesh, boxMaterial, boxLight, screenRayCaster2 ]

        Layer { id: boxLayer; recursive: true }

        CuboidMesh { id: boxMesh
            objectName: "CBoxMesh"
        }

        PhongMaterial {
            id: boxMaterial
            diffuse: "white"
        }

        DirectionalLight {
            id: boxLight
            worldDirection: Qt.vector3d(0, -.5, -1)
        }

        ScreenRayCaster {
            id: screenRayCaster2
            position: Qt.point(200, 200)
            layers: [ boxLayer ]

            onHitsChanged: (hits) => { printHits("Screen hits 2", hits) }
        }
    }
}
