/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 1.0

Viewport  {
    width: 1000
    height: 800
    camera: Camera {
        eye.z: 10
    }

    Item3D {
        id: testItem
        mesh: Mesh { source: "square.3ds" }
        effect: Effect { texture: "test_data/test-image.png" }
    }

    // HACK.
    // The test requires that actual drawing happen,
    // and it would be better to verify that directly.
    Timer {
        running: true
        interval: 5
        onTriggered: Qt.quit()
    }

    //  This test is simply testing to see if the item crashes on startup,
    // so this dummy test is probably not even required
    function test_exists()
    {
        verify(testItem.visible);
        Qt.quit();
    }
}
