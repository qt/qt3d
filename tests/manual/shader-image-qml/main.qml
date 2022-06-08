// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.14
import Qt3D.Render 2.14
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    components: [
        RenderSettings {
            id : external_forward_renderer
            activeFrameGraph : RenderSurfaceSelector {
                Viewport {
                    // Launch Compute Shader Instances
                    DispatchCompute {
                    }

                    // Draw Textures
                    CameraSelector {
                        camera: camera
                        ClearBuffers { buffers: ClearBuffers.ColorDepthBuffer
                            // Ensure we won't be reading from textures if Compute Shader instances aren't completed
                            MemoryBarrier { waitFor: MemoryBarrier.ShaderImageAccess|MemoryBarrier.TextureFetch }
                        }
                    }
                }
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { id: inputSettings }
    ]

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 3.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Texture2D {
        id: inputTexture
        width: 512
        height: 512
        format: Texture.RGBA8_UNorm
        generateMipMaps: false
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }

        // Texture image which are referencing regular image extensions (jpg, png)
        // are loaded through a QImage based texture image generator which generates
        // RGBA8_UNorm based content
        TextureImage {
            source: "qrc:/image_512x512.jpg"
            mirrored: false
        }
    }

    Texture2D {
        id: outputTexture
        width: 512
        height: 512
        format: Texture.RGBA8_UNorm
        generateMipMaps: false
        magnificationFilter: Texture.Linear
        minificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }
    }

    // Compute -> Generate Output Texture Content
    Entity {
        readonly property Material material: Material {
            parameters: [
                Parameter {
                    name: "inputImage"
                    value: ShaderImage {
                        texture: inputTexture
                        layer: 0 // default
                        layered: false // default
                        mipLevel: 0 // default
                        access: ShaderImage.ReadOnly // default is ReadWrite
                        format: ShaderImage.Automatic // default
                    }
                },
                Parameter {
                    name: "outputImage"
                    value: ShaderImage {
                        texture: outputTexture
                        access: ShaderImage.WriteOnly // default is ReadWrite
                        format: ShaderImage.RGBA8_UNorm
                    }
                }
            ]

            effect: Effect {
                techniques: Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 4
                        minorVersion: 3
                    }
                    renderPasses: RenderPass {
                        shaderProgram: ShaderProgram {
                            computeShaderCode:
                                "#version 430

                                 layout(local_size_x = 16, local_size_y = 16) in;
                                 layout(rgba8) readonly uniform image2D inputImage;
                                 layout(rgba8) writeonly uniform image2D outputImage;
                                 uniform float time;
                                 void main(void) {
                                     ivec2 imageCoords = ivec2(gl_GlobalInvocationID.xy);

                                     // Early return if we are past the image's bounds
                                     if (any(greaterThan(imageCoords, imageSize(inputImage))))
                                        return;

                                     // Read from input image
                                     vec4 inputContent = imageLoad(inputImage, imageCoords);

                                     // Store inputContent into output Image
                                     imageStore(outputImage, imageCoords, inputContent * abs(sin(time)));
                                 }"
                        }
                    }
                }
            }
        }

        readonly property ComputeCommand command: ComputeCommand {
            readonly property int localWorkGroupSize: 16
            workGroupX: Math.ceil(inputTexture.width / localWorkGroupSize)
            workGroupY: Math.ceil(inputTexture.height / localWorkGroupSize)
        }

        components: [command, material]
    }

    // Preview Texture Scene
    Entity {
        PlaneMesh {
            id: planeMesh
        }

        components: Transform {
            rotationX: 90
        }

        Entity {
            id: inputTexturePreview
            readonly property Material material: TextureMaterial {
                texture: inputTexture
            }
            readonly property Transform transform: Transform {
                translation: Qt.vector3d(-1, 0, 0)
            }
            components: [planeMesh, material, transform]
        }

        Entity {
            id: outputTexturePreview
            readonly property Material material: TextureMaterial {
                texture: outputTexture
            }
            readonly property Transform transform: Transform {
                translation: Qt.vector3d(1, 0, 0)
            }
            components: [planeMesh, material, transform]
        }
    }
}
