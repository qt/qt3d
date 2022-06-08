// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Viewport {
    id: root
    property alias camera: cameraSelector.camera
    property alias window: surfaceSelector.surface
    property alias clearColor: clearBuffer.clearColor

    readonly property Layer contentLayer: Layer {}
    readonly property Layer visualizationLayer: Layer {}
    readonly property Layer capsLayer: Layer {}

    RenderSurfaceSelector {
        id: surfaceSelector

        CameraSelector {
            id: cameraSelector

            RenderStateSet {
                // Enable 3 clipping planes
                renderStates: [
                    ClipPlane { planeIndex: 0 },
                    ClipPlane { planeIndex: 1 },
                    ClipPlane { planeIndex: 2 },
                    DepthTest { depthFunction: DepthTest.LessOrEqual }
                ]

                // Branch 1
                LayerFilter {
                    // Render entities with their regular material
                    // Fills depth buffer for entities that are clipped
                    layers: [root.contentLayer, root.visualizationLayer]
                    ClearBuffers {
                        id: clearBuffer
                        buffers: ClearBuffers.ColorDepthBuffer
                        RenderPassFilter {
                            matchAny: FilterKey { name: "pass"; value: "material" }
                        }
                    }
                }

                // Branch 2
                ClearBuffers {
                    // Enable and fill Stencil to later generate caps
                    buffers: ClearBuffers.StencilBuffer
                    RenderStateSet {
                        // Disable depth culling
                        // Incr for back faces
                        // Decr for front faces
                        // No need to output color values
                        renderStates: [
                            StencilTest {
                                front {
                                    stencilFunction: StencilTestArguments.Always
                                    referenceValue: 0; comparisonMask: 0
                                }
                                back {
                                    stencilFunction: StencilTestArguments.Always
                                    referenceValue: 0; comparisonMask: 0
                                }
                            },
                            StencilOperation {
                                front.allTestsPassOperation: StencilOperationArguments.Decrement
                                back.allTestsPassOperation: StencilOperationArguments.Increment
                            },
                            ColorMask { redMasked: false; greenMasked: false; blueMasked: false; alphaMasked: false }
                        ]

                        LayerFilter {
                            layers: root.contentLayer
                            RenderPassFilter {
                                matchAny: FilterKey { name: "pass"; value: "stencilFill"; }
                            }
                        }
                    }
                }
            }

            // Branch 3
            RenderStateSet {
                // Draw caps using stencil buffer
                LayerFilter {
                    layers: root.capsLayer
                    RenderPassFilter {
                        matchAny: FilterKey { name: "pass"; value: "capping"; }
                    }
                }

                // Draw back faces - front faces -> caps
                renderStates: [
                    StencilTest {
                        front {
                            stencilFunction: StencilTestArguments.NotEqual
                            referenceValue: 0; comparisonMask: ~0
                        }
                        back {
                            stencilFunction: StencilTestArguments.NotEqual
                            referenceValue: 0; comparisonMask: ~0
                        }
                    }
                ]
            }
        }
    }
}

