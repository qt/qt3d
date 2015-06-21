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

Effect {
    id : sceneMaterialEffect
    techniques : [
        // OpenGL 3.1
        Technique {
            openGLFilter {api : OpenGLFilter.Desktop; profile : OpenGLFilter.Core; minorVersion : 1; majorVersion : 3 }
            renderPasses : RenderPass {
                annotations : Annotation { name : "pass"; value : "geometry" }
                shaderProgram : ShaderProgram {
                    id : sceneShaderGL3
                    vertexShaderCode:
                        "#version 140

                        in vec4 vertexPosition;
                        in vec3 vertexNormal;

                        out vec4 color0;
                        out vec3 position0;
                        out vec3 normal0;

                        uniform mat4 mvp;
                        uniform mat4 modelView;
                        uniform mat3 modelViewNormal;
                        uniform vec4 meshColor;

                        void main()
                        {
                            color0 = meshColor;
                            position0 = vec3(modelView * vertexPosition);
                            normal0 = normalize(modelViewNormal * vertexNormal);
                            gl_Position = mvp * vertexPosition;
                        }
                    "
                    fragmentShaderCode:
                        "#version 140

                        in vec4 color0;
                        in vec3 position0;
                        in vec3 normal0;

                        out vec4 color;
                        out vec3 position;
                        out vec3 normal;

                        void main()
                        {
                            color = color0;
                            position = position0;
                            normal = normal0;
                        }
                    "
                }
            }
        },
        // OpenGL 2.0
        Technique {
            openGLFilter {api : OpenGLFilter.Desktop; profile : OpenGLFilter.Core; minorVersion : 0; majorVersion : 2 }
            renderPasses : RenderPass {
                annotations : Annotation { name : "pass"; value : "geometry" }
                shaderProgram : ShaderProgram {
                    id : sceneShaderGL2
                    vertexShaderCode:
                        "#version 110

                        attribute vec4 vertexPosition;
                        attribute vec3 vertexNormal;

                        varying vec4 color0;
                        varying vec3 position0;
                        varying vec3 normal0;

                        uniform mat4 mvp;
                        uniform mat4 modelView;
                        uniform mat3 modelViewNormal;
                        uniform vec4 meshColor;

                        void main()
                        {
                            color0 = meshColor;
                            position0 = vec3(modelView * vertexPosition);
                            normal0 = normalize(modelViewNormal * vertexNormal);
                            gl_Position = mvp * vertexPosition;
                        }
                    "
                    fragmentShaderCode:
                        "#version 110

                        varying vec4 color0;
                        varying vec3 position0;
                        varying vec3 normal0;

                        void main()
                        {
                            gl_FragData[0] = color0;
                            gl_FragData[1] = vec4(position0, 0);
                            gl_FragData[2] = vec4(normal0, 0);
                        }
                    "
                }
            }
        }
    ]
}
