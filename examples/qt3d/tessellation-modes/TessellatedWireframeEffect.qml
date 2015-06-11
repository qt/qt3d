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
import QtQuick 2.1

Effect {
    id: root

    // It's expected that users of this will specify the tessellation control and evaluation
    // shaders and if necessary suitable replacements for the other stages
    property alias vertexShaderCode: program.vertexShaderCode
    property alias tessellationControlShaderCode: program.tessellationControlShaderCode
    property alias tessellationEvaluationShaderCode: program.tessellationEvaluationShaderCode
    property alias geometryShaderCode: program.geometryShaderCode
    property alias fragmentShaderCode: program.fragmentShaderCode
    property alias shaderProgram: program

    parameters: [
        Parameter { name: "ambient";   value: Qt.vector3d( 0.1, 0.1, 0.1 ) },
        Parameter { name: "diffuse";   value: Qt.vector3d( 0.7, 0.7, 0.7 ) },
        Parameter { name: "specular";  value: Qt.vector3d( 0.95, 0.95, 0.95 ) },
        Parameter { name: "shininess"; value: 150.0 }
    ]

    techniques: [
        Technique {
            openGLFilter {
                api: OpenGLFilter.Desktop
                profile: OpenGLFilter.Core
                majorVersion: 4
                minorVersion: 0
            }

            annotations: [ Annotation { name: "renderingStyle"; value: "forward" } ]

            parameters: [
                Parameter { name: "light.position"; value: Qt.vector4d(0.0, 0.0, 0.0, 1.0) },
                Parameter { name: "light.intensity"; value: Qt.vector3d(1.0, 1.0, 1.0) },
                Parameter { name: "line.width"; value: 0.8 },
                Parameter { name: "line.color"; value: Qt.vector4d(1.0, 1.0, 1.0, 1.0) }
            ]

            renderPasses: [
                RenderPass {

                    bindings: [
                        ParameterMapping { parameterName: "ambient";  shaderVariableName: "ka"; bindingType: ParameterMapping.Uniform },
                        ParameterMapping { parameterName: "diffuse";  shaderVariableName: "kd"; bindingType: ParameterMapping.Uniform },
                        ParameterMapping { parameterName: "specular"; shaderVariableName: "ks"; bindingType: ParameterMapping.Uniform },

                        // TODO: Support uniform arrays. We want to be able to do (or something similar) and map vectors or lists of values
                        // through to the uniforms
                        ParameterMapping { parameterName: "innerTessLevel"; shaderVariableName: "inner[0]"; bindingType: ParameterMapping.Uniform },
                        ParameterMapping { parameterName: "outerTessLevel"; shaderVariableName: "outer[0]"; bindingType: ParameterMapping.Uniform }
                    ]

                    shaderProgram: ShaderProgram {
                        id: program
                        vertexShaderCode:   loadSource("qrc:/shaders/passthru.vert")
                        geometryShaderCode: loadSource("qrc:/shaders/robustwireframe.geom")
                        fragmentShaderCode: loadSource("qrc:/shaders/phongwireframe.frag")
                    }
                }
            ]
        }
    ]
}
