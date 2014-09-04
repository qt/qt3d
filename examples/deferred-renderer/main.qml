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

import QtQuick 2.2 as QQ2
import Qt3D 2.0
import Qt3D.Render 2.0

Entity {
    id : root

    RenderTarget {
        id : gBuffer
        attachments : [

            RenderAttachment {
                name : "color"
                type : RenderAttachment.ColorAttachment0
                texture : Texture {
                    id : colorAttachment
                    target : Texture.Target2D
                    width : 1024
                    height : 768
                    format : Texture.RGBA32F
                    generateMipMaps : false
                    magnificationFilter : Texture.Linear
                    minificationFilter : Texture.Linear
                    wrapMode : Texture.ClampToEdge
                }
            },
            RenderAttachment {
                name : "position"
                type : RenderAttachment.ColorAttachment1
                texture : Texture {
                    id : positionAttachment
                    target : Texture.Target2D
                    width : 1024
                    height : 768
                    format : Texture.RGB32F
                    generateMipMaps : false
                    magnificationFilter : Texture.Linear
                    minificationFilter : Texture.Linear
                    wrapMode : Texture.ClampToEdge
                }
            },
            RenderAttachment {
                name : "normal"
                type : RenderAttachment.ColorAttachment2
                texture : Texture {
                    id : normalAttachment
                    target : Texture.Target2D
                    width : 1024
                    height : 768
                    format : Texture.RGB16F
                    generateMipMaps : false
                    magnificationFilter : Texture.Linear
                    minificationFilter : Texture.Linear
                    wrapMode : Texture.ClampToEdge
                }
            },
            RenderAttachment {
                name : "depth"
                type : RenderAttachment.DepthAttachment
                texture : Texture {
                    id : depthAttachment
                    target : Texture.Target2D
                    width : 1024
                    height : 768
                    format : Texture.D32F
                    generateMipMaps : false
                    magnificationFilter : Texture.Linear
                    minificationFilter : Texture.Linear
                    wrapMode : Texture.ClampToEdge
                }
            }
        ] // attachments
    }

    components : FrameGraph {
        id : deferredFrameGraphComponent
        activeFrameGraph: DeferredRenderer {
            gBuffer : gBuffer
            camera : camera
        }
    }

    Configuration  {
        controlledCamera: camera
    }

    Entity {
        id : screenQuadEntity
        components : [
            Layer {
                name : "screenQuad"
            },
            PlaneMesh {
                width: 2.0
                height: 2.0
                meshResolution: Qt.size(2, 2)
            },
            Transform { // We rotate the plane so that it faces us
                Rotate {
                    axis : Qt.vector3d(1.0, 0.0, 0.0)
                    angle : 90
                }
            },
            Material {
                parameters : [
                    Parameter { name : "color"; value : colorAttachment },
                    Parameter { name : "position"; value : positionAttachment },
                    Parameter { name : "normal"; value : normalAttachment },
                    Parameter { name : "winSize"; value : Qt.size(1024, 768) }
                ]
                effect : Effect {
                    techniques : Technique {
                        openGLFilter {api : OpenGLFilter.Desktop; profile : OpenGLFilter.Core; minorVersion : 1; majorVersion : 3 }
                        renderPasses : RenderPass {
                            criteria : Criterion { name : "pass"; value : "final" }
                            shaderProgram : ShaderProgram {
                                id : finalShader
                                vertexShader:
                                    "
                                    #version 140

                                    in vec4 vertexPosition;
                                    uniform mat4 modelMatrix;

                                    void main()
                                    {
                                        gl_Position = modelMatrix * vertexPosition;
                                    }
                                "
                                fragmentShader:
                                    "
                                    #version 140

                                    uniform sampler2D color;
                                    uniform sampler2D position;
                                    uniform sampler2D normal;
                                    uniform vec2 winSize;

                                    out vec4 fragColor;

                                    struct PointLight
                                    {
                                        vec3 position;
                                        vec3 direction;
                                        vec4 color;
                                        float intensity;
                                    };

                                    const int lightCount = 3;
                                    uniform PointLight pointLights[lightCount];

                                    void main()
                                    {
                                        vec2 texCoord = gl_FragCoord.xy / winSize;
                                        vec4 col = texture2D(color, texCoord);
                                        vec3 pos = texture2D(position, texCoord).xyz;
                                        vec3 norm = texture2D(normal, texCoord).xyz;

                                        vec4 lightColor;
                                        for (int i = 0; i < 3; i++) {
                                            vec3 s = normalize(pointLights[i].position - pos);
                                            lightColor += pointLights[i].color * (pointLights[i].intensity * max(dot(s, norm), 0.0));
                                        }
                                        lightColor /= float(lightCount);
                                        fragColor = col * lightColor;
                                    }
                                "
                            }
                        }
                    }
                }
            }
        ]

    }

    Entity {
        id : sceneEntity

        components : PointLight {
            color : "red"
            intensity : 3.0
        }

        Camera {
            id: camera
            lens : CameraLens {
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: 16/9
                nearPlane : 0.01
                farPlane : 1000.0
            }
            transform : Transform {
                LookAt {
                    position: Qt.vector3d( 10.0, 10.0, -25.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 0.0, 0.0, 10.0 )
                }
            }
        }

        Layer {
            id : sceneLayer
            name : "scene"
        }

        SphereMesh {
            id : sphereMesh
            rings: 50
            slices: 100
        }

        Effect {
            id : sceneMaterialEffect
            techniques : Technique {
                openGLFilter {api : OpenGLFilter.Desktop; profile : OpenGLFilter.Core; minorVersion : 1; majorVersion : 3 }
                renderPasses : RenderPass {
                    criteria : Criterion { name : "pass"; value : "geometry" }
                    shaderProgram : ShaderProgram {
                        id : sceneShader
                        vertexShader:
                            "
                                #version 140

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
                        fragmentShader:
                            "
                                #version 140

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
            }
        }

        Entity {
            id : sphere1

            property Material material : Material {
                effect : sceneMaterialEffect
                parameters : Parameter { name : "meshColor"; value : "dodgerblue" }
            }

            property Transform transform : Transform {
                Translate{ dx: -10; dy: 0; dz : 25 }
            }

            property PointLight light : PointLight {
                color : "yellow"
                intensity : 1.0
            }

            components : [
                sphereMesh,
                material,
                transform,
                light,
                sceneLayer
            ]
        }

        Entity {
            id : sphere2

            property Material material : Material {
                effect : sceneMaterialEffect
                parameters : Parameter { name : "meshColor"; value : "green" }
            }

            property PointLight light : PointLight {
                color : "white"
                intensity : 3.0
            }

            components : [
                sphereMesh,
                material,
                light,
                sceneLayer
            ]
        }

    }
}
