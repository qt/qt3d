/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
import QtQuick 2.2 as QQ2

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 22.5
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d(10.0, 7.0, 35.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(10.0, 0.5, 0.0)
    }

    Configuration  {
        controlledCamera: camera
    }

    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
            }
        }
    ]

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
                renderPasses: RenderPass {
                    bindings: [
                        ParameterMapping { parameterName: "pos";  shaderVariableName: "pos"; bindingType: ParameterMapping.Attribute }
                    ]
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: loadSource("qrc:/instanced.vert")
                        fragmentShaderCode: loadSource("qrc:/instanced.frag")
                    }
                }
            }
        }
    }

    // Create a GeometryRenderer component that uses the standard CylinderGeometry to
    // create the base vertex and index data buffers and attributes.
    GeometryRenderer {
        id: cylinderMeshInstanced
        enabled: instanceCount != 0

        geometry: CylinderGeometry {
            rings: 50
            slices: 30
            radius: 0.3
            length: 3.0

            attributes: [ instanceDataAttribute ]
        }

        // Use our buffer created from C++ as per-instance position data to render
        // many instances (copies) of the base cylinder geometry in a single OpenGL
        // draw call where supported (OpenGL >=3.3 or OpenGL ES 3). On older versions
        // of OpenGL the instancing is emulated on the CPU using multiple draw calls.
        QQ2.SequentialAnimation {
            running: true
            loops: QQ2.Animation.Infinite

            QQ2.NumberAnimation {
                target: cylinderMeshInstanced
                property: "instanceCount"
                duration: 5000
                from: 0
                to: _instanceBuffer.instanceCount
            }

            QQ2.PauseAnimation {
                duration: 3000
            }

            QQ2.NumberAnimation {
                target: cylinderMeshInstanced
                property: "instanceCount"
                duration: 5000
                from: _instanceBuffer.instanceCount
                to: 0
            }

            QQ2.PauseAnimation {
                duration: 3000
            }
        }

        Attribute {
            id: instanceDataAttribute
            name: "pos"
            attributeType: AbstractAttribute.VertexAttribute
            dataType: AbstractAttribute.Float
            dataSize: 3
            divisor: 1
            buffer: _instanceBuffer
        }
    }

    Entity {
        id: torusEntity
        components: [ cylinderMeshInstanced, instancedPhongMaterial ]
    }
}
