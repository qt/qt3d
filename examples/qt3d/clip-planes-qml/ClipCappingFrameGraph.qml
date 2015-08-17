/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0
import Qt3D.Renderer 2.0

Viewport {
    property alias camera: cameraSelector.camera

    CameraSelector {
        id: cameraSelector

        StateSet {
            // Enable 3 clipping planes
            renderStates: [
                ClipPlane { plane: 0 },
                ClipPlane { plane: 1 },
                ClipPlane { plane: 2 },
                DepthTest { func: DepthTest.LessOrEqual }
            ]

            // Branch 1
            LayerFilter {
                // Render entities with their regular material
                // Fills depth buffer for entities that are clipped
                layers: ["content", "visualization"]
                ClearBuffer {
                    buffers: ClearBuffer.ColorDepthBuffer
                    RenderPassFilter {
                        includes: Annotation { name: "pass"; value: "material" }
                    }
                }
            }

            // Branch 2
            ClearBuffer {
                // Enable and fill Stencil to later generate caps
                buffers: ClearBuffer.StencilBuffer
                StateSet {
                    // Disable depth culling
                    // Incr for back faces
                    // Decr for front faces
                    // No need to output color values
                    renderStates: [
                        StencilTest {
                            front {
                                func: StencilTestSeparate.Always
                                ref: 0; mask: 0
                            }
                            back {
                                func: StencilTestSeparate.Always
                                ref: 0; mask: 0
                            }
                        },
                        StencilOp {
                            front.stencilDepthPass: StencilOpSeparate.Decr
                            back.stencilDepthPass: StencilOpSeparate.Incr
                        },
                        ColorMask { red: false; green: false; blue: false; alpha: false }
                    ]

                    LayerFilter {
                        layers: "content"
                        RenderPassFilter {
                            includes: Annotation { name: "pass"; value: "stencilFill"; }
                        }
                    }
                }
            }
        }

        // Branch 3
        StateSet {
            // Draw caps using stencil buffer
            LayerFilter {
                layers: "caps"
                RenderPassFilter {
                    includes: Annotation { name: "pass"; value: "capping"; }
                }
            }

            // Draw back faces - front faces -> caps
            renderStates: [
                StencilTest {
                    front {
                        func: StencilTestSeparate.NotEqual
                        ref: 0; mask: ~0
                    }
                    back {
                        func: StencilTestSeparate.NotEqual
                        ref: 0; mask: ~0
                    }
                }
            ]
        }
    }
}

