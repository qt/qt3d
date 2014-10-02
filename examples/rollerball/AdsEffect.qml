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

    // These parameters act as default values for the effect. They take
    // priority over any parameters specified in the RenderPasses below
    // (none provided in this example). In turn these parameters can be
    // overwritten by specifying them in a Material that references this
    // effect.
    // The priority order is:
    //
    // Material -> Effect -> Technique -> RenderPass -> GLSL default values
    parameters: [
        Parameter { name: "ambient";   value: Qt.vector3d( 0.1, 0.1, 0.1 ) },
        Parameter { name: "diffuse";   value: Qt.vector3d( 0.7, 0.7, 0.7 ) },
        Parameter { name: "specular";  value: Qt.vector3d( 0.95, 0.95, 0.95 ) },
        Parameter { name: "shininess"; value: root.shininess }
    ]

    techniques: [
        Technique {
            openGLFilter {
                api: OpenGLFilter.Desktop
                profile: OpenGLFilter.Core
                majorVersion: 3
                minorVersion: 1
            }

            criteria: [ Criterion { name : "renderingStyle"; value : "forward" } ]

            parameters: [
                Parameter { name: "lightPosition";  value: Qt.vector4d( 0.0, 0.0, 0.0, 1.0 ) },
                Parameter { name: "lightIntensity"; value: Qt.vector3d( 1.0, 1.0, 1.0 ) }
            ]

            renderPasses: [
                RenderPass {

                    // The bindings property allows us to map from names of parameters (uniforms or vertex attributes)
                    // within a shader to more friendly names in QML. By default the parameter names are exposed from
                    // the shader so we only need to add add mappings where the names differ. E.g. here we map from the
                    // ka uniform name in the shader to a property called ambient
                    bindings: [
                        // Uniforms (those provided by the user)
                        ParameterMapper { parameterName: "ambient";  shaderVariableName: "ka"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "diffuse";  shaderVariableName: "kd"; bindingType: ParameterMapper.Uniform },
                        ParameterMapper { parameterName: "specular"; shaderVariableName: "ks"; bindingType: ParameterMapper.Uniform }
                    ]

                    shaderProgram: ShaderProgram {
                        vertexShaderSourceFile:   ":/shaders/phong.vert"
                        fragmentShaderSourceFile: ":/shaders/phong.frag"
                    }
                }
            ]
        }
    ]
}
