// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import QtQuick 2.0 as QQ2
import QtQuick 2.12


Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.OrthographicProjection
        left: -2
        right: 2
        top: 2
        bottom:  -2
        nearPlane: 0.1
        farPlane: 1000.0
        position: Qt.vector3d(0.0, 0.0, 1.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    RenderTarget {
        id: renderTargetFBO
        attachments : [
            RenderTargetOutput {
                attachmentPoint : RenderTargetOutput.Color0
                texture : Texture2DMultisample {
                    id : colorAttachment
                    width : scene3d.width
                    height : scene3d.height
                    format : Texture.RGBA8_UNorm
                    generateMipMaps : false
                    samples: 1
                }
            },
            RenderTargetOutput {
                attachmentPoint : RenderTargetOutput.Depth
                texture : Texture2DMultisample {
                    width : scene3d.width
                    height : scene3d.height
                    format : Texture.D32F
                    generateMipMaps : false
                    samples: 1
                }
            }
        ]
    }

    components: [
        RenderSettings {
            activeFrameGraph: FrameGraph {
                id: framegraph
                camera: camera
                fbo: renderTargetFBO
            }
            renderPolicy: RenderSettings.Always
        }
    ]

    Entity {
        id: firstPassEntities

        Entity {
            components: [
                Transform {
                    translation: Qt.vector3d(0,0,-2)
                },
                Material1 {
                    color: "red"
                },
                SphereMesh { }
            ]
        }


        Entity {
            components: [
                Transform {
                    translation: Qt.vector3d(1,0,-1)
                },
                Material2 {
                    color: "green"
                },
                SphereMesh { }
            ]
        }
    }

    Entity {
        id: secondPassEntities
        components: [framegraph.layer]

        Entity {
            components: [
                Transform {
                    translation: Qt.vector3d(0,0,-4)
                    scale: 0.5
                },
                Material1 {
                    ColorAnimation on color {
                        from: "black"
                        to: "purple"
                        duration: 2000
                        loops: Animation.Infinite
                    }
                },
                SphereMesh { }
            ]
        }


        Entity {
            components: [
                Transform {
                    translation: Qt.vector3d(1,0,-3)
                    scale: 0.5
                },
                Material2 {
                    color: "orange"
                },
                SphereMesh { }
            ]
        }
    }

    PostProcess {
        colorTexture: colorAttachment
    }
}
