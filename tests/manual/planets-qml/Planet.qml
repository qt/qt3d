// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Render 2.0
import Qt3D.Extras 2.0

SphereMesh {
    property real r: 0
    property real x: 0
    property real y: 0
    property real z: 0
    property real roll: 0
    property real tilt: 0

    radius: 1.0
    generateTangents: true
    rings: 64
    slices: 64
}
