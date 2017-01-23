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
import Qt3D.Animation 2.2
import Qt3D.Extras 2.0

DefaultSceneEntity {
    id: scene

    Entity {
        id: cube

        components: [
            CuboidMesh {
            },
            PhongMaterial {
                id: cubeMaterial
                ambient: Qt.rgba(0.02, 0.02, 0.02, 1.0)
                diffuse: "blue"
                shininess: 50
            },
            Transform {
                id: cubeTransform
            },
            ClipAnimator {
                running: true
                clip: AnimationClip {
                    source: "cubeanimation.json"
                }

                // By default introspect parent Entity and try
                // to map fcurve groups to properties of QTransform
                // mapping: AutomaticAnimationMapping {}

                // To do more, we can be explicit
                mapping: ChannelMapper {
                    mappings: [
                        ChannelMapping { channelName: "Location"; target: cubeTransform; property: "translation" },
                        ChannelMapping { channelName: "Rotation"; target: cubeTransform; property: "rotation" },
                        ChannelMapping { channelName: "Diffuse Color"; target: cubeMaterial; property: "diffuse" }
                    ]
                }
            }

        ]
    }

    camera: Camera {
        position: Qt.vector3d(10, 10, 10)
        viewCenter: Qt.vector3d(0, 0, 0)
    }

    OrbitCameraController {
        camera: scene.camera
        linearSpeed: 8
        lookSpeed: 180
    }
}
