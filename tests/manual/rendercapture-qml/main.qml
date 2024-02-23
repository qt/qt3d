// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import Qt3D.Render 2.9
import QtQuick.Scene3D 2.0

Item {

    width: 1250
    height: 700

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10

        Rectangle {
            id: background
            width: 600
            height: 600

            color: "blue"

            Scene3D {
                id: scene3d
                anchors.fill: parent
                multisample: msacheckbox.checked

                aspects: ["input", "logic"]

                CaptureScene {
                    id: scene
                }
            }
        }

        ColumnLayout {

            Button {
                id: button
                text: "Render Capture"

                property var reply
                property bool continuous : checkbox.checked
                property int cid: 1

                function doRenderCapture()
                {
                    reply = scene.requestRenderCapture()
                    console.log(reply)
                    reply.completed.connect(onRenderCaptureComplete)
                }

                function onRenderCaptureComplete()
                {
                    _renderCaptureProvider.updateImage(reply)
                    image.source = "image://rendercapture/" + cid
                    reply.saveImage("capture" + cid + ".png")
                    reply.destroy()
                    cid++
                    if (continuous === true)
                        doRenderCapture()
                }

                onClicked: doRenderCapture()
            }
            RowLayout {
                CheckBox {
                    id: checkbox
                    text: "continuous"
                }
                CheckBox {
                    id: msacheckbox
                    text: "multisample"
                }
            }
            Image {
                id: image
                cache: false
                source: "image://rendercapture/0"
                Layout.maximumWidth: 600
                Layout.minimumWidth: 600
                Layout.maximumHeight: 600
                Layout.minimumHeight: 600
            }
        }
    }
}
