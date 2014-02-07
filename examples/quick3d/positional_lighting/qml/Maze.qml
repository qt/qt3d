/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0
import "."

Item3D {
    id: maze

    property int mazeWidth: 1

    property variant solution: [{x:0, y:0}, {x:1,y:0}, {x:1, y:1}]

    signal wallFinished;
    signal creationFinished;
    signal destinationChanged;

    property int xDestination: Math.round(Math.random() * (maze.mazeWidth -1)) * 2 -maze.mazeWidth + 1;
    property int zDestination: Math.round(Math.random() * (maze.mazeWidth -1)) * 2 -maze.mazeWidth + 1;

    Timer {
        id: creationTimer
        // the 400 is the dropping animation duration
        interval: maze.wallCreationDuration + maze.innerCreationDuration + 400
        onTriggered: {
            maze.creationFinished();
        }
    }

    Timer {
        id: wallCreatedTimer
        // the 400 is the dropping animation duration
        interval: maze.wallCreationDuration + 400
        onTriggered: {
            maze.wallFinished();
        }
    }

    property real wallCreationDuration: 1500
    property real innerCreationDuration: 10000

    SequentialAnimation on x {
        id: shakeAnimation
        running: false
        property real amplitude: 0.5
        property real period: 50.0
        NumberAnimation {
            from: 0
            to: shakeAnimation.amplitude
            duration: shakeAnimation.period / 4.0
        }
        NumberAnimation {
            from: shakeAnimation.amplitude
            to: -shakeAnimation.amplitude
            duration: shakeAnimation.period / 2.0
        }
        NumberAnimation {
            to: 0.0
            duration: shakeAnimation.period / 4.0
        }
    }

    Effect {
        id: wallEffect
        material: Material {
            // bronze
            ambientColor: Qt.rgba(0.125 * 2.0, 0.1275* 2.0, 0.054* 2.0, 1.0)
            diffuseColor: Qt.rgba(0.714* 2.0,
                                  0.4284* 2.0,
                                  0.18144* 2.0, 1.0)
            specularColor: Qt.rgba(0.393548,
                                   0.271906,
                                   0.166721, 1.0)
            shininess: 25.6
        }
    }
    // Positional lighting needs a fair number of vertices to look good,
    // so we use these two meshes instead of the stock meshes.
    Mesh {
        id: subdividedCube
        source: "meshes/subdivided_cube.3ds";
        options: "ForceFaceted"
    }

    Item3D {
        id: floorGrid
        mesh:     Mesh {
            source: "meshes/grid.3ds";
            options: "ForceFaceted"
        }
        effect: Effect {
            material: Material {
                // bronze
                ambientColor: Qt.rgba(0.125 * 2.0, 0.1275* 2.0, 0.054* 2.0, 1.0)
                diffuseColor: Qt.rgba(0.714* 2.0,
                                      0.4284* 2.0,
                                      0.18144* 2.0, 1.0)
                specularColor: Qt.rgba(0.393548,
                                       0.271906,
                                       0.166721, 1.0)
                shininess: 25.6
            }
        }
        // Make the grid cover the whole bottom of the maze
        scale: maze.mazeWidth + 0.5
        // add a tiny bit to avoid artefacts with the bottoms of boxes going
        // trough the floor
        y: -0.501

    }

    Component {
        id: wallComponent
        Item3D {
            id: wallComponentItem
            scale: 0.0
            x: 0
            y: 0
            z: 0
            property real startUpPause: 0
            mesh: subdividedCube
            effect: wallEffect
            SequentialAnimation {
                id: dropInAnimation
                running: startUpPause != 0
                property int dropDuration: 400
                PauseAnimation {
                    id: startUp;
                    duration: wallComponentItem.startUpPause
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: wallComponentItem;
                        properties: "scale"; duration: 250;
                        from: 0.0 ; to: 0.5
                    }
                    NumberAnimation {
                        target: wallComponentItem; property: "y";
                        duration: dropInAnimation.dropDuration;
                        from: 10; to: 0
                    }
                }
                ScriptAction {
                    script: shakeAnimation.start()
                }
            }
        }
    }

    Component.onCompleted: {
        if (wallComponent.status == Component.Ready)
            generateMaze();
        else
            console.log("PANIC");
    }

    function maybeGenerateMaze() {
        if (wallComponent.status == Component.Ready)
            generateMaze();
    }

    function generateMaze() {

        var i,j;
        var westEdge = -mazeWidth;
        var northEdge = -mazeWidth;
        var newWall;

        var pause = 1;
        var numberOfBlocks = 4 * 2 *mazeWidth;
        var cumulativePauseFactor = Math.max(1, Math.floor(maze.wallCreationDuration / numberOfBlocks));

        // Create the outer wall

        // North Wall
        for (i = 0; i < 2 * mazeWidth; i++)
        {
            newWall = wallComponent.createObject(maze);
            newWall.x = westEdge + i;
            newWall.z = northEdge;
            newWall.startUpPause = pause;
            pause += cumulativePauseFactor;
        }
        // East Wall
        for (i = 0; i < 2 * mazeWidth + 1; i++)
        {
            newWall = wallComponent.createObject(maze);
            newWall.x = westEdge + 2 * mazeWidth;
            newWall.z = northEdge + i;
            newWall.startUpPause = pause;
            pause += cumulativePauseFactor;
        }
        // Southwall
        for (i = 2 * mazeWidth -1; i >= 1 ; i--)
        {
            newWall = wallComponent.createObject(maze);
            newWall.x = westEdge + i;
            newWall.z = northEdge + 2 * mazeWidth;
            newWall.startUpPause = pause;
            pause += cumulativePauseFactor;
        }
        // West wall
        for (i = 2 * mazeWidth; i >= 1; i--)
        {
            newWall = wallComponent.createObject(maze);
            newWall.x = westEdge;
            newWall.z = northEdge + i;
            newWall.startUpPause = pause;
            pause += cumulativePauseFactor;
        }

        // Create the inner pillars
        for (i = 1; i < mazeWidth; i++)
        {
            for (j = 1; j < mazeWidth; j++)
            {
                newWall = wallComponent.createObject(maze);
                newWall.x = westEdge + i * 2
                newWall.z = northEdge + j * 2
                newWall.startUpPause = pause + Math.random() * maze.innerCreationDuration;
            }
        }

        // Setup notifications
        creationTimer.start();
        wallCreatedTimer.start();
        return;
    }

    function calculateNewLightDestination() {
        var xRange = 2 * (maze.mazeWidth - 1);
        var xMax = maze.mazeWidth - 1;

        maze.xDestination = Math.round(Math.random() * (maze.mazeWidth -1)) * 2 -maze.mazeWidth + 1;
        maze.zDestination = Math.round(Math.random() * (maze.mazeWidth -1)) * 2 -maze.mazeWidth + 1;
        destinationChanged();
    }

    Material {
        id: bronze
        ambientColor: Qt.rgba(0.2125, 0.1275, 0.054, 1.0)
        diffuseColor: Qt.rgba(0.714,
                              0.4284,
                              0.18144, 1.0)
        specularColor: Qt.rgba(0.393548,
                               0.271906,
                               0.166721, 1.0)
        shininess: 25.6
    }
}
