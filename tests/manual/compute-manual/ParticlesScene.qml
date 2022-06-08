// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.14
import Qt3D.Render 2.14
import Qt3D.Extras 2.14
import Qt3D.Input 2.14

Entity {

    property alias computeMode: particlesComputeJob.runType

    function triggerCompute(frames) {
        particlesComputeJob.trigger(frames)
    }

    components: [
        RenderSettings {
            ComputeFrameGraph {
                camera: sceneCamera
            }
            // explicitly set RenderingPolicy to AlwaysRender, as changes in the
            // scene won't be reflected in actual Qt scene-graph changes (due to
            // GPU compute calls)
            renderPolicy: RenderSettings.Always
        },
        InputSettings {}
    ]

    FirstPersonCameraController { camera: sceneCamera }

    Camera {
        id: sceneCamera
        projectionType: CameraLens.PerspectiveProjection
        viewCenter: Qt.vector3d(0, 0, 0)
        position: Qt.vector3d(0, 20, -800)
        nearPlane: 0.1
        farPlane: 1000
        fieldOfView: 60
    }

    property int particlesCount: 1024 * 1024
    readonly property int floatSize: 4

    function buildParticlesBuffer() {
        var byteSizeOfParticleData = 8;
        var bufferData = new Float32Array(particlesCount * byteSizeOfParticleData);
        for (var x = 0; x < 1024; ++x) {
            for (var z = 0; z < 1024; ++z) {
                var i = x * 1024 + z;
                var positionIdx = i * byteSizeOfParticleData;
                var colorIdx = i * byteSizeOfParticleData + 8;

                bufferData[positionIdx] = -1024 + x * 2;
                bufferData[positionIdx + 1] = 0;
                bufferData[positionIdx + 2] = -1024 + z * 2;
                bufferData[positionIdx + 3] = 1.0;

                bufferData[colorIdx] = 0.25 + Math.sin(x / 1024) * 0.75;
                bufferData[colorIdx + 1] = 0.25 + Math.sin(z / 1024) * 0.75;
                bufferData[colorIdx + 2] = 0.25 + Math.sin(x / 1024) * 0.75;
                bufferData[colorIdx + 3] = 1.0;
            }
        }
        return bufferData
    }

    Buffer {
        id: particleBuffer
        //        struct ParticleData
        //        {
        //            vec3 position;      // Aligned to 4 floats
        //            vec3 color;         // Aligned to 4 floats
        //        };
        data: buildParticlesBuffer()
    }

    Attribute {
        id: particlePositionDataAttribute
        name: "particlePosition"
        attributeType: Attribute.VertexAttribute
        vertexBaseType: Attribute.Float
        vertexSize: 3
        divisor: 1
        byteStride: 8 * floatSize
        buffer: particleBuffer
    }

    Attribute {
        id: particleColorDataAttribute
        name: "particleColor"
        attributeType: Attribute.VertexAttribute
        vertexBaseType: Attribute.Float
        vertexSize: 3
        divisor: 1
        byteOffset: 4 * floatSize
        byteStride: 8 * floatSize
        buffer: particleBuffer
    }

    ComputeMaterial {
        id: computeMaterial
        dataBuffer: particleBuffer
    }

    ComputeCommand {
        id: particlesComputeJob
        onRunTypeChanged: {
            // Don't forget to re-enable the command
            // when switching back to continuous
            if (runType === ComputeCommand.Continuous)
                enabled = true
        }
    }

    Entity {
        id: particleComputeEntity
        components: [
            particlesComputeJob,
            computeMaterial
        ]
    }

    SphereGeometry {
        id: sphereGeometry
        rings: 10
        slices: 10
        radius: 1
        // Additional Attributes
        attributes: [
            particlePositionDataAttribute,
            particleColorDataAttribute
        ]
    }

    Entity {
        id: particleRenderEntity
        readonly property GeometryRenderer particlesRenderer: GeometryRenderer {
            instanceCount: particlesCount
            indexOffset: 0
            firstInstance: 0
            primitiveType: GeometryRenderer.Triangles
            geometry:  sphereGeometry
        }

        components: [
            particlesRenderer,
            computeMaterial
        ]
    }
}

