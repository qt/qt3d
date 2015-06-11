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

import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2

Entity {
    id: root
    objectName: "root"

    // Use the renderer configuration specified in ForwardRenderer.qml
    // and render from the mainCamera
    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
                clearColor: "black"
            }
        }
    ]

    BasicCamera {
        id: mainCamera
        position: Qt.vector3d( 0.0, 0.0, 10.0 )
    }

    Configuration  {
        controlledCamera: mainCamera
    }

    TessellatedWireframeMaterial {
        id: quadMaterial
        ambient: Qt.rgba( 0.2, 0.0, 0.0, 1.0 )
        diffuse: Qt.rgba( 0.8, 0.0, 0.0, 1.0 )

        property real innerLevel : 1.0
        property real outerLevel : 1.0

        innerTessLevel : [innerLevel, innerLevel]
        outerTessLevel : [outerLevel, outerLevel, outerLevel, outerLevel]

        effect: TessellatedWireframeEffect {
            tessellationControlShaderCode: shaderProgram.loadSource("qrc:/shaders/quads.tcs")
            tessellationEvaluationShaderCode: shaderProgram.loadSource("qrc:/shaders/quads.tes")
        }

        QQ2.SequentialAnimation {
            loops: QQ2.Animation.Infinite
            running: true

            QQ2.NumberAnimation {
                target: quadMaterial;
                properties: "innerLevel,outerLevel";
                duration: 1000;
                from: 1.0
                to: 20.0
            }

            QQ2.NumberAnimation {
                target: quadMaterial;
                properties: "innerLevel,outerLevel";
                duration: 1000;
                from: 20.0
                to: 1.0
            }
        }
    }

    TessellatedQuad {
        id: quad
        material: quadMaterial
    }

    // TODO: Add examples for triangle and isoline tessellation modes
}
