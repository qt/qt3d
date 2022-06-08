// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.2 as QQ2

FrameGraph {
    id: root

    property alias viewCamera: viewCameraSelector.camera
    property alias lightCamera: lightCameraSelector.camera
    readonly property Texture2D shadowTexture: depthTexture

    activeFrameGraph: Viewport {
        rect: Qt.rect(0.0, 0.0, 1.0, 1.0)
        clearColor: Qt.rgba(0.0, 0.0, 0.0, 1.0)

        RenderPassFilter {
            includes: [ FilterKey { name: "pass"; value: "shadowmap" } ]

            RenderTargetSelector {
                target: RenderTarget {
                    attachments: [
                        RenderAttachment {
                            name: "depth"
                            type: RenderAttachment.DepthAttachment
                            texture: Texture2D {
                                id: depthTexture
                                width: 1024
                                height: 1024
                                format: Texture.DepthFormat
                                generateMipMaps: false
                                magnificationFilter: Texture.Linear
                                minificationFilter: Texture.Linear
                                wrapMode {
                                    x: WrapMode.ClampToEdge
                                    y: WrapMode.ClampToEdge
                                }
                                comparisonFunction: Texture.CompareLessEqual
                                comparisonMode: Texture.CompareRefToTexture
                            }
                        }
                    ]
                }

                ClearBuffers {
                    buffers: ClearBuffers.DepthBuffer

                    CameraSelector {
                        id: lightCameraSelector
                    }
                }
            }
        }

        RenderPassFilter {
            includes: [ FilterKey { name: "pass"; value: "forward" } ]

            ClearBuffers {
                buffers : ClearBuffers.ColorDepthBuffer

                CameraSelector {
                    id: viewCameraSelector
                }
            }
        }
    }
}
