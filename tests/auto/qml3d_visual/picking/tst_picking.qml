/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0
import Qt3D.Shapes 2.0
import QtTest 1.0
import QtQuick 2.0

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
                    // a scale 4 quad will fill a square viewport
                    scale: 4
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

        Component {
            id: pickTestDelegate
            Quad {
                objectName: "foo"
                transform: [
                    Rotation3D {
                        axis: Qt.vector3d(1,0,0);
                        angle: 90
                    },
                    // Bring quad forwards a little bit to ensure it's drawn
                    Translation3D { translate: Qt.vector3d(0,0,0.1)}
                ]

                // Ensure items are above smallerQuad
                position: Qt.vector3d(0, 0, (index + 1) * 0.1)
                property color color: model.color
            }
        }

        ListModel{
            id: pickTestModel
        }

        Repeater {
            delegate: pickTestDelegate
            model: pickTestModel
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
                // if (viewport.objectForPoint(x,y) != null)
                //     pickingTestCase.when = true;
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
                skip("objectForPoint() function is not currently supported");
                verify(!timedOut, "timed out");
                verify(viewport.objectForPoint(0,0) == fullScreenQuad,
                       "Didn't find full screen quad in top left corner");
                verify(viewport.objectForPoint(midX,midY) == smallerQuad,
                       "Didn't find smaller quad at midpoint");
                verify(viewport.objectForPoint(-midX,-midY) == null,
                       "Incorrectly found object off viewport");
                pickTestModel.append({"color":"#00ff00"});
                verify(viewport.objectForPoint(midX,midY).color == "#00ff00",
                       "Didn't find modelview quad at midpoint");
                pickTestModel.append({"color":"#ff00ff"});
                verify(viewport.objectForPoint(midX,midY).color == "#ff00ff",
                       "Didn't find second modelview quad at midpoint");
            }
        }
    }
}
