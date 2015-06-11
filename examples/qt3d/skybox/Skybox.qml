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

    property alias cameraPosition: cameraTranslate.translation;
    property string sourceDirectory: "";
    property string extension: ".webp"

    property TextureCubeMap skyboxTexture: TextureCubeMap {
        generateMipMaps: false
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }
        TextureImage { cubeMapFace: Texture.CubeMapPositiveX; source: sourceDirectory + "_posx" + extension }
        TextureImage { cubeMapFace: Texture.CubeMapPositiveY; source: sourceDirectory + "_posy" + extension }
        TextureImage { cubeMapFace: Texture.CubeMapPositiveZ; source: sourceDirectory + "_posz" + extension }
        TextureImage { cubeMapFace: Texture.CubeMapNegativeX; source: sourceDirectory + "_negx" + extension }
        TextureImage { cubeMapFace: Texture.CubeMapNegativeY; source: sourceDirectory + "_negy" + extension }
        TextureImage { cubeMapFace: Texture.CubeMapNegativeZ; source: sourceDirectory + "_negz" + extension }
    }

    ShaderProgram {
        id: gl3SkyboxShader
        vertexShaderCode: loadSource("qrc:/shaders/gl3/skybox.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/gl3/skybox.frag")
    }

    ShaderProgram {
        id: gl2es2SkyboxShader
        vertexShaderCode: loadSource("qrc:/shaders/es2/skybox.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/es2/skybox.frag")
    }

    CuboidMesh {
        id: cuboidMesh
        yzMeshResolution: Qt.size(2, 2)
        xzMeshResolution: Qt.size(2, 2)
        xyMeshResolution: Qt.size(2, 2)
    }

    Transform {
        id: transform
        Translate { id: cameraTranslate }
    }

    Material {
        id: skyboxMaterial
        parameters: Parameter { name: "skyboxTexture"; value: skyboxTexture}

        effect: Effect {
            techniques: [
                // GL3 Technique
                Technique {
                    openGLFilter {
                        api: OpenGLFilter.Desktop
                        profile: OpenGLFilter.Core
                        majorVersion: 3
                        minorVersion: 1
                    }
                    renderPasses: RenderPass {
                        shaderProgram: gl3SkyboxShader
                        renderStates: [
                            // cull front faces
                            CullFace { mode: CullFace.Front },
                            DepthTest { func: DepthTest.LessOrEqual }
                        ]
                    }
                },
                Technique {
                    openGLFilter {
                        api: OpenGLFilter.Desktop
                        profile: OpenGLFilter.None
                        majorVersion: 2
                        minorVersion: 0
                    }
                    renderPasses: RenderPass {
                        shaderProgram: gl2es2SkyboxShader
                        renderStates: [
                            CullFace { mode: CullFace.Front },
                            DepthTest { func: DepthTest.LessOrEqual }
                        ]
                    }
                },
                Technique {
                    openGLFilter {
                        api: OpenGLFilter.ES
                        profile: OpenGLFilter.None
                        majorVersion: 2
                        minorVersion: 0
                    }
                    renderPasses: RenderPass {
                        shaderProgram: gl2es2SkyboxShader
                        renderStates: [
                            CullFace { mode: CullFace.Front },
                            DepthTest { func: DepthTest.LessOrEqual }
                        ]
                    }
                }
            ]
        }
    }

    components: [cuboidMesh, skyboxMaterial, transform]
}

