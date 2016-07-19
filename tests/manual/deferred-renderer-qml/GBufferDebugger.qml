/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    readonly property Layer layer: debugLayer

    components : [
        Layer { id: debugLayer },

        PlaneMesh {
            width: 2.0
            height: 2.0
            meshResolution: Qt.size(2, 2)
        },

        Transform { // We rotate the plane so that it faces us
            rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 90)
        },

        Material {
            effect : Effect {
                techniques: [
                    Technique {
                        graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.NoProfile; minorVersion : 0; majorVersion : 2 }
                        renderPasses: RenderPass {
                            filterKeys: FilterKey { name: "pass"; value: "final" }
                            shaderProgram: ShaderProgram {
                                vertexShaderCode:
                                    "#version 110

                                    attribute vec4 vertexPosition;
                                    uniform mat4 modelMatrix;

                                    void main()
                                    {
                                        gl_Position = modelMatrix * vertexPosition;
                                    }"

                                fragmentShaderCode:
                                    "#version 110

                                    uniform sampler2D color;
                                    uniform sampler2D position;
                                    uniform sampler2D normal;
                                    uniform sampler2D depth;
                                    uniform vec2 winSize;

                                    void main()
                                    {
                                        vec2 texCoord = (gl_FragCoord.xy + vec2(-winSize.x, 0)) / winSize;

                                        // Draw 4 quadrants
                                        if (texCoord.x > 0.5) { // Right
                                            if (texCoord.y > 0.5) { // Top
                                                gl_FragColor = vec4(texture2D(normal, vec2(texCoord.x - 0.5, texCoord.y - 0.5) * 2.0).xyz, 1.0);
                                            } else { // Bottom
                                                gl_FragColor = vec4(texture2D(color, vec2(texCoord.x - 0.5, texCoord.y) * 2.0).xyz, 1.0);
                                            }
                                        } else { // Left
                                            if (texCoord.y > 0.5) { // Top
                                                gl_FragColor = texture2D(position, vec2(texCoord.x, texCoord.y - 0.5) * 2.0);
                                            } else { // Bottom
                                                gl_FragColor = vec4(texture2D(depth, texCoord * 2.0).rrr, 1.0);
                                            }
                                        }
                                        gl_FragColor.a = 0.5;
                                    }"
                            }
                        }
                    }
                ]
            }
        }
    ]
}
