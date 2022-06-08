// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
            }
        }
    ]

    TemporaryCamera {
        id: camera
        pos: Qt.vector3d( 0.0, 0.0, 120.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
        up: Qt.vector3d( 0.0, 1.0, 0.0 )
    }

    RenderableEntity {
        id: rootEntity

        meshSource: "assets/obj/toyplane.obj"
        material: PhongMaterial {
            diffuse: "red"
        }

        RenderableEntity {
            id: trefoil

            translation: Qt.vector3d( 0.0, 20.0, 0.0 )
            scale: 4

            meshSource: "assets/obj/trefoil.obj"
            material: PhongMaterial {
                diffuse: "green"
            }

            RenderableEntity {
                id: ball

                translation: Qt.vector3d( 0.0, 5.0, 0.0 )
                scale: 0.2

                meshSource: "assets/obj/ball.obj"
                material: PhongMaterial {
                    diffuse: "purple"
                }
            }
        }
    }

    TrefoilKnot {
        translation: Qt.vector3d( 20, 0, 0 )
        scale: 6.0
        phi: 60.0
        material: PhongMaterial {
            diffuse: "grey"
        }
    }
}
