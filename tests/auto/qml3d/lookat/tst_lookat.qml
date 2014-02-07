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

import QtQuick 2.0
import Qt3D 2.0
import QtTest 1.0

Viewport {
    id: viewport

    Item3D {
        id: target1
        x: 5.0
    }

    Item3D {
        id: targetNegativeXAxis
        x: -5.0
    }

    Item3D {
        id: targetUpVector
        y: 5.0
    }

    Item3D {
        id: targetExpectedUpVectorTrue
        z: 5.0
    }

    Item3D {
        id: item1
        transform: LookAt {
            id: lookAt1
            subject: target1
        }
        Item3D {
            id: child1
            z: 5.0
        }
    }

    Item3D {
        id: itemNegativeXAxis
        transform: LookAt {
            id: lookAtNegativeXAxis
            subject: targetNegativeXAxis
        }
        Item3D {
            id: childNegativeXAxis
            z: 5.0
        }
    }

    Item3D {
        id: itemUpVector
        transform: LookAt {
            id: lookAtUpVector
            subject: targetUpVector
        }
        Item3D {
            id: childUpVector
            z: 5.0
        }
    }

    Item3D {
        id: itemUpVectorTrue
        transform: LookAt {
            id: lookAtUpVectorTrue
            subject: targetUpVector
            preserveUpVector: true
        }
        Item3D {
            id: childUpVectorTrue
            z: 5.0
        }
    }

    TestCase {
        name: "LookAt"

        // helper functions
        function vectorToString(vector) {
            return "Vector3D(" + vector.x + ", " + vector.y + ", " + vector.z + ")";
        }

        function fuzzyCompare(float1, float2) {
            return (Math.abs(float1 - float2) < 0.0005);
        }

        function compareVectors(vector1, vector2, string )
        {
            compare(vector1.x, vector2.x, string + " x");
            compare(vector1.y, vector2.y, string + " y");
            compare(vector1.z, vector2.z, string + " z");
        }

        function test_lookAt() {
            var testPosition = child1.localToWorld();
            var targetPosition = target1.localToWorld();
            compare(testPosition.x, targetPosition.x, "test lookAt along X axis: x");
            compare(testPosition.y, targetPosition.y, "test lookAt along X axis: y");
            compare(testPosition.z, targetPosition.z, "test lookAt along X axis: z");

            testPosition = childNegativeXAxis.localToWorld();
            targetPosition = targetNegativeXAxis.localToWorld();
            compare(testPosition.x, targetPosition.x, "test negativeXAxis: x");
            compare(testPosition.y, targetPosition.y, "test negativeXAxis: y");
            compare(testPosition.z, targetPosition.z, "test negativeXAxis: z");

            target1.x = -5.0;
            target1.y = -5.0;
            target1.z = -5.0;
            // Adjust the child's Z offset to match the new target position
            child1.z = Math.sqrt(target1.x*target1.x + target1.y*target1.y +
                                 target1.z*target1.z);

            testPosition = child1.localToWorld();
            targetPosition = target1.localToWorld();

            verify(fuzzyCompare(testPosition.x, targetPosition.x), "test subject position change: x");
            verify(fuzzyCompare(testPosition.y, targetPosition.y), "test subject position change: y");
            verify(fuzzyCompare(testPosition.z, targetPosition.z), "test subject position change: z");
        }

        function test_preserveUpVector() {
            compare(lookAtUpVector.preserveUpVector, false, "default preserveUpVector is false");

            // Verify that the lookAt works:
            var childUpVectorPosition = childUpVector.localToWorld();
            var targetUpVectorPosition = targetUpVector.localToWorld();
            compareVectors(childUpVectorPosition, targetUpVectorPosition, "preserveUpVector false");

            var targetUpVectorPosition = targetUpVector.localToWorld();
            var childUpVectorTruePosition = childUpVectorTrue.localToWorld();
            var expectedUpVectorTruePosition =
                targetExpectedUpVectorTrue.localToWorld();

            compareVectors(childUpVectorTruePosition,
                           expectedUpVectorTruePosition,
                           "preserveUpVector true" );

            // test that changing preserveUpVector works
            lookAtUpVector.preserveUpVector = true;
            compare(lookAtUpVector.preserveUpVector, true, "setPreserveUpVector(true)");
            childUpVectorPosition = childUpVector.localToWorld();
            compareVectors(childUpVectorPosition, expectedUpVectorTruePosition, "preserveUpVector set to true ");

            lookAtUpVector.preserveUpVector = false;
            compare(lookAtUpVector.preserveUpVector, false, "setPreserveUpVector(false)");
            var childUpVectorSetFalsePosition = childUpVector.localToWorld();
            compareVectors(childUpVectorSetFalsePosition,
                           targetUpVectorPosition,
                           "preserveUpVector set to false ");
            return;
        }
    }
}
