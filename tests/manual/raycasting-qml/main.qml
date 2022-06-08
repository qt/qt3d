// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.11
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.0

Entity {
    id: sceneRoot

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

    OrbitCameraController { camera: camera }

    RenderSettings {
        id : external_forward_renderer
        activeFrameGraph : ForwardRenderer {
            camera: camera
            clearColor: "black"
        }
        pickingSettings.pickMethod: PickingSettings.TrianglePicking
    }

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

        onHitsChanged: printHits("Model hits", hits)
    }

    ScreenRayCaster {
        id: screenRayCaster
        position: Qt.point(200, 200)

        onHitsChanged: printHits("Screen hits", hits)
    }

    MouseHandler {
        id: mouseHandler
        sourceDevice:  MouseDevice {}
        onReleased: { screenRayCaster.trigger(Qt.point(mouse.x, mouse.y)) }
    }

    KeyboardDevice { id: kbDevice }
    KeyboardHandler {
        id: kbHandler
        focus: true
        sourceDevice: kbDevice
        onPressed: {
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

    components: [ external_forward_renderer, inputSettings, headLight, raycaster, screenRayCaster, mouseHandler, kbHandler ]

    function printHits(desc, hits) {
        console.log(desc, hits.length)
        for (var i=0; i<hits.length; i++) {
            console.log("  " + hits[i].entity.objectName, hits[i].distance,
                        hits[i].worldIntersection.x, hits[i].worldIntersection.y, hits[i].worldIntersection.z)
        }
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
        id: raydisplay
        origin: raycaster.origin
        direction: raycaster.direction
        length: raycaster.length
    }
}
