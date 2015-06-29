/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

Entity {

    components: FrameGraph {
        ForwardRenderer {}
    }


    Entity {
        components : [
            PlaneMesh {
                width: 2.0
                height: 2.0
                meshResolution: Qt.size(2, 2)
            },

            Transform { // Rotate the plane so that it faces us
                Rotate {
                    axis: Qt.vector3d(1.0, 0.0, 0.0)
                    angle: 90
                }
            },

            Material
            {
                parameters: Parameter { name: "winsize"; value: Qt.vector2d(_window.width, _window.height) }

                effect: Effect {
                    techniques: Technique {

                        openGLFilter {
                            minorVersion: 3
                            majorVersion: 2
                            api: OpenGLFilter.Desktop
                            profile: OpenGLFilter.Core
                        }

                        renderPasses: RenderPass {
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/plasma.vert")
                                fragmentShaderCode: loadSource("qrc:/plasma.frag")

                            }
                        }
                    }
                }
            }
        ]
    }
}
