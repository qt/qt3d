/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0
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
    property alias viewportRect: viewport.rect
    property alias cameraViewportTopRight: cameraSelectorTopRight.camera
    property alias cameraViewportBottomLeft: cameraSelectorBottomLeft.camera
    property alias layerFilters : layerFilter.layers
    property alias clearColor : viewport.clearColor
    property alias colorAttachmentTexture : defaultColorAttachment.texture

    criteria : [
        Annotation { name : "RenderingStyle"; value : "forward";},
        Annotation { name : "Enabled"; value : true;}
    ]

    ClearBuffer {
        buffers : ClearBuffer.ColorDepthBuffer

        Viewport {
            id: viewport
            objectName : "viewport"
            rect: Qt.rect(0.0, 0.0, 1.0, 1.0) // From Top Left

            Viewport {
                id : top_right_viewport
                rect: Qt.rect(0.5, 0, 0.5, 0.5)
                objectName : "topRightViewport"
                CameraSelector {
                    id : cameraSelectorTopRight
                    objectName : "cameraSelector"
                    LayerFilter {
                        id : layerFilter
                        objectName : "layerFilter"
                        RenderPassFilter { includes : [Annotation {name : "Name"; value : "TextureLighting";}] }
                    }
                }
            }
        }
    }

    Viewport {
        id : bottom_left_viewport
        objectName : "bottomLeftViewport"
        rect: Qt.rect(0.0, 0.5, 0.5, 0.5)
        CameraSelector {
            id: cameraSelectorBottomLeft
            objectName : "cameraSelector"
            RenderTargetSelector {
                target : RenderTarget {
                    attachments : [
                        RenderAttachment {
                            id : defaultColorAttachment
                            type : RenderAttachment.ColorAttachment0
                            name : "diffuse"
                        }
                    ]
                }
                RenderPassFilter { includes : [Annotation {name : "Name"; value : "Texture";}] }
                RenderPassFilter { includes : [Annotation {name : "Name"; value : "Lighting";}] }
            }
            RenderPassFilter { includes : [Annotation {name : "Name"; value : "Final";}] }
        }
    }
}
