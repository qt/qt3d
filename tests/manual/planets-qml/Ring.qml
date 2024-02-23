// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Render 2.0

// TODO: Once support for meshes from arrays is implemented, replace ring.obj with that
Mesh {
    property real innerRadius
    property real outerRadius
    /*
    property real ringSegments

    property var vertices: []
    property var faces: []
    property var faceVertexUvs: []
    */

    source: "qrc:/meshes/ring.obj"

    // Converted from the _RingGeometry() method in the threex.planets.js extension.
    /*
    function makeRing() {
        innerRadius = innerRadius || 0
        outerRadius = outerRadius || 50
        var thetaSegments = ringSegments || 8
        console.log(innerRadius + "," + outerRadius + "," + thetaSegments)

        var normal = Qt.vector3d(0, 0, 1)

        for (var i = 0; i < thetaSegments; i++) {
            var angleLo = (i / thetaSegments) * Math.PI * 2
            var angleHi = ((i + 1) / thetaSegments) * Math.PI * 2

            var vertex1 = Qt.vector3d(innerRadius * Math.cos(angleLo),
                                      innerRadius * Math.sin(angleLo),
                                      0)
            var vertex2 = Qt.vector3d(outerRadius * Math.cos(angleLo),
                                      outerRadius * Math.sin(angleLo),
                                      0)
            var vertex3 = Qt.vector3d(innerRadius * Math.cos(angleHi),
                                      innerRadius * Math.sin(angleHi),
                                      0)
            var vertex4 = Qt.vector3d(outerRadius * Math.cos(angleHi),
                                      outerRadius * Math.sin(angleHi),
                                      0)

            vertices.push(vertex1)
            vertices.push(vertex2)
            vertices.push(vertex3)
            vertices.push(vertex4)

            var vertexIdx = i * 4

            // Create the first triangle
            var face = Qt.vector4d(vertexIdx + 0, vertexIdx + 1, vertexIdx + 2, normal)
            var uvs = []

            var uv = Qt.vector2d(0, 0)
            uvs.push(uv)
            uv = Qt.vector2d(1, 0)
            uvs.push(uv)
            uv = Qt.vector2d(0, 1)
            uvs.push(uv)

            faces.push(face)
            //faceVertexUvs[0].push(uvs)
            faceVertexUvs.push(uvs)

            // Create the second triangle
            face = Qt.vector4d(vertexIdx + 2, vertexIdx + 1, vertexIdx + 3, normal)
            uvs = []

            uv = Qt.vector2d(0, 1)
            uvs.push(uv)
            uv = Qt.vector2d(1, 0)
            uvs.push(uv)
            uv = Qt.vector2d(1, 1)
            uvs.push(uv)

            faces.push(face)
            //faceVertexUvs[0].push(uvs)
            faceVertexUvs.push(uvs)
        }

        computeFaceNormals()
    }

    // Converted from the computeFaceNormals() method in the three.js extension.
    function computeFaceNormals() {
        var cb = Qt.vector3d(0, 0, 0)
        var ab = Qt.vector3d(0, 0, 0)

        for (var f = 0, fl = faces.length; f < fl; f ++) {
            var face = faces[f]

            var vA = vertices[face.x]
            var vB = vertices[face.y]
            var vC = vertices[face.z]

            cb = vC.minus(vB)
            ab = vA.minus(vB)
            cb = cb.crossProduct(ab)

            cb = cb.normalized()

            face.w = cb
        }
    }
    */
}


