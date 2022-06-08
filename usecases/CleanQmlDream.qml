// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

import QtQuick 2.1 as QQ2
import Qt3D 2.0

Node {
    // Node can contains any type of Node or QObject
    id : root
    objectName : "root"


    // Entity can be composed of components and child entities
    // than inherit its attributes (transformations, framegraph rendering)
    Entity {
        id : sceneRoot
        objectName : sceneRoot

        FrameGraph {
            SampleFrameGraphConfiguration {
                viewportRect : Qt.rect(0,0,1,1)
                camera : camera1 // Problem here, we cannot reference an Entity
            }
        }

        // Cameras are Entity composed of an Optic component called Camera
        // Maybe Camera should be its own Entity subclass
        Entity {
            id : camera1
            objectName : "camera1"

            Transform {
                LookAt {}
                Translate {}
                Scale {}
                Rotate {}
            }

            Camera { // Maybe this should be rename to optics
                projectionType: Camera.PerspectiveProjection
                fieldOfView: 22.5
                aspectRatio: 1920.0 / 1080.0
            }
        }

        Entity {
            id : testMesh
            objectName : "testMesh"

            Mesh mesh : Mesh { // subclass of Component
                source : "test_mesh.obj"

                Material { // subclass of Component
                    diffuseColor : "#00ff00"
                    specularColor : "#00ff00"
                }
            }
        }

    } // sceneRoot

} // root
