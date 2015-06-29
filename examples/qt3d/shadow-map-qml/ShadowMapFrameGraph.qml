/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
import QtQuick 2.2 as QQ2

FrameGraph {
    id: root

    property alias viewCamera: viewCameraSelector.camera
    property alias lightCamera: lightCameraSelector.camera
    readonly property Texture2D shadowTexture: depthTexture

    activeFrameGraph: Viewport {
        rect: Qt.rect(0.0, 0.0, 1.0, 1.0)
        clearColor: Qt.rgba(0.0, 0.4, 0.7, 1.0)

        RenderPassFilter {
            includes: [ Annotation { name: "pass"; value: "shadowmap" } ]

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

                ClearBuffer {
                    buffers: ClearBuffer.DepthBuffer

                    CameraSelector {
                        id: lightCameraSelector
                    }
                }
            }
        }

        RenderPassFilter {
            includes: [ Annotation { name: "pass"; value: "forward" } ]

            ClearBuffer {
                buffers: ClearBuffer.ColorDepthBuffer

                CameraSelector {
                    id: viewCameraSelector
                }
            }
        }
    }
}
