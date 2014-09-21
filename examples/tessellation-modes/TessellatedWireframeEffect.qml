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

// For Qt.vector3d() and friends. For some reason this is provided by
// QQuickValueTypeProvider in QtQuick rather than the default value
// type provider in QtQml. So we will need to replicate this in Qt3D
// for the types that we wish to support. Otherwise we'll have to import
// QtQuick 2.1 all over the place.
import QtQuick 2.1

Effect {
    id: root

    // It's expected that users of this will specify the tessellation control and evaluation
    // shaders and if necessary suitable replacements for the other stages
    property alias vertexShaderSourceFile: program.vertexShaderSourceFile
    property alias tessellationControlShaderSourceFile: program.tessellationControlShaderSourceFile
    property alias tessellationEvaluationShaderSourceFile: program.tessellationEvaluationShaderSourceFile
    property alias geometryShaderSourceFile: program.geometryShaderSourceFile
    property alias fragmentShaderSourceFile: program.fragmentShaderSourceFile

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

            criteria: [ Criterion { name: "renderingStyle"; value: "forward" } ]

            parameters: [
                Parameter { name: "light.position"; value: Qt.vector4d(0.0, 0.0, 0.0, 1.0) },
                Parameter { name: "light.intensity"; value: Qt.vector3d(1.0, 1.0, 1.0) },
                Parameter { name: "line.width"; value: 0.8 },
                Parameter { name: "line.color"; value: Qt.vector4d(1.0, 1.0, 1.0, 1.0) }
            ]

            renderPasses: [
                RenderPass {
                    name: "lighting"

                    bindings: [
                        ParameterMapper { parameterName: "ambient";  shaderVariableName: "ka"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "diffuse";  shaderVariableName: "kd"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "specular"; shaderVariableName: "ks"; bindingType: ParameterMapper.Uniform },

                        // TODO: Support uniform arrays. We want to be able to do (or something similar) and map vectors or lists of values
                        // through to the uniforms
                        //ParameterMapper { parameterName: "innerTessLevel"; shaderVariableName: "inner"; bindingType: ParameterMapper.Uniform }
                        //ParameterMapper { parameterName: "outerTessLevel"; shaderVariableName: "outer"; bindingType: ParameterMapper.Uniform }
                        ParameterMapper { parameterName: "innerTessLevel0"; shaderVariableName: "inner[0]"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "innerTessLevel1"; shaderVariableName: "inner[1]"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "outerTessLevel0"; shaderVariableName: "outer[0]"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "outerTessLevel1"; shaderVariableName: "outer[1]"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "outerTessLevel2"; shaderVariableName: "outer[2]"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "outerTessLevel3"; shaderVariableName: "outer[3]"; bindingType: ParameterMapper.Uniform }
                    ]

                    shaderProgram: ShaderProgram {
                        id: program
                        vertexShaderSourceFile:   ":/shaders/passthru.vert"
                        geometryShaderSourceFile: ":/shaders/robustwireframe.geom"
                        fragmentShaderSourceFile: ":/shaders/phongwireframe.frag"
                    }
                }
            ]
        }
    ]
}
