/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

//! [1]
Image {
    id: screen
    source: "nebula.jpg"
    width: parent.width
    height: parent.height
//! [1]

    // Joystix font available for free from Ray Larabie via
    // http://typodermicfonts.com/the-larabie-fonts-collection
    // Licence does *NOT* allow distribution in ttf form as part of the
    // source package.
    FontLoader { id: customFont; source: "JOYSTIX.TTF" }

//! [2]
    Viewport  {
        id: viewport
        y: 30
        anchors.fill: parent
        visible: false
        navigation: false
        renderMode: Viewport.BufferedRender
//! [2]

        camera: Camera {
            eye.x: cameraTarget.x
            // Keep the lander and pad in view for reasonable values
            eye.y: (Math.abs(lander.x) * 2.0) + (lander.y * 2.0)
                   + (Math.abs(lander.z) * 2.0) + 5.0
            eye.z: lander.z + 20.0
            center: lander.position
        }

        Item3D {
            id:cameraTarget
            x: ((lander.x + landingPad.x) / 2.0)
            y: 0
            z: 0
        }

//! [3]
        Item3D {
            // Landing pad must come before lander so that transparency on
            // the flames is in the correct order
            id: landingPad

            // The landing crater isn't quite centered, and we use the lander's
            // position for scoring, so we're going to adjust the landscape's
            // position and scale to get it to the right size and place.
            scale: 1.4
            x: -0.28
            z: -0.0

            mesh: Mesh { source: "meshes/lunar-landscape.obj" }

            // This is the top of the model, where we want the lander to land.
            // It would be nice if there was API for this
            property real yMax : y + 3.6;
        }
//! [3]

        Quad {
            // Simple drop shadow
            x: lander.x
            y: landingPad.yMax
            z: lander.z
            scale: 2.0
            effect: Effect { texture: "dropshadow.png"; blending: true }
        }

//! [4]
        Item3D {
            id: lander
            scale: 0.5
            mesh: Mesh { source: "meshes/lunar-lander.obj" }
            effect: Effect {
                color: "#aaca00"
                texture: "rusty.png"
                decal: true
            }

            transform: [
                Rotation3D {
                    Behavior on angle { NumberAnimation { duration: 200}}
                    axis: Qt.vector3d(-1.0,0,0)
                    angle: gameLogic.fuel > 0 ? gameLogic.zBoostInput * 50 : 0
                },
                Rotation3D {
                    Behavior on angle { NumberAnimation { duration: 200}}
                    axis: Qt.vector3d(0,0,1.0)
                    angle: gameLogic.fuel > 0 ? gameLogic.xBoostInput * 50 : 0
                }
            ]
//! [4]

            // HACK.  There should be API for this
            property real yMin: -0.37;
            property bool jetsVisible: gameLogic.gameRunning &&
                                       gameLogic.fuel > 0.0
            property real yBoostScaleFactor: (gameLogic.yboosting ? 1.3 : 0.8)
            property real activeScaleFactor: 1.3

            // Draw back to front to avoid depth vs transparancy issues

            // Back
            Jet {
                z: -2.7
                scaleFactor: lander.yBoostScaleFactor -
                             gameLogic.zBoostInput * lander.activeScaleFactor
                enabled: lander.jetsVisible
            }

            // Left
            Jet {
                x: -2.7
                scaleFactor: lander.yBoostScaleFactor -
                             gameLogic.xBoostInput * lander.activeScaleFactor
                enabled: lander.jetsVisible
            }

            // Right
            Jet {
                x: 2.7
                scaleFactor: lander.yBoostScaleFactor +
                             gameLogic.xBoostInput * lander.activeScaleFactor
                enabled:  lander.jetsVisible
            }

            // Front
            Jet {
                z: 2.7
                scaleFactor: lander.yBoostScaleFactor +
                             gameLogic.zBoostInput * lander.activeScaleFactor
                enabled:  lander.jetsVisible
            }
        }

        Rectangle {
            id: fuelGauge
            color: "#333333"
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            width: 15
            visible: true

            Rectangle {
                id: gauge
                anchors.bottom: fuelGauge.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                height: fuelGauge.height * gameLogic.fuel;
                width: fuelGauge.width * 0.8
                color: Qt.rgba(1.0 -gameLogic.fuel,0.2,gameLogic.fuel)
            }
        }

//! [5]
        MouseArea {
            id: gameInputPad
            // TODO: Fix anchors.fill for children of Viewport
            // HACK. Setting width and height temporarily fixes this problem
            width: 720
            height: 480
            enabled: false

            onMouseYChanged: {
                gameLogic.yboosting = true;
                gameLogic.xBoostInput = (mouseX / viewport.width) - 0.5;
                gameLogic.zBoostInput = (mouseY / viewport.height) - 0.5;
            }
//! [5]
            onMouseXChanged: {
                gameLogic.yboosting = true;
                gameLogic.xBoostInput = (mouseX / viewport.width) - 0.5;
                gameLogic.zBoostInput = (mouseY / viewport.height) - 0.5;
            }
            onPressed: {
                gameLogic.yboosting = true;
                gameLogic.xBoostInput = (mouseX / viewport.width) - 0.5;
                gameLogic.zBoostInput = (mouseY / viewport.height) - 0.5;
            }
            onReleased: {
                gameLogic.yboosting = false;
                gameLogic.xBoostInput = 0.0;
                gameLogic.zBoostInput = 0.0;
            }
        }

        // TODO : Key input

        Item {
            id: gameLogic
            visible: false
            property string state: "titleScreen"

            // Game State
            property int score : 0
            property int hiScore : 0
            property bool gameRunning: false
            property real fuel : 1.0;

            property real xBoostInput: 0.0
            property real xVelocity : 0
            property real xBoostFactor: gravity

            property bool yboosting: false
            property real yVelocity : 0
            property real yBoostFactor: gravity * 2.0

            property real zBoostInput: 0.0
            property real zVelocity : 0
            property real zBoostFactor: gravity

            // Constants
            property real gravity: 0.1 / 60.0;
            // Should be about 5 seconds of fuel
            property real fuelConsuptionRate: 1.0 / 60.0 / 5.0

            Timer {
                id: simulationTickTimer
                running: false
                interval: 1000.0 / 60.0
                repeat: true
                onTriggered: {
                    gameLogic.tick()
                }
            }

            function tick() {
                // apply gravity and user inputs to velocities
                yVelocity -= gravity;
                if (fuel > 0.0)
                {
                    if (yboosting)
                    {
                        yVelocity += yBoostFactor;
                        fuel -= fuelConsuptionRate;
                    }
                    xVelocity -= xBoostInput * xBoostFactor;
                    zVelocity -= zBoostInput * zBoostFactor;
                }

                // update lander position
                lander.x += xVelocity;
                lander.y += yVelocity;
                lander.z += zVelocity;

                // Check win condition
                if (lander.y + lander.yMin <= landingPad.yMax)
                {
                    // Correct very fast landings
                    lander.y = landingPad.yMax - lander.yMin;
                    win();
                }
            }

            function win() {
                // Theoretical max score is 2 * 100^5, or 20,000,000,000
                score = Math.floor(sanitize(xVelocity) / sanitize(yVelocity) /
                                   sanitize(zVelocity) / sanitize(lander.x)
                                   / sanitize(lander.z) * (fuel + 1)                                   );
                if (score > hiScore) hiScore = score;
                simulationTickTimer.running = false;
                endGame();
            }

            // When calculating scores, don't divide by zero and ignore sign.
            // Can't have infinite scores, and don't want negative ones!
            function sanitize(value) {
                return Math.max(0.01, Math.abs(value));
            }

            function newGame() {
                titleBar.visible = false;
                viewport.visible = true;
                simulationTickTimer.running = true;
                gameInputPad.enabled = true;
                gameLogic.gameRunning = true;

                // reset state
                score = 0;
                xBoostInput = 0.0
                xVelocity = 0
                zBoostInput = 0.0
                zVelocity = 0
                yboosting = false
                fuel = 1.0;

                // Random starting position
                lander.position = Qt.vector3d(Math.random() * 10.0 - 5.0,
                                              5.0,
                                              Math.random() * 10.0 - 5.0);

                // add a small positive yVelocity to give the player a chance
                // to get their bearings
                yVelocity = 0.1
            }

            function endGame() {
                simulationTickTimer.running = false;
                // Tidy up visuals
                titleBar.visible = true;
                gameInputPad.enabled = false;
                gameLogic.gameRunning = false;
                gameLogic.zBoostInput = 0.0;
                gameLogic.xBoostInput = 0.0;
                gameLogic.yboosting = false;
            }
        }
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        Row {
            id: scoreBar
            anchors.left: parent.left
            anchors.right: parent.right
            Column {
                // Player 1 Score
                width: parent.width / 3.0
                Text {
                    text: "Player 1"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: customFont.name;
                    color: "red"
                }
                Text {
                    id: scoreBoardText
                    text: "Score: " + gameLogic.score
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: customFont.name;
                    color: "white"
                }
            }
            Column {
                // Hi Score
                width: parent.width / 3.0
                Text {
                    text: "Hi Score"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: customFont.name;
                    color: "red"
                }
                Text {
                    text: "Score: " + gameLogic.hiScore
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: customFont.name;
                    color: "white"
                }
            }
            Column {
                // Player 2 Score (Not used)
                width: screen.width / 3.0
                Text {
                    text: "Player 2"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.family: customFont.name;
                    color: "red"
                }
            }
        }

        Text {
            id: titleBar
            anchors.horizontalCenter: parent.horizontalCenter
            y: screen.height / 5.0
            text: "Qt-Lander"
            font.family: customFont.name
            font.pointSize: 42
            color: "white"
            SequentialAnimation on color {
                loops: Animation.Infinite
                ColorAnimation { to: "#ff0000"; duration: 100 }
                ColorAnimation { to: "#ffff00"; duration: 100 }
                ColorAnimation { to: "#00ff00"; duration: 100 }
                ColorAnimation { to: "#00ffff"; duration: 100 }
                ColorAnimation { to: "#0000ff"; duration: 100 }
                ColorAnimation { to: "#ff00ff"; duration: 100 }
            }
        }
    }

    Item {
        id: tapToStart
        visible: titleBar.visible
        anchors.fill: parent
        Text {
            text: "Tap to Play"
            anchors.centerIn: parent
            font.family: customFont.name
            font.pointSize: 20
            color: "white"
        }
        MouseArea {
            // Note - this mousearea will be obscured by the game's mousearea
            // during play
            anchors.fill: parent
            onClicked: gameLogic.newGame();
        }
    }
}
