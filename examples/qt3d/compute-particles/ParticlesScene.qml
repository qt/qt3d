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

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    property alias particleStep: computeMaterial.particleStep
    property alias finalCollisionFactor: computeMaterial.finalCollisionFactor

    readonly property int _SPHERE: 0
    readonly property int _CUBE: 1
    readonly property int _CYLINDER: 2
    readonly property int _TORUS: 3

    property int particlesShape: _SPHERE

    signal reset()

    components: [
        FrameGraph {
            ComputeFrameGraph {
                camera: sceneCamera
            }
        }
    ]

    FirstPersonCameraController { camera: sceneCamera }

    Camera {
        id: sceneCamera
        projectionType: CameraLens.PerspectiveProjection
        viewCenter: Qt.vector3d(0, 0, 0)
        position: Qt.vector3d(0, 0, -800)
        nearPlane: 0.1
        farPlane: 1000
        fieldOfView: 25
        aspectRatio: 1.33
    }

    property int particlesCount: 50 * 1024
    readonly property int floatSize: 4

    function buildParticlesBuffer() {
        var bufferData = new Float32Array(particlesCount * 4 * 3);
        var factor = 500.0;
        for (var i = 0; i < particlesCount; ++i) {
            var positionIdx = i * 12;
            var velocityIdx = i * 12 + 4;
            var colorIdx = i * 12 + 8;

            for (var j = 0; j < 3; ++j) {
                bufferData[positionIdx + j] = (Math.random() - 0.5) * factor;
                bufferData[velocityIdx + j] = Math.random() * 2.0;
                bufferData[colorIdx + j] = 0.75 + Math.sin(((i / 1024.0) + j * 0.333) * 6.0) * 0.25;
            }

            bufferData[positionIdx + 3] = 1.0;
            bufferData[velocityIdx + 3] = 0.0;
            bufferData[colorIdx + 3] = 1.0;
        }
        return bufferData
    }

    Buffer {
        id: particleBuffer
        type: Buffer.VertexBuffer
        //        struct ParticleData
        //        {
        //            vec3 position;      // Aligned to 4 floats
        //            vec3 velocity;      // Aligned to 4 floats
        //            vec3 color;         // Aligned to 4 floats
        //        };
        data: buildParticlesBuffer()
    }

    onReset : {
        particleBuffer.data = buildParticlesBuffer()
    }

    Attribute {
        id: particlePositionDataAttribute
        name: "particlePosition"
        attributeType: Attribute.VertexAttribute
        dataType: Attribute.Float
        dataSize: 3
        divisor: 1
        byteStride: 12 * floatSize
        buffer: particleBuffer
    }

    Attribute {
        id: particleColorDataAttribute
        name: "particleColor"
        attributeType: Attribute.VertexAttribute
        dataType: Attribute.Float
        dataSize: 3
        divisor: 1
        byteOffset: 8 * floatSize
        byteStride: 12 * floatSize
        buffer: particleBuffer
    }

    ComputeMaterial {
        id: computeMaterial
        dataBuffer: particleBuffer
    }

    Entity {
        id: particleComputeEntity
        readonly property ComputeJob particlesComputeJob: ComputeJob {}
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

    CuboidGeometry {
        id: cubeGeometry
        yzMeshResolution: Qt.size(2, 2)
        xzMeshResolution: Qt.size(2, 2)
        xyMeshResolution: Qt.size(2, 2)
        // Additional Attributes
        attributes: [
            particlePositionDataAttribute,
            particleColorDataAttribute
        ]
    }

    CylinderGeometry {
        id: cylinderGeometry
        rings: 10
        slices: 10
        radius: 1
        length: 1.5
        // Additional Attributes
        attributes: [
            particlePositionDataAttribute,
            particleColorDataAttribute
        ]
    }

    TorusGeometry {
        id: torusGeometry
        rings: 10
        slices: 10
        radius: 1
        minorRadius: 0.5
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
            baseVertex: 0
            baseInstance: 0
            primitiveType: GeometryRenderer.Triangles
            geometry:  {
                switch (particlesShape) {
                case _SPHERE:
                    return sphereGeometry;
                case _CUBE:
                    return cubeGeometry;
                case _CYLINDER:
                    return cylinderGeometry;
                case _TORUS:
                    return torusGeometry;
                }
            }
        }

        components: [
            particlesRenderer,
            computeMaterial
        ]
    }
}

