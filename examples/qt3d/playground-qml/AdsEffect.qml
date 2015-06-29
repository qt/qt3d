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
