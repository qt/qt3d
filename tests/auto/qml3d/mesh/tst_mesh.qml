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
import QtQuickTest 1.0

Viewport {
    id: viewport

    Mesh {
        id: url_test_mesh
        source: ""
        onDataChanged: { dataChangedCounter += 1 }
        property int dataChangedCounter: 0
    }

    Mesh {
        id: meshName_test_mesh
        onDataChanged: { dataChangedCounter += 1 }
        property int dataChangedCounter: 0
    }

    Mesh {
        id: meshName_test_mesh2
        meshName: "testName"
        onDataChanged: { dataChangedCounter += 1 }
        property int dataChangedCounter: 0
    }

    Mesh {
        id: options_test_mesh
        onOptionsChanged: { optionsChangedCounter += 1}
        property int optionsChangedCounter: 0
    }

    TestCase {
        name: "Mesh"
        id: meshTest

        function test_url() {
            compare(url_test_mesh.dataChangedCounter, 0, "pre-test counter validation")
            compare(url_test_mesh.source, "", "null default source")


            var testString = "test_data/teapot.bez";
            url_test_mesh.source = testString;

            // QUrl contains the absolute file path, so just compare the end
            var substring = url_test_mesh.source.toString().substr(-testString.length,testString.length);
            compare(substring, testString, "setSource() (relative file path)");
            compare(url_test_mesh.dataChangedCounter, 1, "dataChanged signal")
        }

        function test_meshName() {
            meshName_test_mesh.dataChangedCounter = 0;
            compare(meshName_test_mesh.dataChangedCounter, 0, "pre-test counter validation")

            compare(meshName_test_mesh.meshName, "", "null default meshName");
            compare(meshName_test_mesh2.meshName, "testName", "qml assignment test");

            meshName_test_mesh.meshName = "setMeshNameTestName";
            compare(meshName_test_mesh.meshName, "setMeshNameTestName", "setMeshNameTestName");
            compare(meshName_test_mesh.dataChangedCounter, 1, "dataChanged signal")
        }

        function test_options() {
            compare(options_test_mesh.optionsChangedCounter, 0,
                    "pretest validation")
            compare(options_test_mesh.options, "", "null default options");

            options_test_mesh.options="test options string";
            compare(options_test_mesh.options, "test options string",
                    "setOptionsString");

            options_test_mesh.options="test options string";
            compare(options_test_mesh.options,  "test options string",
                    "setOptions()");
            compare(options_test_mesh.optionsChangedCounter, 1,
                    "optionsChanged signal 1");

            options_test_mesh.options=" \\ \ \$";
            compare(options_test_mesh.options, " \\ \ \$",
                    "potentially troublesome characters")
            compare(options_test_mesh.optionsChangedCounter, 2,
                    "optionsChanged signal 2");
        }
    }
}
