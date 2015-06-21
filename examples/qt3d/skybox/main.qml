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

import Qt3D 2.0
import Qt3D.Renderer 2.0

Entity {
   id: root

   components: FrameGraph {
       ForwardRenderer {
           camera: basicCamera
           clearColor: "black"
       }
   }

   Camera {
       id: basicCamera
       projectionType: CameraLens.PerspectiveProjection
       fieldOfView: 60
       aspectRatio: 16/9
       nearPlane:   0.01
       farPlane:    1000.0
       viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
       upVector:   Qt.vector3d( 0.0, 1.0, 0.0 )
       position: Qt.vector3d( 0.0, 0.0, -40.0 )
   }

   // So that the camera is rendered always at the same position as the camera
   SkyboxEntity {
       cameraPosition: basicCamera.position
       baseName: "qrc:/assets/cubemaps/miramar/miramar"
       extension: ".webp"
   }

   Configuration  {
       controlledCamera: basicCamera
   }

   Entity {
       components: [
           SphereMesh { radius: 5 },
           PhongMaterial { diffuse: "dodgerblue" }
       ]
   }
}

