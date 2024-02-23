// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    property vector3d origin
    property vector3d direction
    property real length: 1

    components: [ mesh, material ]

    function buildVertexBufferData(o, d, l) {
        // Vertices
        var v0 = o
        var v1 = o.plus(d.times(l))

        var vertices = [ v1, v0 ]

        var vertexArray = new Float32Array(vertices.length * 3);
        var i = 0;

        vertices.forEach(function(vec3) {
            vertexArray[i++] = vec3.x;
            vertexArray[i++] = vec3.y;
            vertexArray[i++] = vec3.z;
        });

        return vertexArray;
    }

    Buffer {
        id: vertexBuffer
        data: buildVertexBufferData(root.origin, root.direction, root.length)
    }

    GeometryRenderer {
        id: mesh
        instanceCount: 1
        indexOffset: 0
        firstInstance: 0
        primitiveType: GeometryRenderer.Lines

        geometry:  Geometry {
            Attribute {
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float
                vertexSize: 3
                byteOffset: 0
                count: 2
                name: defaultPositionAttributeName
                buffer: vertexBuffer
            }
        }
    }

    LineMaterial {
        id: material
        maincolor: "white"
    }
}
