// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

RenderTarget {

    property alias color : colorAttachment
    property alias position : positionAttachment
    property alias normal : normalAttachment
    property alias depth : depthAttachment

    attachments : [

        RenderTargetOutput {
            objectName : "color"
            attachmentPoint : RenderTargetOutput.Color0
            texture : Texture2D {
                id : colorAttachment
                width : 1024
                height : 1024
                format : Texture.RGBA32F
                generateMipMaps : false
                magnificationFilter : Texture.Linear
                minificationFilter : Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
            }
        },
        RenderTargetOutput {
            objectName : "position"
            attachmentPoint : RenderTargetOutput.Color1
            texture : Texture2D {
                id : positionAttachment
                width : 1024
                height : 1024
                // We use RGBA32F here instead of a more fitting format because
                // OpenGL vendors might not support other formats
                format : Texture.RGBA32F
                generateMipMaps : false
                magnificationFilter : Texture.Linear
                minificationFilter : Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
            }
        },
        RenderTargetOutput {
            objectName : "normal"
            attachmentPoint : RenderTargetOutput.Color2
            texture : Texture2D {
                id : normalAttachment
                width : 1024
                height : 1024
                // We use RGBA32F here instead of a more fitting format because
                // OpenGL vendors might not support other formats
                format : Texture.RGBA32F
                generateMipMaps : false
                magnificationFilter : Texture.Linear
                minificationFilter : Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
            }
        },
        RenderTargetOutput {
            objectName : "depth"
            attachmentPoint : RenderTargetOutput.Depth
            texture : Texture2D {
                id : depthAttachment
                width : 1024
                height : 1024
                format : Texture.D32F
                generateMipMaps : false
                magnificationFilter : Texture.Linear
                minificationFilter : Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
            }
        }
    ] // outputs
}
