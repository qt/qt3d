// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.2 as QQ2
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d(0.0, -300.0, 50.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    FirstPersonCameraController { camera: camera }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
            }
            // we need to explicitly set the RenderPolicy to AlwaysRender here,
            // because a vertex shader will use time-based rendering, so the
            // scene will change every frame without the scene-graph changing.
            // RenderOnDemand would therefore not render every frame, as it won't
            // detect scene-graph changes
            renderPolicy: RenderSettings.Always
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    property real timeValue: 5

    Material {
        id: instancedPhongMaterial
        effect: Effect {
            techniques: Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    minorVersion: 2
                    majorVersion: 3
                }
                filterKeys: [ FilterKey { name: "renderingStyle"; value: "forward" } ]
                renderPasses: RenderPass {
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: loadSource("qrc:/instanced.vert")
                        fragmentShaderCode: loadSource("qrc:/instanced.frag")
                    }
                }
            }
        }
        parameters: Parameter { name: "instanceCount"; value: cylinderMeshRenderer.view.instanceCount }
    }

    CylinderMesh {
        id: cylinderMeshRenderer
        view.instanceCount: 2000
        rings: 50
        slices: 30
        radius: 2.5
        length: 5.0
    }

    Entity {
        id: torusEntity
        components: [ cylinderMeshRenderer, instancedPhongMaterial ]
    }
}
