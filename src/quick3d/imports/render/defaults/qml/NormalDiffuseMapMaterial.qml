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

Material {
    id: root

    property color ambient:  Qt.rgba( 0.05, 0.05, 0.05, 1.0 )
    property alias diffuse: diffuseTextureImage.source
    property color specular: Qt.rgba( 0.95, 0.95, 0.95, 1.0 )
    property alias normal: normalTextureImage.source
    property real shininess: 150.0
    property real textureScale: 1.0

    parameters: [
        Parameter { name: "ka"; value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b) },
        Parameter {
            name: "diffuseTexture"
            value: Texture2D {
                id: diffuseTexture
                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.Repeat
                    y: WrapMode.Repeat
                }
                generateMipMaps: true
                maximumAnisotropy: 16.0
                TextureImage { id: diffuseTextureImage; }
            }
        },
        Parameter {
            name: "normalTexture"
            value: Texture2D {
                id: normalTexture
                minificationFilter: Texture.Linear
                magnificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.Repeat
                    y: WrapMode.Repeat
                }
                maximumAnisotropy: 16.0
                TextureImage { id: normalTextureImage; }
            }
        },
        Parameter { name: "ks"; value: Qt.vector3d(root.specular.r, root.specular.g, root.specular.b) },
        Parameter { name: "shininess"; value: root.shininess },
        Parameter { name: "texCoordScale"; value: textureScale }
    ]

    effect: Effect {

        parameters: [
            Parameter { name: "lightPosition";  value: Qt.vector4d( 0.0, 0.0, 0.0, 1.0 ) },
            Parameter { name: "lightIntensity"; value: Qt.vector3d( 1.0, 1.0, 1.0 ) }
        ]

        ShaderProgram {
            id: gl2Es2Shader
            vertexShaderCode:   loadSource("qrc:/shaders/es2/normaldiffusemap.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/es2/normaldiffusemap.frag")
        }

        ShaderProgram {
            id: gl3Shader
            vertexShaderCode:   loadSource("qrc:/shaders/gl3/normaldiffusemap.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/gl3/normaldiffusemap.frag")
        }

        techniques: [
            // OpenGL 3.1
            Technique {
                openGLFilter {
                    api: OpenGLFilter.Desktop
                    profile: OpenGLFilter.Core
                    majorVersion: 3
                    minorVersion: 1
                }
                renderPasses: RenderPass { shaderProgram: gl3Shader }
            },

            // OpenGL 2.1
            Technique {
                openGLFilter {
                    api: OpenGLFilter.Desktop
                    profile: OpenGLFilter.None
                    majorVersion: 2
                    minorVersion: 0
                }
                renderPasses: RenderPass { shaderProgram: gl2Es2Shader }
            },

            // OpenGL ES 2
            Technique {
                openGLFilter {
                    api: OpenGLFilter.ES
                    profile: OpenGLFilter.None
                    majorVersion: 2
                    minorVersion: 0
                }
                renderPasses: RenderPass { shaderProgram: gl2Es2Shader }
            }
        ]
    }
}

