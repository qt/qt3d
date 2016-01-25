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

// For Qt.vector3d() and friends. For some reason this is provided by
// QQuickValueTypeProvider in QtQuick rather than the default value
// type provider in QtQml. So we will need to replicate this in Qt3D
// for the types that we wish to support. Otherwise we'll have to import
// QtQuick 2.1 all over the place.
import QtQuick 2.1

Effect {
    // TODO add another effect that uses texture for diffuse
    //property Texture2D diffuseTexture
    property vector3d ambient: Qt.vector3d( 0.1, 0.1, 0.1 )
    property vector3d diffuse: Qt.vector3d( 0.7, 0.7, 0.7 )
    property vector3d specular: Qt.vector3d( 0.2, 0.2, 0.2 )
    property real shininess: 10.0

    techniques: [
        Technique {
            criteria : [
                TechniqueCriterion { criterionType : TechniqueCriterion.RenderingStyle; criterionValue : "forward"}
            ]

            renderPasses: [
                RenderPass {
                    name: "zFill"
                    shaderProgram: zFillProgram
                    //renderStates: [ DepthState { depthTest: true; } ]
                },

                RenderPass {
                    name: "lighting"
                    shaderProgram: adsProgram
                    //renderStates: [ DepthState { depthWrites: true; depthTest: true; } ]
                }
            ]
        }
    ]

    ShaderProgram {
        id: zFillProgram
        objectName: "zFillProgram"
        vertexSourceFile: ":/shaders/zfill.vert"
        fragmentSourceFile: ":/shaders/zfill.frag"
    }

    ShaderProgram {
        id: adsProgram
        objectName: "adsProgram"
        vertexSourceFile: ":/shaders/phong.vert"
        fragmentSourceFile: ":/shaders/phong.frag"

//        // How to handle uniforms? Especially arrays and structs
//        uniforms: [
//            Uniform { name: "light[0].position:";  value: Qt.vector3d( 0.0, 0.0, 0.0 ) },
//            Uniform { name: "light[0].intensity:"; value: Qt.vector3d( 1.0, 1.0, 1.0 ) }
//        ]
    }
}
