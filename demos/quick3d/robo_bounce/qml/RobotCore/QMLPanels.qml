/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 1.0

Item {
    Rectangle {
        id: buttonBox1
        x: 30
        y: 110
        width: 60
        height: 160
        opacity: 0.5
        color: "black"
        border.color: "#2D918D"
        border.width: 2
        radius: 5
    }
    PushButton {
        x: buttonBox1.x+10
        y: buttonBox1.y+10
    }
    PushButton {
        x: buttonBox1.x+10
        y: buttonBox1.y+60
    }
    PushButton {
        x: buttonBox1.x+10
        y: buttonBox1.y+110
    }

    Rectangle {
        id: buttonBox2
        x: 550
        y: 110
        width: 60
        height: 160
        opacity: 0.5
        color: "black"
        border.color: "#2D918D"
        border.width: 2
        radius: 5
    }
    PushButton {
        x: buttonBox2.x+10
        y: buttonBox2.y+10
    }
    PushButton {
        x: buttonBox2.x+10
        y: buttonBox2.y+60
    }
    PushButton {
        x: buttonBox2.x+10
        y: buttonBox2.y+110
    }

    states: [
        State {},
        State {}

    ]
}


