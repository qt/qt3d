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

Material {
    id:root
    property color ambient:  Qt.rgba( 0.05, 0.05, 0.05, 1.0 )
    property color diffuse:  Qt.rgba( 0.7, 0.7, 0.7, 1.0 )
    property color specular: Qt.rgba( 0.95, 0.95, 0.95, 1.0 )
    property real shininess: 150.0

    parameters: [
        Parameter { name: "ambient";   value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b) },
        Parameter { name: "diffuse";   value: Qt.vector3d(root.diffuse.r, root.diffuse.g, root.diffuse.b) },
        Parameter { name: "specular";  value: Qt.vector3d(root.specular.r, root.specular.g, root.specular.b) },
        Parameter { name: "shininess"; value: root.shininess }
    ]

    ShaderProgram {
        id: gl3PhongShader
        vertexShaderCode: loadSource("qrc:/shaders/gl3/phong.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/gl3/phong.frag")
    }

    ShaderProgram {
        id: gl2es2PhongShader
        vertexShaderCode: loadSource("qrc:/shaders/es2/phong.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/es2/phong.frag")
    }

    ParameterMapping { id: ambientMapping; parameterName: "ambient";  shaderVariableName: "ka"; bindingType: ParameterMapping.Uniform }
    ParameterMapping { id: diffuseMapping; parameterName: "diffuse";  shaderVariableName: "kd"; bindingType: ParameterMapping.Uniform }
    ParameterMapping { id: specularMapping; parameterName: "specular"; shaderVariableName: "ks"; bindingType: ParameterMapping.Uniform }

    effect: Effect {
        techniques: [
            // GL 3 Technique
            Technique {
                openGLFilter {
                    api: OpenGLFilter.Desktop
                    profile: OpenGLFilter.Core
                    majorVersion: 3
                    minorVersion: 1
                }
                renderPasses: RenderPass {
                    bindings: [ambientMapping, diffuseMapping, specularMapping]
                    shaderProgram: gl3PhongShader
                }
            },

            // GL 2 Technique
            Technique {
                openGLFilter {
                    api: OpenGLFilter.Desktop
                    profile: OpenGLFilter.None
                    majorVersion: 2
                    minorVersion: 0
                }
                renderPasses: RenderPass {
                    bindings: [ambientMapping, diffuseMapping, specularMapping]
                    shaderProgram: gl2es2PhongShader
                }
            },

            // ES 2 Technique
            Technique {
                openGLFilter {
                    api: OpenGLFilter.ES
                    profile: OpenGLFilter.None
                    majorVersion: 2
                    minorVersion: 0
                }
                renderPasses: RenderPass {
                    bindings: [ambientMapping, diffuseMapping, specularMapping]
                    shaderProgram: gl2es2PhongShader
                }
            }
        ]
    }
}

