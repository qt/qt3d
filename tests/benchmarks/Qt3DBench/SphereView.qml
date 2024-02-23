// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0 as QQ2

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 1280 / 768 //_window.width / _window.height
        nearPlane: 0.01
        farPlane: 1000.0
        position: Qt.vector3d(0.0, 0.0, -100.0)
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration {
        controlledCamera: camera
    }

    Light {
        id: light
    }

    components: [
        Qt3DBenchFrameGraph {
            id: frameGraph
            viewCamera: camera
            lightCamera: light.lightCamera
        }
    ]

    ShadowEffect {
        id: shadowEffect

        shadowTexture: frameGraph.shadowTexture
        light: light
    }

    NodeInstantiator {
        id: spheres
        property int count: 100
        property real spacing: 2
        property int cols: 10
        property int rows: 10
        property int levelCount: cols * rows
        property int levels: 1

        model: count
        delegate: SphereElement {
            id: sphereEntity

            xPos: spheres.spacing * (index % spheres.cols - 0.5 * (spheres.cols - 1))
            yPos: spheres.spacing * (Math.floor(index / spheres.levelCount) - 0.5 * spheres.levels)
            zPos: spheres.spacing * (Math.floor((index % spheres.levelCount) / spheres.cols) - 0.5 * spheres.rows)
            material: ShadowMaterial {
                effect: shadowEffect
                diffuseColor: Qt.rgba(0.9, 0.5, 0.3, 1.0)
                shininess: 75
            }
        }
    }

    // Ground plane
    // Just for showing that shadows are really working
    GroundPlane {
        material: ShadowMaterial {
            effect: shadowEffect
            diffuseColor: Qt.rgba(0.2, 0.5, 0.3, 1.0)
            specularColor: Qt.rgba(1.0, 0, 0, 1.0)
        }
    }
}
