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

Viewport  {
    id: viewport
    width: parent.width
    height: parent.height
    fillColor: "black"

    camera: Camera { eye: Qt.vector3d(21,70,300)}

    property color lightColor: Qt.hsla(hue, saturation, lightness, 1.0)
    property real hue: 0
    property real saturation: 1.0
    property real lightness: 1.0

    // This is the actual light
    light: Light {
        id: mazeLight
        position: lightProxy.localToWorld()
        // A dimmer ambient light, so it doesn't light up the "wrong"
        // side of the blocks
        ambientColor: "#888888"
        specularColor: viewport.lightColor
        diffuseColor: viewport.lightColor
        constantAttenuation: 1
        linearAttenuation: 0
        quadraticAttenuation: 0
    }

    Maze {
        // Maze is an Item3D based element encompassing all the blocks and the
        // floor, so that there is something to light
        scale: 10
        id: maze
        y: 0.5
        mazeWidth: 6
        // We use this item3D to position the light relative to the Maze,
        // and update the light to follow it's position as it moves around.
        Item3D {
            id: lightProxy
            scale: 0.5
            y: 0
            x: 1
            z: -maze.mazeWidth + 1

            // Unfortunately, we need to use this signal rather than
            // QML bindings because localToWorld() is not a property.
            onPositionChanged: mazeLight.position = localToWorld();

            // We'll add a sphere here to be a visible source for the light
            // It's fine to combine this with the lightProxy,
            // They are only separated here to try and make the position
            // proxy easier to understand.
            Sphere {
                effect: Effect {
                    color: viewport.lightColor
                    useLighting: false
                }
            }

            SequentialAnimation {
                id: lightAnimation
                running: true

                NumberAnimation {
                    id: xAnimation
                    target: lightProxy;
                    property: "x"; duration: 1200; easing.type: Easing.InOutQuad
                    to: maze.xDestination;
                }
                NumberAnimation {
                    id: zAnimation
                    target: lightProxy;
                    property: "z"; duration: 1200; easing.type: Easing.InOutQuad
                    to: maze.zDestination;
                }
                // It seems like we need this indirection to load the new values
                onCompleted: maze.calculateNewLightDestination()
            }
        }

        onWallFinished: dimmerAnimation.running = true
        onDestinationChanged: lightAnimation.restart()
    }

    // This is the initial dimming animation.
    // At 0 quadratic attenuation, the light goes on forever, but as it
    // increases, the light tapers off faster.
    NumberAnimation {
        id: dimmerAnimation
        target: mazeLight
        property: "quadraticAttenuation"
        from: 0
        to: lightPulse.upperBound
        duration: 1500
        onCompleted: lightPulse.running = true;
    }

    // Animating the lighting attenuation to make the light pulse.
    // Note that there is no easing curve (although there could be), the
    // exponential nature of the attenuation co-efficient just makes it
    // look like it's changing at different rates.
    SequentialAnimation {
        id: lightPulse
        property real period: 2500
        property real upperBound: 0.001
        property real lowerBound: 0.005
        loops: Animation.Infinite
        NumberAnimation {
            target: mazeLight
            property: "quadraticAttenuation"
            from: lightPulse.upperBound
            to: lightPulse.lowerBound
            duration: lightPulse.period / 2.0
        }
        NumberAnimation {
            target: mazeLight
            property: "quadraticAttenuation"
            from: lightPulse.lowerBound
            to: lightPulse.upperBound
            duration: lightPulse.period / 2.0
        }
    }

    // Animating the light color for no good reason.
    SequentialAnimation {
        id: hsvColorAnimation
        running: false
        NumberAnimation  {
            target: viewport
            property: "lightness"
            from:1.0; to:0.5;
            duration: 1000;
        }
        NumberAnimation {
            target: viewport
            property: "hue"
            from:0; to: 1.0;
            duration: 6000;
        }
        NumberAnimation {
            target: viewport
            property: "lightness"
            from:0.5; to:1.0;
            duration: 1000;
        }
    }

    // Trigger the disco mode on keyboard input
    focus: true
    Keys.onPressed: {
        hsvColorAnimation.running = !hsvColorAnimation.running;
    }
}
