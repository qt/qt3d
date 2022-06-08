// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.10
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
        position: Qt.vector3d( 0.0, 0.0, 3.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Camera {
        id: backgroundCamera
        projectionType: CameraLens.OrthographicProjection
        left: -0.25
        bottom: -0.25
        top: 0.25
        right: 0.25
        nearPlane: 0.1
        farPlane: 1.5
        position: Qt.vector3d( 0.0, 0.0, 1.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Layer {
        id: backgroundLayer
        recursive: true
    }

    Layer {
        id: sceneLayer
        recursive: true
    }

    Layer {
        id: secondBackgroundLayer
    }

    RenderSettings {
        id : external_forward_renderer
        activeFrameGraph :

            RenderSurfaceSelector {
            Viewport {
                normalizedRect: Qt.rect(0,0,1,1)

                ClearBuffers {
                    buffers: ClearBuffers.ColorDepthStencilBuffer
                    clearColor: "white"

                    LayerFilter {
                        layers: [
                            backgroundLayer,
                            secondBackgroundLayer
                        ]

                        CameraSelector {
                            camera: backgroundCamera
                        }
                    }
                }

                ClearBuffers {
                    buffers: ClearBuffers.DepthStencilBuffer

                    LayerFilter {
                        layers: [
                            backgroundLayer,
                        ]
                        filterMode: LayerFilter.DiscardAnyMatchingLayers

                        CameraSelector {
                            camera: camera
                        }
                    }
                }
            }
        }
    }

    OrbitCameraController {
        camera: camera
    }

    InputSettings {
        id: inputSettings
    }

    components: [external_forward_renderer, inputSettings]

    PlaneMesh {
        id: backgroundMesh
        width: 0.5
        height: 0.5
    }

    Transform {
        id: backgroundTransform
        rotation: fromAxisAndAngle(Qt.vector3d(1,0,0), 90)
    }

    Transform {
        id: smallBackgroundTransform
        translation: Qt.vector3d(0,0.5,0)
        scale: 0.5
    }

    PhongMaterial {
        id: backgroundMaterial
        ambient: "black"
    }

    PhongMaterial {
        id: smallBackgroundMaterial
        ambient: "red"
    }

    GeometryRenderer {
        id: mesh
        view: PlaneMesh {
            width: 1
            height: 1
        }
    }

    Transform {
        id: transform
        translation: Qt.vector3d(0,0,-1)
        rotation: fromAxisAndAngle(Qt.vector3d(1,0,0), 90)
    }

    PhongMaterial {
        id: material
        ambient: "blue"
    }

    Entity {
        id: backgroundFilterEntity
        components: [backgroundLayer]

        Entity {
            id: backgroundEntity
            objectName: "backgroundEntity"
            components: [backgroundMesh, backgroundMaterial, backgroundTransform]

            Entity {
                id: smallBackground
                objectName: "smallBackground"
                components: [backgroundMesh, smallBackgroundMaterial, smallBackgroundTransform]
            }
        }
    }

    Entity {
        components: [sceneLayer]

        Entity {
            id: mainEntity
            objectName: "mainEntity"
            components: [ mesh, material, transform ]
        }
    }
}
