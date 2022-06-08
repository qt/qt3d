// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.2 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0

RenderSettings {
    id: root

    property alias viewCamera: viewCameraSelector.camera
    property alias lightCamera: lightCameraSelector.camera
    readonly property Texture2D shadowTexture: depthTexture

    activeFrameGraph: Viewport {
        normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

        RenderSurfaceSelector {
            RenderPassFilter {
                matchAny: [ FilterKey { name: "pass"; value: "shadowmap" } ]

                RenderTargetSelector {
                    target: RenderTarget {
                        attachments: [
                            RenderTargetOutput {
                                objectName: "depth"
                                attachmentPoint: RenderTargetOutput.Depth
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
                matchAny: [ FilterKey { name: "pass"; value: "forward" } ]

                ClearBuffers {
                    clearColor: Qt.rgba(0.0, 0.4, 0.7, 1.0)
                    buffers: ClearBuffers.ColorDepthBuffer

                    CameraSelector {
                        id: viewCameraSelector
                    }
                }
            }
        }
    }
}
