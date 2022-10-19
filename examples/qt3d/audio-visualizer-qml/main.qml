// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Scene3D
import QtQuick.Layouts
import QtMultimedia

Item {
    id: mainview
    width: 1215
    height: 720
    visible: true
    property bool isHoverEnabled: false

    property variant magnitudeArray: null
    property int millisecondsPerBar: 68
    property string magnitudeDataSourceFile: "qrc:/music/visualization.raw"
    property int mediaLatencyOffset: 68

    state: "stopped"
    states: [
        State {
            name: "playing"
            PropertyChanges {
                target: playButtonImage
                source: {
                    if (playButtonMouseArea.containsMouse)
                        "qrc:/images/pausehoverpressed.png"
                    else
                        "qrc:/images/pausenormal.png"
                }
            }
            PropertyChanges {
                target: stopButtonImage
                source: "qrc:/images/stopnormal.png"
            }
        },
        State {
            name: "paused"
            PropertyChanges {
                target: playButtonImage
                source: {
                    if (playButtonMouseArea.containsMouse)
                        "qrc:/images/playhoverpressed.png"
                    else
                        "qrc:/images/playnormal.png"
                }
            }
            PropertyChanges {
                target: stopButtonImage
                source: "qrc:/images/stopnormal.png"
            }
        },
        State {
            name: "stopped"
            PropertyChanges {
                target: playButtonImage
                source: "qrc:/images/playnormal.png"
            }
            PropertyChanges {
                target: stopButtonImage
                source: "qrc:/images/stopdisabled.png"
            }
        }
    ]

    Component.onCompleted: isHoverEnabled = touchSettings.isHoverEnabled()

    //![0]
    MediaPlayer {
        id: mediaPlayer
        source: "qrc:/music/tiltshifted_lost_neon_sun.mp3"
        audioOutput: AudioOutput {}
        //![0]

        onMediaStatusChanged: {
            if (mediaStatus == MediaPlayer.EndOfMedia) {
                mainview.state = "stopped"
            } else if (mediaStatus == MediaPlayer.LoadedMedia && !visualizer.started) {
                // Load the pre-calculated magnitude data for the visualizer
                var request = new XMLHttpRequest()
                request.responseType = 'arraybuffer'
                request.onreadystatechange = function() {
                    if (request.readyState === XMLHttpRequest.DONE) {
                        if (request.status == 200 || request.status == 0) {
                            var arrayBuffer = request.response
                            if (arrayBuffer) {
                                magnitudeArray = new Uint16Array(arrayBuffer)
                                visualizer.startVisualization()
                            }
                        } else {
                            console.warn("Couldn't load magnitude data for bars.")
                        }
                        request = null
                    }
                };

                request.open('GET', magnitudeDataSourceFile, true)
                request.send(null)
            }
        }

        onErrorStringChanged: console.error("error with audio " + errorString)

        function getNextAudioLevel(offsetMs) {
            if (magnitudeArray === null)
                return 0.0;

            // Calculate the integer index position in to the magnitude array
            var index = ((mediaPlayer.position + offsetMs) /
                         mainview.millisecondsPerBar) | 0;

            if (index < 0 || index >= magnitudeArray.length)
                return 0.0;

            return (magnitudeArray[index] / 63274.0);
        }
    }

    Image {
        id: coverImage
        anchors.fill: parent
        source: "qrc:/images/albumcover.png"
    }

    //![1]
    Scene3D {
        anchors.fill: parent

        Visualizer {
            id: visualizer
            animationState: mainview.state
            numberOfBars: 120
            barRotationTimeMs: 8160 // 68 ms per bar
        }
    }
    //![1]

    Rectangle {
        id: blackBottomRect
        color: "black"
        width: parent.width
        height: 0.14 * mainview.height
        anchors.bottom: parent.bottom
    }

    // Duration of played content
    Text {
        text: formatDuration(mediaPlayer.position)
        color: "#80C342"
        x: parent.width / 6
        y: mainview.height - mainview.height / 8
        font.pixelSize: 12
    }

    // Duration of the content left
    Text {
        text: "-" + formatDuration(mediaPlayer.duration - mediaPlayer.position)
        color: "#80C342"
        x: parent.width - parent.width / 6
        y: mainview.height - mainview.height / 8
        font.pixelSize: 12
    }

    function formatDuration(milliseconds) {
        var minutes = Math.floor(milliseconds / 60000)
        milliseconds -= minutes * 60000
        var seconds = milliseconds / 1000
        seconds = Math.round(seconds)
        if (seconds < 10)
            return minutes + ":0" + seconds
        else
            return minutes + ":" + seconds
    }

    property int buttonHorizontalMargin: 10
    Rectangle {
        id: playButton
        height: 54
        width: 54
        anchors.bottom: parent.bottom
        anchors.bottomMargin: width
        x: parent.width / 2 - width - buttonHorizontalMargin
        color: "transparent"

        Image {
            id: playButtonImage
            source: "qrc:/images/pausenormal.png"
        }

        MouseArea {
            id: playButtonMouseArea
            anchors.fill: parent
            hoverEnabled: isHoverEnabled
            onClicked: {
                if (mainview.state == 'paused' || mainview.state == 'stopped')
                    mainview.state = 'playing'
                else
                    mainview.state = 'paused'
            }
            onEntered: {
                if (mainview.state == 'playing')
                    playButtonImage.source = "qrc:/images/pausehoverpressed.png"
                else
                    playButtonImage.source = "qrc:/images/playhoverpressed.png"
            }
            onExited: {
                if (mainview.state == 'playing')
                    playButtonImage.source = "qrc:/images/pausenormal.png"
                else
                    playButtonImage.source = "qrc:/images/playnormal.png"
            }
        }
    }

    Rectangle {
        id: stopButton
        height: 54
        width: 54
        anchors.bottom: parent.bottom
        anchors.bottomMargin: width
        x: parent.width / 2 + buttonHorizontalMargin
        color: "transparent"

        Image {
            id: stopButtonImage
            source: "qrc:/images/stopnormal.png"
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: isHoverEnabled
            onClicked: mainview.state = 'stopped'
            onEntered: {
                if (mainview.state != 'stopped')
                    stopButtonImage.source = "qrc:/images/stophoverpressed.png"
            }
            onExited: {
                if (mainview.state != 'stopped')
                    stopButtonImage.source = "qrc:/images/stopnormal.png"
            }
        }
    }
}
