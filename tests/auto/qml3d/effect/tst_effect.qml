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

    Item3D {
        Effect {
            id: "effect"
        }
    }

    Material {
        id: "china"
        ambientColor: "#c09680"
        specularColor: "#3c3c3c"
        shininess: 128
    }

    TestCase {
        name: "Effect"

        function test_color() {
            verify(effect.color == "#ffffff", "Default color is white");
            effect.color = "#ff4488";
            verify(effect.color == "#ff4488", "setColor()")
        }

        function test_lighting() {
            verify(effect.useLighting == true, "Default useLighting value is true");
            effect.useLighting = false;
            verify(effect.useLighting == false, "setUseLighting()")
        }

        function test_decal() {
            verify(effect.decal == false, "Default decal");
            effect.decal = true;
            verify(effect.decal == true, "setDecal()");
        }

        function test_blending() {
            verify(effect.blending == false, "Default blending");
            effect.blending = true;
            verify(effect.blending == true, "setBlending()");
        }

        function test_texture() {
            verify(effect.texture == "", "Default texture");
            effect.texture = "test.png";
            verify(effect.texture != "");
            var textureString = ("" + effect.texture);
            var protocolString = textureString.substring(0,5);
            var stringLength = textureString.length;
            var fileNameFragment = textureString.substring(stringLength-8,stringLength);
            compare(protocolString, "file:", "recognize local file");
            compare(fileNameFragment, "test.png", "correct file name");
        }

        function test_textureImage() {
            // No support for interrogating QImages from QML
            // so can't write meaningful test case for textureImage
        }

        function test_material() {
            verify(!effect.material, "default material is null")
            effect.material = china;
            compare(effect.material, china, "setMaterial()");
        }

        function test_progress() {
            verify(effect.progress == 0.0);
        }
    }
}
