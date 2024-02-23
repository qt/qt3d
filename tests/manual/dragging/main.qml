// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

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

    RenderSettings {
        id : external_forward_renderer
        activeFrameGraph : ForwardRenderer {
            camera: camera
            clearColor: "white"
        }
    }

    // Event Source will be set by the Qt3DQuickWindow
    InputSettings { id: inputSettings }

    components: [external_forward_renderer, inputSettings]

    Entity {
        components: [
            DirectionalLight {
                worldDirection: Qt.vector3d(0., 0., -1.0).normalized();
                color: "white"
                intensity: 1.
            }
        ]
    }


    Entity {
        id: mainEntity
        objectName: "mainEntity"

        ConeMesh {
            id: coneMesh
            length: 4
        }

        PhongMaterial {
            id: coneMaterial
            diffuse: "yellow"
        }

        Transform {
            id: coneTransform
            scale: 1.5
            translation: Qt.vector3d(0, 2, 0)
        }

        components: [ coneMesh, coneMaterial, coneTransform ]
    }



    Entity {
        id: draggerEntity
        objectName: "draggerEntity"
        property vector3d position: Qt.vector3d(0, 0, 0)

        CylinderMesh {
            id: mesh
            radius: .5
            length: 3
            rings: 2
            slices: 20
        }

        Transform {
            id: transform
            rotationZ: 90
            translation: draggerEntity.position.plus(Qt.vector3d(0, -5, 0))
        }

        PhongMaterial {
            id: material
            diffuse: picker.pressed ? "lightgreen" : "green"
        }

        ObjectPicker {
            id: picker
            property double lastPos: 0
            dragEnabled: true
            onPressed: {
                lastPos = pick.worldIntersection.x
            }
            onMoved: {
                var x = draggerEntity.position.x + (pick.worldIntersection.x - lastPos)
                if (x > 5) x = 5
                if (x < -5) x = -5
                draggerEntity.position.x = x
                lastPos = pick.worldIntersection.x
                coneMesh.bottomRadius = 1. + (x / 7.)
            }
        }

        components: [ mesh, material, transform, picker ]
    }


    Entity {
        id: obstructionEntity

        components: [
            CuboidMesh { },
            PhongMaterial { diffuse: "blue" },
            Transform {
                scale: 2
                translation: Qt.vector3d(4, -5, 0)
            },
            ObjectPicker { }
        ]
    }
}
