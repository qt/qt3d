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
import QtQuick 2.4 as QQ2

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 800/600
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration  { controlledCamera: camera }

    components: FrameGraph { activeFrameGraph: ForwardRenderer{ camera: camera ; clearColor: Qt.rgba(0.0, 0.5, 1, 1) } }

    TorusMesh {
        id: torusMesh
        radius: 5
        minorRadius: 1
        rings: 100
        slices: 20
    }

    SphereMesh {
        id: sphereMesh
        radius: 3
    }

    Transform {
        id: sphereTransform
        Translate { dx: 10 }
    }

    Transform {
        id: cylinderTransform
        Translate { dx: -10 }
    }

    Entity {
        id: torusEntity
        components: [ torusMesh, phongMaterial ]
    }

    Entity {
        id: sphereEntity
        components: [ sphereMesh, alphaMaterial, sphereTransform ]
    }

    Entity {
        id: cylinderEntity
        components: [ sphereMesh, alphaMaterial, cylinderTransform ]
    }

    Material
    {
        id: alphaMaterial

        property real alpha: 0.5

        QQ2.NumberAnimation {
            duration: 2000
            loops: QQ2.Animation.Infinite
            target: alphaMaterial
            property: "alpha"
            from: 0.0
            to: 1.0
            running: true
        }

        effect:  Effect {
            parameters: [
                Parameter { name: "alpha";  value: alphaMaterial.alpha },
                Parameter { name: "ka";   value: "black" },
                Parameter { name: "kd";   value: "blue" },
                Parameter { name: "ks";  value: "white" },
                Parameter { name: "shininess"; value: 75.0 },
                Parameter { name: "lightPosition"; value: Qt.vector4d(1.0, 1.0, 0.0, 1.0) },
                Parameter { name: "lightIntensity"; value: Qt.vector3d(1.0, 1.0, 1.0) }
            ]

            ShaderProgram {
                id: alphaPhong
                vertexShaderCode: loadSource("qrc:/phongalpha.vert")
                fragmentShaderCode: loadSource("qrc:/phongalpha.frag")
            }

            techniques: [
                Technique
                {
                    openGLFilter {
                        api: OpenGLFilter.Desktop
                        profile: OpenGLFilter.None
                        majorVersion: 2
                        minorVersion: 0
                    }

                    renderPasses: RenderPass {
                        renderStates: [
                            CullFace { mode : CullFace.Back },
                            DepthTest { func: DepthTest.Less },
                            DepthMask { mask: false },
                            BlendState {
                                srcRGB: BlendState.SrcAlpha
                                dstRGB: BlendState.OneMinusSrcAlpha
                            }
                            ,BlendEquation {mode: BlendEquation.FuncAdd}
                        ]

                        shaderProgram: alphaPhong
                    }
                },
                Technique
                {
                    openGLFilter {
                        api: OpenGLFilter.ES
                        profile: OpenGLFilter.None
                        majorVersion: 2
                        minorVersion: 0
                    }

                    renderPasses: RenderPass {
                        renderStates: [
                            CullFace { mode : CullFace.Back },
                            DepthTest { func: DepthTest.Less },
                            DepthMask { mask: false },
                            BlendState {
                                srcRGB: BlendState.SrcAlpha
                                dstRGB: BlendState.OneMinusSrcAlpha
                            }
                            ,BlendEquation {mode: BlendEquation.FuncAdd}
                        ]

                        shaderProgram: alphaPhong
                    }
                }
            ]
        }
    }

    PhongMaterial {
        id: phongMaterial
    }
}
