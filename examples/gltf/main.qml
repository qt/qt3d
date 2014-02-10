/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0

// For Qt.vector3d() and friends. For some reason this is provided by
// QQuickValueTypeProvider in QtQuick rather than the default value
// type provider in QtQml. So we will need to replicate this in Qt3D
// for the types that we wish to support. Otherwise we'll have to import
// QtQuick 2.1 all over the place.
import QtQuick 2.1 as QQ2

Node {
    id: root
    objectName: "root"

    Scene {
        id: wineScene
        source: ":/assets/gltf/wine/wine.json"

        Entity {
            transforms: [
                Translate {
                    dx: 100
                    dy: 0
                    dz: -100
                }

            ]

            property Mesh myMesh: toyplane
        }

        Entity {
            property Mesh myMesh: toyplane

            transforms: [
                Translate {
                    dx: 0
                    dy: 100
                }

            ]

            property Material mat: greenMaterial
        }

        Entity {
            property Shape myShape: sphere1

            transforms: [
                Translate {
                    dx: 50
                    dy: 50
                }

            ]

            property Material mat: greenMaterial

            Shape {
                id: sphere1
                radius: 25
                rings: 32
                slices: 32
            }
        }

        Entity {
            property Shape myShape: torus1

            transforms: [
                Translate {
                    dx: 50
                    dy: 120
                }

            ]

            property Material mat: blueMaterial

            Shape {
                id: torus1
                radius: 40
                minorRadius: 15
                type: Shape.ShapeTorus
            }
        }


        Entity {
            property Mesh myMesh: toyplane

            transforms: [
                Rotate {
                    angle: 45
                    axis: Qt.vector3d(0, 1, 0)
                },

                Translate {
                    dx: 40
                    dy: 100
                }
            ]

            property Material mat: blueMaterial
        }
    }

    Mesh {
        id: toyplane
        source: ":/assets/obj/toyplane.obj"
    }

    Material {
        id: greenMaterial

        parameters: [
            /*
            Tag {
                name: "kd"
                value: Qt.green
            }
            */

            Tag {
                name: "kd"
                value: Qt.vector3d( 0.2, 1.0, 0.1 )

                QQ2.Vector3dAnimation on value {
                    from: Qt.vector3d(0.0, 1.0, 0.1)
                    to: Qt.vector3d(1.0, 1.0, 0.1)
                    duration: 1000
                    loops: QQ2.Animation.Infinite
                }
            },

            Tag {
                name: "diffuse"
                value: brick
            }
        ]
    }

    Material {
        id: blueMaterial

        parameters: [
            Tag { name: "kd"; value: Qt.vector3d(0.2, 0.2, 1.0) }
        ]
    }

    Texture {
        id: brick
        source: "/Users/jmt/KDAB/Qt3D/qt3d-ng/assets/textures/brick1.jpg"
    }
}
