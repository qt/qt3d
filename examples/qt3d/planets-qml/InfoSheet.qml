// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0

Rectangle {
    id: infoSheet

    width: 200
    height: 450
    anchors.verticalCenter: parent.verticalCenter

    property alias planet: planetText.planet
    property alias radius: infoText.radius
    property alias temperature: infoText.temperature
    property alias orbitalPeriod: infoText.orbitalPeriod
    property alias distance: infoText.distance
    property alias exampleDetails: infoText.exampleDetails

    Behavior on opacity { PropertyAnimation {} }

    color: "black"

    Text {
        id: planetText
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter

        property string planet: ""

        font.family: "Helvetica"
        font.pixelSize: 32
        font.weight: Font.Light
        color: "white"

        text: "<p>" + planet + "</p>"
    }

    Text {
        id: infoText
        anchors.top: planetText.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        property string radius: ""
        property string temperature: ""
        property string orbitalPeriod: ""
        property string distance: ""
        property string exampleDetails: ""

        font.family: "Helvetica"
        font.pixelSize: 18
        font.weight: Font.Light
        lineHeight: 1.625 * 18
        lineHeightMode: Text.FixedHeight
        color: "white"

        text: {
            if (planet == "Solar System") {
                "<p>" + exampleDetails + "</p>"
            } else if (planet == "Sun") {
                "<p>Equatorial Diameter:</p><p>" + radius + "</p></br>"
                + "<p>Surface Temperature:</p><p>" + temperature + "</p>"
            } else {
                "<p>Equatorial Diameter:</p><p>" + radius + "</p></br>"
                + "<p>Surface Temperature:</p><p>" + temperature + "</p></br>"
                + "<p>Solar Orbit Period:</p><p>" + orbitalPeriod + "</p></br>"
                + "<p>Distance from Sun:</p><p>" + distance + "</p>"
            }
        }

        onLinkActivated: Qt.openUrlExternally(link)
    }
}

