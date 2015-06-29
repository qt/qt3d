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
import Qt3D.Input 2.0

Entity {
    id: root

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

    Configuration  { controlledCamera: camera }

    components: FrameGraph {
        activeFrameGraph: ForwardRenderer {
            camera: camera
        }
    }

    Effect {
        id: colorDiffuseEffect
        parameters: [
            Parameter { name: "ambient";   value: Qt.vector3d( 0.1, 0.1, 0.1 ) },
            Parameter { name: "diffuse";   value: Qt.vector3d( 0.7, 0.7, 0.7 ) },
            Parameter { name: "specular";  value: Qt.vector3d( 0.95, 0.95, 0.95 ) },
            Parameter { name: "shininess"; value: root.shininess }
        ]
    }

    SphereMesh {
        id: sphereMesh
    }

    KeyboardController {
        id: keyboardController
    }

    Entity {
        id: sphere1
        property Material material: Material {
            effect: colorDiffuseEffect
        }
        property Transform transform: Transform {
            Translate {
                dx: 5
            }
            Scale {
                scale: sphere1.input.focus ? 2 : 1
            }
        }
        property KeyboardInput input: KeyboardInput {
            controller: keyboardController
            focus: true
            onTabPressed: {
                sphere2.input.focus = true
            }
        }

        components: [material, transform, sphereMesh, input]
    }

    Entity {
        id: sphere2
        property Material material: Material {
            effect: colorDiffuseEffect
        }
        property Transform transform: Transform {
            Translate {
                dx: 0
            }
            Scale {
                scale: sphere2.input.focus ? 2 : 1
            }
        }
        property KeyboardInput input: KeyboardInput {
            controller: keyboardController

            onTabPressed: {
                sphere3.input.focus = true;
            }
        }

        components: [material, transform, sphereMesh, input]
    }

    Entity {
        id: sphere3
        property Material material: Material {
            effect: colorDiffuseEffect
        }
        property Transform transform: Transform {
            Translate {
                id: sphere3dTranslate
                dx: -5
            }
            Scale {
                scale: sphere3.input.focus ? 2 : 1
            }
        }
        property KeyboardInput input: KeyboardInput {
            controller: keyboardController

            onPressed: {
                switch (event.key) {
                case Qt.Key_W:
                    sphere3dTranslate.dy += 0.1;
                    break;
                case Qt.Key_S:
                    sphere3dTranslate.dy -= 0.1;
                    break;
                case Qt.Key_A:
                    sphere3dTranslate.dx += 0.1;
                    break;
                case Qt.Key_D:
                    sphere3dTranslate.dx -= 0.1;
                    break;
                }
            }
            onTabPressed: {
                sphere1.input.focus = true;
            }

        }

        components: [material, transform, sphereMesh, input]
    }
}
