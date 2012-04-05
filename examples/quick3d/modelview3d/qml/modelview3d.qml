/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Rectangle {
    id: topLevel
    width: 800; height: 480

    // The ModelView architecture has 3 main parts.
    // The Model (in this case "dynamicModel", a ListModel) contains the data
    // (elements) used to construct items.
    // The delegate ("cubeDelegate") describes how to use the data elements
    // to construct items.
    // The the view (in this case an anonymous Repeater) combines the Model
    // and delegate and manages the resulting Items.

    // See the "QML Data Models" documentation for more details on Qt Quick's
    // ModelView Architecture.


    // This ListModel is the primary source of data used to construct the
    // items.
    // In this example, the initial ListElement specifies a white cube at
    // the origin.  You can add as many ListElements as you wish, or you can
    // add more elements dynamically using ListModel.append(), as shown below.

    // ListModel is the simplest data model, but there are several others
    // available.  See the "QML Data Models" documentation for more details.
    ListModel {
        id: dynamicModel
        ListElement { x:0; y:0; z:0; color:"#ffffff"}
    }

    // This function shows how to build new elements and add them to the model.
    // In this example, the data required is pulled directly from the gui
    // items.
    function addItem() {
        // To add new elements, simply create a new object with the
        // desired properties, and append() them to the model.
        var newModelData = {
            "x": navigator.currentX/5,
            "y": navigator.currentY/5,
            "z": navigator.currentZ/5,
            "color": colorPicker.currentColor
        }
        dynamicModel.append( newModelData );
        // It is currently necessary to explicitly call update after changing
        // models.
        viewport.update3d();
    }

    // This component is the delegate (or blueprint) used to create actual
    // items out of model elements.
    Component {
        id: cubeDelegate
        Cube {
            id: item

            // "model" is a special variable that contains the element data for
            // each item drawn from your model.
            x: model.x
            y: model.y
            z: model.z
            effect: Effect {
                id: itemEffect
                // Note that model is still available inside child items, but
                // you should consider using an alias if it makes your code
                // more readable.
                color: model.color
                blending: true
            }

            // Common properties across all items
            scale:  1 / 5

            // If you intend on removing elements from the model,
            // make sure you include this logic.
            // "index" is another special variable in the resulting item that
            // indicates the source element's position in the data model.
            // Note that index can change over the lifetime of the item.  In
            // particular, the -1 index implies that the data used to create
            // this item has been removed from the model, and that this item
            // should be disabled and will most likely be destroyed shortly.
            enabled: index != -1

            // You can have your own functions, signals, animations etc.
            onClicked: {
//                onClicked: console.log("Cube("+index+") color: " + itemEffect.color);
                removeAnimation.start();
            }

            SequentialAnimation  {
                id: removeAnimation
                ColorAnimation { target: itemEffect; property: "color" ;from: model.color; to: Qt.rgba(0,0,0,0); duration: 500 }
                ScriptAction { script: {
                        dynamicModel.remove(index);
                        viewport.update3d();
                    }
                }
            }
            // Component.onCompleted can be a useful place to do
            // initialization, like hooking up signals and slots.
            Component.onCompleted: {
                // console.log("Cube Completed");
            }
        }
    }

    Viewport {
        id: viewport
        // Picking and/or blending will need to be enabled on the viewport
        // if any of the generated items need it.
        picking: true
        blending: true

        // This repeater actually instantiates the items.  The Repeater must
        // be the child of an Item3D or a Viewport for the Item3Ds it creates
        // to be visible in the scene.
        Repeater {
            delegate: cubeDelegate
            model: dynamicModel
        }

        anchors.left: controlPanel.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        camera: Camera {
            id: main_camera
            eye: Qt.vector3d(0, 4, 12)
        }
    }

    // This control panel contains all the gui elements to drive the example.
    // The controls provide the data for constructing building new
    // ListElements (position and color), but there is nothing below this
    // comment that is directly related to the modelview behaviours.
    Rectangle {
        id: controlPanel
        color: "#ffffff"
        width: 210
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        Rectangle {
            id: colorPicker
            color: "white"
            height: sliderContainter.height + titleText.height + 5
            anchors.margins: 5
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            Text {
                id: titleText
                text: qsTr("Color Picker")
                font.bold: true

                anchors.top: parent.top
                anchors.left:parent.left
                anchors.margins: 5
            }

            property color currentColor: "white"
            property real currentRed: 1.0
            property real currentGreen: 1.0
            property real currentBlue: 1.0
            property real currentAlpha: 1.0

            // We use Qt.rgba() to build the colour, which breaks the bindings,
            // so we update manually when the color components change.
            onCurrentRedChanged: updateColor();
            onCurrentGreenChanged: updateColor();
            onCurrentBlueChanged: updateColor();
            onCurrentAlphaChanged: updateColor();

            function updateColor() {
                currentColor = Qt.rgba(colorPicker.currentRed,
                                       colorPicker.currentGreen,
                                       colorPicker.currentBlue,
                                       colorPicker.currentAlpha);
                redSlider.updateColors();
                greenSlider.updateColors();
                blueSlider.updateColors();
                swatch.color = currentColor;
            }

            Rectangle {
                id: swatch
                anchors.margins: 5
                height: 90
                y: 5
                border.width: 2
                border.color: "black"
                color: Qt.rgba(colorPicker.currentRed,
                               colorPicker.currentGreen,
                               colorPicker.currentBlue,
                               colorPicker.currentAlpha);
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: sliderContainter.right
            }

            Rectangle {
                id: sliderContainter
                anchors.bottom: parent.bottom
                width: 125
                height: 110

                Rectangle {
                    id: redSlider
                    property real currentValue: 1.0
                    rotation: -90
                    x: 30
                    y: -20
                    height: 80
                    width: 30
                    border.width: 2
                    border.color: "black"
                    radius: 5

                    property color zeroColor: Qt.rgba(0,
                                                      colorPicker.currentGreen,
                                                      colorPicker.currentBlue,
                                                      colorPicker.currentAlpha)
                    property color fullColor: Qt.rgba(1.0,
                                                      colorPicker.currentGreen,
                                                      colorPicker.currentBlue,
                                                      colorPicker.currentAlpha)

                    function updateColors()
                    {
                        zeroColor = Qt.rgba(0,
                                            colorPicker.currentGreen,
                                            colorPicker.currentBlue,
                                            colorPicker.currentAlpha);
                        fullColor = Qt.rgba(1.0,
                                            colorPicker.currentGreen,
                                            colorPicker.currentBlue,
                                            colorPicker.currentAlpha);
                    }

                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: redSlider.zeroColor}
                        GradientStop {
                            position: 1.0
                            color: redSlider.fullColor
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onMousePositionChanged: {
                            redSlider.currentValue = mouseY / redSlider.height;
                            colorPicker.currentRed = redSlider.currentValue;
                        }
                    }
                }

                Rectangle {
                    id: redSwatch
                    width: redSlider.width
                    height: redSlider.width
                    color: Qt.rgba(1.0, 0, 0);
                    border.width: 2
                    border.color: "black"

                    x: redSlider.height + 10
                    y: 5
                    radius: 5
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {colorPicker.currentRed = 1.0;
                            colorPicker.currentGreen = 0;
                            colorPicker.currentBlue = 0}
                    }
                }

                Rectangle {
                    id: greenSlider
                    rotation: -90
                    x: 30
                    y: 15
                    height: 80
                    width: 30

                    property color zeroColor: Qt.rgba(colorPicker.currentRed,
                                                      0.0,
                                                      colorPicker.currentBlue,
                                                      colorPicker.currentAlpha)
                    property color fullColor: Qt.rgba(colorPicker.currentRed,
                                                      1.0,
                                                      colorPicker.currentBlue,
                                                      colorPicker.currentAlpha)

                    function updateColors()
                    {
                        zeroColor = Qt.rgba(colorPicker.currentRed,
                                            0.0,
                                            colorPicker.currentBlue,
                                            colorPicker.currentAlpha);
                        fullColor = Qt.rgba(colorPicker.currentRed,
                                            1.0,
                                            colorPicker.currentBlue,
                                            colorPicker.currentAlpha);
                    }

                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: greenSlider.zeroColor
                        }
                        GradientStop {
                            position: 1.0
                            color: greenSlider.fullColor
                        }
                    }
                    border.width: 2
                    border.color: "black"
                    radius: 5
                    MouseArea {
                        anchors.fill: parent
                        onMousePositionChanged: {
                            colorPicker.currentGreen = mouseY / greenSlider.height;
                        }
                    }
                }

                Rectangle {
                    id: greenSwatch
                    width: greenSlider.width
                    height: greenSlider.width
                    color: Qt.rgba(0, 1, 0);
                    border.width: 2
                    border.color: "black"

                    x: greenSlider.height + 10
                    y: 40
                    radius: 5
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            colorPicker.currentRed = 0;
                            colorPicker.currentGreen = 1;
                            colorPicker.currentBlue = 0
                        }
                    }
                }

                Rectangle {
                    id: blueSlider
                    rotation: -90
                    x: 30
                    y: 50
                    height: 80
                    width: 30
                    property color zeroColor: Qt.rgba(colorPicker.currentRed,
                                                      colorPicker.currentGreen,
                                                      0.0,
                                                      colorPicker.currentAlpha)
                    property color fullColor: Qt.rgba(colorPicker.currentRed,
                                                      colorPicker.currentGreen,
                                                      1.0,
                                                      colorPicker.currentAlpha)

                    function updateColors()
                    {
                        zeroColor = Qt.rgba(colorPicker.currentRed,
                                            colorPicker.currentGreen,
                                            0.0,
                                            colorPicker.currentAlpha);
                        fullColor = Qt.rgba(colorPicker.currentRed,
                                            colorPicker.currentGreen,
                                            1.0,
                                            colorPicker.currentAlpha);
                    }

                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: blueSlider.zeroColor
                        }
                        GradientStop {
                            position: 1.0
                            color: blueSlider.fullColor
                        }
                    }
                    border.width: 2
                    border.color: "black"
                    radius: 5
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            colorPicker.currentBlue =
                                    mouseY / blueSlider.height;
                        }
                        onMousePositionChanged: {
                            colorPicker.currentBlue =
                                    mouseY / blueSlider.height;
                        }
                    }
                }

                Rectangle {
                    id: blueSwatch
                    width: blueSlider.width
                    height: blueSlider.width
                    color: Qt.rgba(0, 0, 1);
                    border.width: 2
                    border.color: "black"

                    x: blueSlider.height + 10
                    y: 75
                    radius: 5
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            colorPicker.currentRed = 0;
                            colorPicker.currentGreen = 0;
                            colorPicker.currentBlue = 1
                        }
                    }
                }
            }
        }

        Rectangle {
            id: navigator
            height: 200
            anchors.margins: 5
            anchors.top: colorPicker.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            // These properties contain are used by the modelview to populate
            // new elements
            property int currentX:0;
            property int currentY:0;
            property int currentZ:0;

            property int buttonWidth: 45
            property int buttonHeight: 30

            Button {
                width: navigator.buttonWidth
                height: navigator.buttonHeight
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                text: qsTr("Up")
                onClicked: {
                    navigator.currentY += 1;
                    topLevel.addItem();
                }
            }

            Button {
                width: navigator.buttonWidth
                height: navigator.buttonHeight
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                text: qsTr("Down")
                onClicked: {
                    navigator.currentY -= 1;
                    topLevel.addItem();
                }
            }

            Button {
                width: navigator.buttonWidth
                height: navigator.buttonHeight
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Left")
                onClicked: {
                    navigator.currentX -= 1;
                    topLevel.addItem();
                }
            }

            Button {
                width: navigator.buttonWidth
                height: navigator.buttonHeight
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Right")
                onClicked: {
                    navigator.currentX += 1;
                    topLevel.addItem();
                }
            }

            Button {
                width: navigator.buttonWidth
                height: navigator.buttonHeight
                anchors.right: parent.right
                anchors.top: parent.top
                text: qsTr("In")
                onClicked: {
                    navigator.currentZ -= 1;
                    topLevel.addItem();
                }
            }

            Button {
                width: navigator.buttonWidth
                height: navigator.buttonHeight
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                text: qsTr("Out")
                onClicked: {
                    navigator.currentZ += 1;
                    topLevel.addItem();
                }
            }

            Image {
                id: axesImage
                anchors.centerIn: parent
                source: "axes.png"

                property color buttonColor: "#448888"
            }
        }

        Rectangle {
            id: instructions
            anchors.margins: 5
            anchors.top: navigator.bottom
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.right: navigator.right
            Text {
                anchors.fill: parent
                id: instructionsText
                wrapMode: Text.WordWrap
                text: qsTr("Use this navigator to create new boxes and move around"
                           + " 3d space.\n"
                           + "Use the color picker to change the color of "
                           + "generated cubes."
                           //                       + "\n  Click on cubes to remove them from the scene."
                           )
            }
        }
    }
}
