// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.9
import Qt3D.Render 2.9

RenderSurfaceSelector {
    id: surfaceSelector

    readonly property Layer layer: Layer {
        recursive: true
    }

    property alias camera: cameraSelector.camera
    property alias clearColor: clearBuffers.clearColor
    property alias fbo : renderToFboSelector.target

    Viewport {
        normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

        // 1st: render scene to texture
        RenderTargetSelector {
            id : renderToFboSelector

            RenderStateSet {
                renderStates: [
                    MultiSampleAntiAliasing {},
                    DepthTest {
                        depthFunction: DepthTest.Less
                    },
                    CullFace{
                        mode: CullFace.Back
                    }
                ]

                ClearBuffers {
                    id: clearBuffers
                    buffers: ClearBuffers.ColorDepthBuffer
                    NoDraw {}
                }

                CameraSelector {
                    id: cameraSelector

                    LayerFilter {
                        layers: [layer]
                        filterMode: LayerFilter.DiscardAllMatchingLayers
                        TechniqueFilter {
                            RenderPassFilter {
                                matchAny: FilterKey { name: "pass"; value: 0 }
                            }
                            SortPolicy {
                                sortTypes: [SortPolicy.BackToFront]
                                RenderPassFilter {
                                    matchAny: FilterKey { name: "pass"; value: 1 }
                                }
                            }
                        }
                    }

                    RenderStateSet {
                        renderStates: [
                            DepthTest {
                                depthFunction: DepthTest.Always
                            }
                        ]
                        LayerFilter {
                            layers: [layer]
                            filterMode: LayerFilter.AcceptAnyMatchingLayers
                            TechniqueFilter {
                                RenderPassFilter {
                                    matchAny: FilterKey { name: "pass"; value: 0 }
                                }
                                SortPolicy {
                                    sortTypes: [SortPolicy.BackToFront]
                                    RenderPassFilter {
                                        matchAny: FilterKey { name: "pass"; value: 1 }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        RenderStateSet {
            renderStates: [
                DepthTest {depthFunction: DepthTest.Always},
                BlendEquation {},
                BlendEquationArguments {
                    sourceRgb: BlendEquationArguments.One
                    sourceAlpha: BlendEquationArguments.One
                    destinationRgb: BlendEquationArguments.Zero
                    destinationAlpha: BlendEquationArguments.Zero
                }
            ]

            RenderPassFilter {
                matchAny : FilterKey { name : "pass"; value : "final" }
            }
        }
    }
}
