/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import Qt3D 1.0
import Qt3D.Shapes 1.0
import QtQuickTest 1.0

Rectangle
{
    id: topLevel
    width: 480; height: 480

    Viewport {
        id: viewport
        width: 480; height: 480
        picking: true

        // Result of manual click
        property color clickedColor: "#000000"

        Quad {
            id: fullScreenQuad

            transform: [
                Rotation3D {
                    axis: Qt.vector3d(1,0,0);
                    angle: 90
                },
                Scale3D {
                    // a scale 2 quad will fill a square viewport
                    scale: 2
                }
            ]
            position: Qt.vector3d(0, 0, 0)
            property color color: "#ff0000"

            effect: Effect {
                color: fullScreenQuad.color
            }

            onClicked: {
                viewport.clickedColor = fullScreenQuad.color
            }
        }

        Quad {
            id: smallerQuad

            transform: [
                Rotation3D {
                    axis: Qt.vector3d(1,0,0);
                    angle: 90
                },
                // Bring quad forwards a little bit to ensure it's drawn
                Translation3D { translate: Qt.vector3d(0,0,0.1)}
            ]

            position: Qt.vector3d(0, 0, 0)
            property color color: "#0000ff"

            effect: Effect {
                color: smallerQuad.color
            }

            onClicked: {
                viewport.clickedColor = smallerQuad.color
            }
        }

        // picking fails until the first paint occurs.  There's no
        // straight-forward way to wait for the first paint, so we'll use a
        // timer to allow the paint to happen.

        Timer {
            id: paintedTest
            running: true
            interval: 1
            repeat: true
            onTriggered: {
                var x = viewport.width / 2;
                var y = viewport.height / 2;
                // Usually succeeds on the first run
//                console.log("objectForPoint(" + x + "," + y + "): " + viewport.objectForPoint(x,y)  );
                if (viewport.objectForPoint(x,y) != null)
                    pickingTestCase.when = true;
            }
        }

        // Time out and fail
        Timer {
            running: true
            interval: 2000
            onTriggered: {
                pickingTestCase.timedOut = true;
                pickingTestCase.when = true;
            }
        }

        TestCase {
            id: pickingTestCase
            name: "Picking Test"

            // Delay test until after first paint
            when: false
            property bool timedOut : false

            property int midX : viewport.width / 2;
            property int midY : viewport.height / 2;
            function test_picking() {
                verify(!timedOut, "timed out");
                verify(viewport.objectForPoint(0,0) == fullScreenQuad,
                       "Didn't find full screen quad in top left corner");
                verify(viewport.objectForPoint(midX,midY) == smallerQuad,
                       "Didn't find smaller quad at midpoint");
                verify(viewport.objectForPoint(-midX,-midY) == null,
                       "Incorrectly found object off viewport");
            }
        }
    }
}
