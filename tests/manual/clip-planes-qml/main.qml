// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.4 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 800/600
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController { camera: camera }

    components: [
        RenderSettings {
            ClipCappingFrameGraph {
                id: frameGraph
                camera: camera;
                clearColor: Qt.rgba(0.0, 0.5, 1, 1)
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    ClipMaterialEffect {
        id: clipMaterialEffect
        sectionsData: clippingPlanes.sectionData
    }

    ClippingPlanes {
        id: clippingPlanes
        visualizationLayer: frameGraph.visualizationLayer
        capsLayer: frameGraph.capsLayer
    }

    // Entity being clipped
    Entity {

        property Material material: Material {
            effect: clipMaterialEffect
            parameters: [
                Parameter { name: "ka"; value: "black" },
                Parameter { name: "kd"; value: "blue" },
                Parameter { name: "ks"; value: "white" }
            ]
        }

        property Mesh mesh: Mesh {
            source: "qrc:/assets/obj/trefoil.obj"
        }

        property Transform transform: Transform {
            translation: Qt.vector3d( 0.0, 0.0, 0.0 )
            scale: 3
        }

        components: [material, transform, mesh, frameGraph.contentLayer]
    }
}
