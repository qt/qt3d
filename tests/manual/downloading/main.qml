// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    Camera {
        id: camera
        position: Qt.vector3d( 0.0, 0.5, 15.0 )
        viewCenter: Qt.vector3d( 0.0, 0.5, 0.0 )
    }

    OrbitCameraController { camera: camera }

    RenderSettings {
        id : external_forward_renderer
        activeFrameGraph : ForwardRenderer {
            camera: camera
            clearColor: "white"
        }
    }

    // Event Source will be set by the Qt3DQuickWindow
    InputSettings { id: inputSettings }

    DirectionalLight { id: light; worldDirection: camera.viewVector }

    components: [external_forward_renderer, inputSettings, light]

    Mesh {
        id: mesh
        source: "https://codereview.qt-project.org/gitweb?p=qt/qt3d.git;a=blob_plain;f=examples/qt3d/exampleresources/assets/obj/plane-10x10.obj"
        onStatusChanged: console.log("Mesh status " + status)
    }

    Transform {
        id: transform
        scale: 0.03
        rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 30)
    }

    DiffuseMapMaterial {
        id: material
        diffuse: TextureLoader {
            source: "https://codereview.qt-project.org/gitweb?p=qt/qt3d.git;a=blob_plain;hb=refs/heads/dev;f=examples/qt3d/planets-qml/images/solarsystemscope/earthmap2k.jpg"
            onStatusChanged: console.log("TextureLoader status " + status)
        }
        specular: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
        shininess: 2.0
    }

    Entity {
        id: mainEntity
        objectName: "mainEntity"
        components: [ mesh, material, transform ]
    }
}
