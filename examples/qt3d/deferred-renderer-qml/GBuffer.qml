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

RenderTarget {

    property alias color : colorAttachment
    property alias position : positionAttachment
    property alias normal : normalAttachment
    property alias depth : depthAttachment

    attachments : [

        RenderAttachment {
            name : "color"
            type : RenderAttachment.ColorAttachment0
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
        RenderAttachment {
            name : "position"
            type : RenderAttachment.ColorAttachment1
            texture : Texture2D {
                id : positionAttachment
                width : 1024
                height : 1024
                // This texture format may not be supported by
                // some OpenGL vendors when used as a color attachment
                format : Texture.RGB32F
                generateMipMaps : false
                magnificationFilter : Texture.Linear
                minificationFilter : Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
            }
        },
        RenderAttachment {
            name : "normal"
            type : RenderAttachment.ColorAttachment2
            texture : Texture2D {
                id : normalAttachment
                width : 1024
                height : 1024
                // This texture format may not be supported by
                // some OpenGL vendors when used as a color attachment
                format : Texture.RGB16F
                generateMipMaps : false
                magnificationFilter : Texture.Linear
                minificationFilter : Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
            }
        },
        RenderAttachment {
            name : "depth"
            type : RenderAttachment.DepthAttachment
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
    ] // attachments
}
