/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

import Qt 4.7
import Qt3D 1.0
import Qt3D.Shapes 1.0
import QtQuickTest 1.0

Viewport {
    id: viewport

    width: 480
    height: 320

    property string fragmentShader: "\
\
    void main(void)\
    {\
        gl_FragColor = vec4(1,0,1,1);\
    }\
    "

    property string vertexShader: "\
        attribute highp vec4 qt_Vertex;\
        uniform mediump mat4 qt_ModelViewProjectionMatrix;\
\
        void main(void)\
        {\
            gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;\
        }\
        "

    Effect {
        id: testEffect
        material: china
    }

    Cube {
        id: testItemWithNullFragmentShader
        effect: testShaderProgramWithNullFragmentShader
        x: -1.5;
    }

    Cube {
        id: testItem
        effect: testShaderProgram
    }

    Cube {
        id: testItemWithNullVertexShader
        effect: testShaderProgramWithNullVertexShader
        x: 1.5
    }


    // Simple
    Timer {
        running: true
        interval: 1
        onTriggered: {
            shadersTest.when = true;
            viewport.update3d();
        }
    }

    TestCase {
        id: shadersTest

        // Delay test until after first paint
        when: false
        //
        function test_null_fragment_shader() {
            // a test failure is an assert during shader program linking
            // so if execution reaches this point, the test has passed
            verify(true, "shader program didn't crash with null shaders");
        }
    }

    ShaderProgram {
        id: testShaderProgram
        fragmentShader: viewport.fragmentShader
        vertexShader: viewport.vertexShader
    }

    ShaderProgram {
        id: testShaderProgramWithNullFragmentShader

        fragmentShader: ""
        vertexShader: viewport.vertexShader
    }

    ShaderProgram {
        id: testShaderProgramWithNullVertexShader
        fragmentShader: viewport.fragmentShader
        vertexShader: ""
    }

}
