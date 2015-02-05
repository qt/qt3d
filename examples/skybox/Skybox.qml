/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

Entity {

    property alias cameraPosition: cameraTranslate.translation;
    property string sourceDirectory: "qrc:/assets/cubemaps/miramar/miramar";
    property string extension: ".webp"

    property TextureCubeMap skyboxTexture: TextureCubeMap {
        generateMipMaps: false
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
            z: WrapMode.ClampToEdge
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

        vertexShaderCode: "
        #version 140

        in vec3 vertexPosition;
        out vec3 texCoord0;

        uniform mat4 mvp;
        uniform mat4 inverseProjectionMatrix;
        uniform mat4 inverseModelView;

        void main()
        {
            texCoord0 = vertexPosition.xyz;
            gl_Position = vec4(mvp * vec4(vertexPosition, 1.0)).xyww;
        }
        "
        fragmentShaderCode: "
        #version 140

        in vec3 texCoord0;
        out vec4 fragColor;
        uniform samplerCube skyboxTexture;

        void main()
        {
            fragColor = texture(skyboxTexture, texCoord0);
        }
        "
    }

    CuboidMesh {
        id: cuboidMesh
        yzMeshResolution: Qt.size(2, 2)
        xzMeshResolution: Qt.size(2, 2)
        xyMeshResolution: Qt.size(2, 2)
    }

    Transform {
        id: transform
        Translate { id: cameraTranslate; }
    }

    Material {
        id: skyboxMaterial

        parameters: [
            Parameter { name: "skyboxTexture"; value: skyboxTexture}
        ]

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
                }
            ]
        }
    }

    components: [cuboidMesh, skyboxMaterial, transform]
}

