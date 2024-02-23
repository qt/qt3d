// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


import Qt3D.Input 2.0 as QQ3Input20
import QtQuick 2.0

Item {

    // Misc
    //QQ3Input20.KeyEvent                     // (uncreatable) Qt3DInput::QKeyEvent
    QQ3Input20.KeyboardDevice {}              //Qt3DInput::QKeyboardDevice
    QQ3Input20.KeyboardHandler {}             //Qt3DInput::QKeyboardHandler
    QQ3Input20.InputSettings {}               //Qt3DInput::QInputSettings
    //QQ3Input20.MouseEvent                   // (uncreatable) Qt3DInput::QMouseEvent
    //QQ3Input20.WheelEvent                   // (uncreatable) Qt3DInput::QWheelEvent
    QQ3Input20.MouseHandler {}                //Qt3DInput::QMouseHandler
    QQ3Input20.MouseDevice {}                 //Qt3DInput::QMouseDevice
    //QQ3Input20.AbstractActionInput          // (uncreatable) Qt3DInput::QAbstractActionInput
    QQ3Input20.ActionInput {}                 //Qt3DInput::QActionInput
    //QQ3Input20.AbstractAxisInput            // (uncreatable) Qt3DInput::QAbstractAxisInput
    QQ3Input20.AxisSetting {}                 //Qt3DInput::QAxisSetting
    QQ3Input20.AnalogAxisInput {}             //Qt3DInput::QAnalogAxisInput
    QQ3Input20.ButtonAxisInput {}             //Qt3DInput::QButtonAxisInput
    // Disabled for Qt 6
    //    QQ3Input20.GamepadInput {}                //Qt3DInput::QGamepadInput

}
