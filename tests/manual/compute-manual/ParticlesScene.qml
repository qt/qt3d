/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
        type: Buffer.VertexBuffer
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

