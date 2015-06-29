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

Entity {
    id: root

    property string potShape: "cross"
    property string plantType: "bamboo"

    property alias x: pot.x
    property alias y: pot.y
    property alias z: pot.z
    property real scale: 1.0

    RenderableEntity {
        id: pot
        source: "assets/houseplants/" + root.potShape + "-pot.obj"
        scale: 0.03 * root.scale

        material: NormalDiffuseMapMaterial {
            diffuse: "assets/houseplants/pot.webp"
            normal: "assets/houseplants/pot_normal.webp"
            specular: Qt.rgba( 0.75, 0.75, 0.75, 1.0 )
            shininess: 10.0
        }

        RenderableEntity {
            source: "assets/houseplants/" + root.potShape + "-" + root.plantType + ".obj"
            material: NormalDiffuseMapAlphaMaterial {
                diffuse: "assets/houseplants/"  + root.plantType + ".webp"
                normal: "assets/houseplants/" + root.plantType + "_normal.webp"
                shininess: 10.0
            }
        }

        RenderableEntity {
            source: "assets/houseplants/" + root.potShape + "-pot-cover.obj"
            material: NormalDiffuseMapMaterial {
                diffuse: "assets/houseplants/cover.webp"
                normal: "assets/houseplants/cover_normal.webp"
                specular: Qt.rgba( 0.05, 0.05, 0.05, 1.0 )
                shininess: 5.0
            }
        }
    }
}
