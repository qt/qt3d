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

Item {
    CustomObject { id: theObject }
    property real scaleX : 1.0
    property real scaleY : 1.0
    property real scaleZ : 1.0
    property real translationX : 1.0
    property real translationY : 1.0
    property real translationZ : 1.0
    property real unboundReal : 1.0

    property real variableBoundToMatrix : 1.0
    property real variableBoundToVariantList : 1.0

    property variant matrixWithBoundVariable : Qt3D.matrix4x4 (
            1.0, 0.0, 0.0, variableBoundToMatrix,
            0.0, 1.0, 0.0, 1.0,
            0.0, 0.0, 1.0, 1.0,
            0.0, 0.0, 0.0, 1.0 );

    property variant variantListWithBoundVariable : [
            1.0, 0.0, 0.0, variableBoundToVariantList,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0 ];


    function createMatrix4x4WithConstants()
    {
        theObject.variantProp = 0;
        for (var i = 0; i < 1000; ++i)
        theObject.variantProp = Qt3d.matrix4x4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0 );
    }

    function createOneMatrix4x4WithConstants()
    {
        theObject.variantProp = 0;
        theObject.variantProp = Qt3d.matrix4x4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0 );
    }

    function createMatrix4x4WithVariables()
    {
        theObject.variantProp = 0;
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp = Qt3d.matrix4x4 (
                    scaleX, 0.0, 0.0, translationX,
                    0.0, scaleY, 0.0, translationY,
                    0.0, 0.0, scaleY, translationZ,
                    0.0, 0.0, 0.0, 1.0 );
    }

    function createOneMatrix4x4WithVariables()
    {
        theObject.variantProp = 0;
        theObject.variantProp = Qt3d.matrix4x4 (
                scaleX, 0.0, 0.0, translationX,
                0.0, scaleY, 0.0, translationY,
                0.0, 0.0, scaleY, translationZ,
                0.0, 0.0, 0.0, 1.0 );
    }

    function createVariantListWithConstants()
    {
        theObject.variantProp = 0;
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp = [
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0 ];
    }

    function createOneVariantListWithConstants()
    {
        theObject.variantProp = 0;
        theObject.variantProp = [
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0 ];
    }

    function createVariantListWithVariables() {
        theObject.variantProp = 0;
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp = [
            scalex, 0.0, 0.0, translationX,
            0.0, scaley, 0.0, translationY,
            0.0, 0.0, scalez, translationZ,
            0.0, 0.0, 0.0, 1.0 ];
    }

    function createOneVariantListWithVariables() {
        theObject.variantProp = 0;
        theObject.variantProp = [
            scalex, 0.0, 0.0, translationX,
            0.0, scaley, 0.0, translationY,
            0.0, 0.0, scalez, translationZ,
            0.0, 0.0, 0.0, 1.0 ];
    }

    function modifyMatrix4x4() {
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp.m(2,3) = 1.0;
    }

    function modifyVariantList() {
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp[4] = 2.0;
    }

    function modifyBoundVariable() {
        translationX += 0.1;
    }

    function modifyUnboundVariable() {
        unboundReal += 0.1;
    }

}
