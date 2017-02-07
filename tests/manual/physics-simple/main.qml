/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Physics 2.3
import Qt3D.Extras 2.0
import QtQuick 2.0

DefaultSceneEntity {
    id: scene

    Entity {
        id: cube

        components: [
            Transform {
                property real height: 10
                id: cubeTransform
                translation: Qt.vector3d(0, height, 0)
            },
            CuboidMesh {
            },
            PhongMaterial {
                id: cubeMaterial
                ambient: Qt.rgba(0.02, 0.02, 0.02, 1.0)
                diffuse: "blue"
                shininess: 50
            },
            PhysicsMaterial {
                mass: 20
            },
            Collider {
                shape: CuboidCollisionShape {

                }
            },

            ObjectPicker {
                onClicked: anim.running = true

                NumberAnimation {
                    id: anim
                    target: cubeTransform
                    property: "height"
                    duration: 1000
                    easing.type: Easing.InOutQuad
                    from: 10
                    to: 0
                }
            }

        ]
    }

    Entity {
        id: ground

        components: [
            CuboidMesh {
                xExtent: 100
                zExtent: 100
            },
            PhongMaterial {
                id: groundMaterial
                ambient: Qt.rgba(0.3, 0.9, 0.1, 1.0)
                diffuse: "darkgreen"
                shininess: 30
            },
            PhysicsMaterial {
                mass: 0
            },
            Collider {
                shape: CuboidCollisionShape {

                }
            }

        ]
    }

    camera: Camera {
        position: Qt.vector3d(10, 10, 25)
        viewCenter: Qt.vector3d(0, 5, 0)
    }

    OrbitCameraController {
        camera: scene.camera
        linearSpeed: 8
        lookSpeed: 180
    }
}
