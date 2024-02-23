// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.7
import QtQuick.Window 2.2

Item {
    id: appleTVInput

    property real xThreshold: 500
    property real yThreshold: 300

    signal swipeLeft()
    signal swipeRight()
    signal swipeUp()
    signal swipeDown()

    onSwipeLeft: {
        var focusedItem = Window.window.activeFocusItem
        if (typeof(focusedItem.swipeLeft) === "function") {
            focusedItem.swipeLeft()
        }
    }

    onSwipeRight: {
        var focusedItem = Window.window.activeFocusItem
        if (typeof(focusedItem.swipeRight) === "function") {
            focusedItem.swipeRight()
        }
    }

    onSwipeUp: {
        var focusedItem = Window.window.activeFocusItem
        if (typeof(focusedItem.swipeUp) === "function") {
            focusedItem.swipeUp()
        }
    }

    onSwipeDown: {
        var focusedItem = Window.window.activeFocusItem
        if (typeof(focusedItem.swipeDown) === "function") {
            focusedItem.swipeDown()
        }
    }

    MultiPointTouchArea {
        id: multiPointTouchArea

        anchors.fill: parent

        property int startX: 0
        property int startY: 0

        property int pointX: 0
        property int pointY: 0

        property int xIsIncreasing: -1 // 1 - true, 0 - false, -1 - undefined
        property int yIsIncreasing: -1 // 1 - true, 0 - false, -1 - undefined

        property bool pressed: false

        touchPoints: [
            TouchPoint {

                onPressedChanged: {
                    if (pressed) {
                        multiPointTouchArea.startX = x
                        multiPointTouchArea.startY = y
                        multiPointTouchArea.pressed = true
                    } else {
                        multiPointTouchArea.pressed = false
                        multiPointTouchArea.xIsIncreasing = -1
                        multiPointTouchArea.yIsIncreasing = -1
                        multiPointTouchArea.startX = 0
                        multiPointTouchArea.startY = 0
                    }
                }

                onXChanged: {
                    if (multiPointTouchArea.pressed) {
                        var newValue = x - multiPointTouchArea.startX

                        var focusedItem = appleTVInput.Window.window.activeFocusItem

                        if (focusedItem.panningEnabled) {
                            if (multiPointTouchArea.xIsIncreasing === -1) {
                                multiPointTouchArea.xIsIncreasing = (newValue > multiPointTouchArea.startX) ? 1 : 0
                            } else {
                                if (multiPointTouchArea.xIsIncreasing === 1 && newValue < multiPointTouchArea.pointX) {
                                    multiPointTouchArea.startX = x
                                    multiPointTouchArea.xIsIncreasing = 0
                                    multiPointTouchArea.pointX = newValue
                                } else if (multiPointTouchArea.xIsIncreasing === 0 && newValue > multiPointTouchArea.pointX) {
                                    multiPointTouchArea.startX = x
                                    multiPointTouchArea.xIsIncreasing = 1
                                    multiPointTouchArea.pointX = newValue
                                } else {
                                    multiPointTouchArea.pointX = newValue
                                    focusedItem.pannedHorizontally(multiPointTouchArea.pointX / (multiPointTouchArea.width / 2))
                                }
                            }
                        } else {
                            multiPointTouchArea.pointX = newValue

                            if (multiPointTouchArea.pointX > xThreshold) {
                                multiPointTouchArea.startX = x
                                swipeRight()
                            } else if (multiPointTouchArea.pointX < -xThreshold) {
                                multiPointTouchArea.startX = x
                                swipeLeft()
                            }
                        }
                    }
                }

                onYChanged: {
                    if (multiPointTouchArea.pressed) {
                        var newValue = multiPointTouchArea.startY - y

                        var focusedItem = appleTVInput.Window.window.activeFocusItem

                        if (focusedItem.panningEnabled) {
                            if (multiPointTouchArea.yIsIncreasing === -1) {
                                multiPointTouchArea.yIsIncreasing = (newValue > multiPointTouchArea.startY) ? 1 : 0
                            } else {
                                if (multiPointTouchArea.yIsIncreasing === 1 && newValue < multiPointTouchArea.pointY) {
                                    multiPointTouchArea.startY = y
                                    multiPointTouchArea.yIsIncreasing = 0
                                    multiPointTouchArea.pointY = newValue
                                } else if (multiPointTouchArea.yIsIncreasing === 0 && newValue > multiPointTouchArea.pointY) {
                                    multiPointTouchArea.startY = y
                                    multiPointTouchArea.yIsIncreasing = 1
                                    multiPointTouchArea.pointY = newValue
                                } else {
                                    multiPointTouchArea.pointY = newValue
                                    focusedItem.pannedVertically(multiPointTouchArea.pointY / (multiPointTouchArea.height / 2))
                                }
                            }
                        } else {
                            multiPointTouchArea.pointY = newValue

                            if (multiPointTouchArea.pointY > yThreshold) {
                                multiPointTouchArea.startY = y
                                swipeUp()
                            } else if (multiPointTouchArea.pointY < -yThreshold) {
                                multiPointTouchArea.startY = y
                                swipeDown()
                            }
                        }
                    }
                }
            }
        ]
    }
}
