// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

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

            TechniqueFilter {
                matchAll: [ FilterKey { name: "name"; value: "Desktop" } ]

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
                                        width: mainview.width
                                        height: mainview.height
                                        format: Texture.D24
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
                            clearColor: Qt.rgba(0., 0., 0., 1.)

                            CameraSelector {
                                id: lightCameraSelector
                            }
                        }
                    }
                }
            }

            RenderPassFilter {
                matchAny: [ FilterKey { name: "pass"; value: "forward" } ]

                ClearBuffers {
                    buffers: ClearBuffers.ColorDepthBuffer

                    CameraSelector {
                        id: viewCameraSelector
                    }
                }
            }
        }
    }
}
