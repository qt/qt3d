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

        // Note that test_material must be called before tests that create
        // a material e.g. setColor() (tests are run in alphabetical order)
        function test_material() {
            verify(!effect.material, "default material is null")
            effect.material = china;
            compare(effect.material, china, "setMaterial()");
        }

        function test_setColor() {
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

        function test_progress() {
            verify(effect.progress == 0.0);
        }
    }
}
