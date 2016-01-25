/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D.Core 2.0
import Qt3D.Render 2.0

TechniqueFilter {
    id: root

    objectName : "techniqueFilter"

    // Expose the viewport rect and camera. This allows users of this
    // forward rendering framegraph to decide which camera in the
    // scene renders to a given viewport region.
    //
    // Using this as a building block for a larger framegraph would
    // allow a scene to be rendered multiple times to different
    // viewports using different cameras for e.g.
    property alias mainCamera: mainCameraSelector.camera
    property alias detailCamera: detailCameraSelector.camera
    property alias layerFilters: layerFilter.layers
    property alias clearColor: viewport.clearColor

    requires : [
        Annotation { name: "RenderingStyle"; value: "forward";},
        Annotation { name: "Enabled"; value: true;}
    ]

    ClearBuffer {
        buffers: ClearBuffer.ColorDepthBuffer

        // Main Viewport
        Viewport {
            id: viewport
            rect: Qt.rect(0.0, 0, 1.0, 1.0)

            CameraSelector {
                id: mainCameraSelector

                LayerFilter {
                    id: layerFilter
                    RenderPassFilter { includes: [
                            Annotation {name : "Name"; value : "Texture"}
                        ] }
                }
            }
        }
    }

    Viewport {
        id: detailViewport
        rect: Qt.rect(0.75, 0.0, 0.25, 0.25)

        CameraSelector {
            id: detailCameraSelector
            RenderPassFilter {
                includes: [ Annotation { name: "Name"; value: "ColorMaterial"; } ]
            }
        }
    }
}
