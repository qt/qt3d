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

import QtQuick 2.2 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration  {
        controlledCamera: camera
    }

    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0.5, 0.5, 1, 1)
                camera: camera
            }
        }
    ]

    PerVertexColorMaterial {
        id: material
    }

    function buildVertexBufferData()
    {
        // Vertices
        var v0 = Qt.vector3d(-1.0, 0.0, -1.0)
        var v1 = Qt.vector3d(1.0, 0.0, -1.0)
        var v2 = Qt.vector3d(0.0, 1.0, 0.0)
        var v3 = Qt.vector3d(0.0, 0.0, 1.0)

        // Face Normals
        function normal(v0, v1, v2) {
            return v1.minus(v0).crossProduct(v2.minus(v0)).normalized();
        }
        var n023 = normal(v0, v2, v3)
        var n012 = normal(v0, v1, v2)
        var n310 = normal(v3, v1, v0)
        var n132 = normal(v1, v3, v2)

        // Vector normals
        var n0 = n023.plus(n012).plus(n310).normalized()
        var n1 = n132.plus(n012).plus(n310).normalized()
        var n2 = n132.plus(n012).plus(n023).normalized()
        var n3 = n132.plus(n310).plus(n023).normalized()

        // Colors
        var red = Qt.vector3d(1.0, 0.0, 0.0)
        var green = Qt.vector3d(0.0, 1.0, 0.0)
        var blue = Qt.vector3d(0.0, 0.0, 1.0)
        var white = Qt.vector3d(1.0, 1.0, 1.0)

        var vertices = [
                    v0, n0, red,
                    v1, n1, blue,
                    v2, n2, green,
                    v3, n3, white
                ]

        var vertexArray = new Float32Array(4 * (3 + 3 + 3));
        var i = 0;

        vertices.forEach(function(vec3) {
            vertexArray[i++] = vec3.x;
            vertexArray[i++] = vec3.y;
            vertexArray[i++] = vec3.z;
        });

        return vertexArray;
    }

    function buildIndexBufferData()
    {
        var indexArray = new Uint16Array(12);

        // Front
        indexArray[0] = 0;
        indexArray[1] = 1;
        indexArray[2] = 2;
        // Bottom
        indexArray[3] = 3;
        indexArray[4] = 1;
        indexArray[5] = 0;
        // Left
        indexArray[6] = 0;
        indexArray[7] = 2;
        indexArray[8] = 3;
        // Right
        indexArray[9] = 1;
        indexArray[10] = 3;
        indexArray[11] = 2;

        return indexArray;
    }

    GeometryRenderer {
        id: customMesh
        instanceCount: 1
        baseVertex: 0
        baseInstance: 0
        primitiveType: GeometryRenderer.Triangles
        Buffer {
            id: vertexBuffer
            type: Buffer.VertexBuffer
            data: buildVertexBufferData()
        }

        Buffer {
            id: indexBuffer
            type: Buffer.IndexBuffer
            data: buildIndexBufferData()
        }
        geometry:  Geometry {
            Attribute {
                attributeType: Attribute.VertexAttribute
                dataType: Attribute.Float
                dataSize: 3
                byteOffset: 0
                byteStride: 9 * 4
                count: 4
                name: defaultPositionAttributeName()
                buffer: vertexBuffer
            }

            Attribute {
                attributeType: Attribute.VertexAttribute
                dataType: Attribute.Float
                dataSize: 3
                byteOffset: 3 * 4
                byteStride: 9 * 4
                count: 4
                name: defaultNormalAttributeName()
                buffer: vertexBuffer
            }

            Attribute {
                attributeType: Attribute.VertexAttribute
                dataType: Attribute.Float
                dataSize: 3
                byteOffset: 6 * 4
                byteStride: 9 * 4
                count: 4
                name: defaultColorAttributeName()
                buffer: vertexBuffer
            }

            Attribute {
                attributeType: Attribute.IndexAttribute
                dataType: Attribute.UnsignedShort
                dataSize: 1
                byteOffset: 0
                byteStride: 0
                count: 12
                buffer: indexBuffer
            }
        }
    }

    Transform {
        id: meshTransform
        property real userAngle: 0.0
        scale: 10
        rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), userAngle)
    }

    QQ2.NumberAnimation {
        target: meshTransform
        property: "userAngle"
        duration: 10000
        from: 0
        to: 360

        loops: QQ2.Animation.Infinite
        running: true
    }

    Entity {
        id: sphereEntity
        components: [ customMesh, material, meshTransform ]
    }
}
