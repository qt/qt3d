// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.7
import QtQuick.Window 2.2 as QQW

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick.Scene3D 2.0

Entity {
    id: rootItem

    QQW.Window {
        id: window2
        title: "window2"
        visible: true
        width: 600
        height: 600
    }

    Scene {
        id: scene
        view1: _view
        view2: window2
        Component.onCompleted: scene.view1 = _view
    }
}
