// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Viewport {
    id: root
    normalizedRect : Qt.rect(0.0, 0.0, 1.0, 1.0)

    property GBuffer gBuffer
    property alias camera : sceneCameraSelector.camera
    property alias sceneLayer: sceneLayerFilter.layers
    property alias screenQuadLayer: screenQuadLayerFilter.layers
    property alias debugLayer: debugLayerFilter.layers

    readonly property real windowWidth: surfaceSelector.surface !== null ? surfaceSelector.surface.width: 0
    readonly property real windowHeight: surfaceSelector.surface !== null ? surfaceSelector.surface.height: 0

    RenderSurfaceSelector {
        id: surfaceSelector

        CameraSelector {
            id : sceneCameraSelector

            // Fill G-Buffer
            LayerFilter {
                id: sceneLayerFilter
                RenderTargetSelector {
                    id : gBufferTargetSelector
                    target: gBuffer

                    ClearBuffers {
                        buffers: ClearBuffers.ColorDepthBuffer

                        RenderPassFilter {
                            id : geometryPass
                            matchAny : FilterKey { name : "pass"; value : "geometry" }
                        }
                    }
                }
            }

            TechniqueFilter {
                parameters: [
                    Parameter { name: "color"; value : gBuffer.color },
                    Parameter { name: "position"; value : gBuffer.position },
                    Parameter { name: "normal"; value : gBuffer.normal },
                    Parameter { name: "depth"; value : gBuffer.depth },
                    Parameter { name: "winSize"; value : Qt.size(1024, 1024) }
                ]

                RenderStateSet {
                    // Render FullScreen Quad
                    renderStates: [
                        BlendEquation { blendFunction: BlendEquation.Add },
                        BlendEquationArguments { sourceRgb: BlendEquationArguments.SourceAlpha; destinationRgb: BlendEquationArguments.DestinationColor }
                    ]
                    LayerFilter {
                        id: screenQuadLayerFilter
                        ClearBuffers {
                            buffers: ClearBuffers.ColorDepthBuffer
                            RenderPassFilter {
                                matchAny : FilterKey { name : "pass"; value : "final" }
                                parameters: Parameter { name: "winSize"; value : Qt.size(windowWidth, windowHeight) }

                            }
                        }
                    }
                    // RenderDebug layer
                    LayerFilter {
                        id: debugLayerFilter
                        Viewport {
                            normalizedRect : Qt.rect(0.5, 0.5, 0.5, 0.5)
                            RenderPassFilter {
                                matchAny : FilterKey { name : "pass"; value : "final" }
                                parameters: Parameter { name: "winSize"; value : Qt.size(windowWidth * 0.5, windowHeight * 0.5) }
                            }
                        }
                    }
                }
            }
        }
    }
}
