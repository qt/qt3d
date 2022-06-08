// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.10
import QtQuick 2.5

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 20.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    OrbitCameraController { camera: camera }

    RenderSettings {
        id : external_forward_renderer
        activeFrameGraph : ForwardRenderer {
            camera: camera
            clearColor: "lightgrey"
        }
    }

    // Event Source will be set by the Qt3DQuickWindow
    InputSettings { id: inputSettings }

    components: [external_forward_renderer, inputSettings]

    PlaneMesh {
        id: mesh
    }

    SpriteGrid {
        id: spriteGrid
        rows: 2
        columns: 6
        texture: textureLoader
    }

    SpriteSheet {
        id:spriteSheet
        texture: textureLoader

        SpriteItem { x:    0; y:   0; width: 250; height: 172 }
        SpriteItem { x:  276; y:   0; width: 250; height: 172 }
        SpriteItem { x:  550; y:   0; width: 250; height: 172 }
        SpriteItem { x:  826; y:   0; width: 250; height: 172 }
        SpriteItem { x: 1100; y:   0; width: 250; height: 172 }
        SpriteItem { x: 1376; y:   0; width: 250; height: 172 }
        SpriteItem { x:    0; y: 198; width: 250; height: 172 }
        SpriteItem { x:  276; y: 198; width: 250; height: 172 }
        SpriteItem { x:  550; y: 198; width: 250; height: 172 }
        SpriteItem { x:  826; y: 198; width: 250; height: 172 }
        SpriteItem { x: 1100; y: 198; width: 250; height: 172 }
        SpriteItem { x: 1376; y: 198; width: 250; height: 172 }
    }

    Transform {
        id: transform1
        scale: 8
        rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
        translation: Qt.vector3d(-6, 0, 0)
    }

    TextureMaterial {
        id: material1
        texture: TextureLoader {
            id: textureLoader
            source: "spritegrid.png"
            mirrored: false
        }
        textureTransform: spriteGrid.textureTransform
    }

    Entity {
        components: [ mesh, material1, transform1 ]
    }


    Transform {
        id: transform2
        scale: 8
        rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
        translation: Qt.vector3d(6, 0, 0)
    }

    TextureMaterial {
        id: material2
        texture: material1.texture
        textureTransform: spriteSheet.textureTransform
    }

    Entity {
        components: [ mesh, material2, transform2 ]
    }


    Transform {
        id: transform3
        scale3D: Qt.vector3d(12, 4, 4)
        rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 90)
        translation: Qt.vector3d(0, -6, 0)
    }

    TextureMaterial {
        id: material3
        texture: material1.texture
    }

    Entity {
        components: [ mesh, material3, transform3 ]
    }


    Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            spriteGrid.currentIndex = (spriteGrid.currentIndex + 1) % (spriteGrid.rows * spriteGrid.columns)
            spriteSheet.currentIndex = (spriteSheet.currentIndex + 1) % (spriteSheet.sprites.length)
        }
    }
}
