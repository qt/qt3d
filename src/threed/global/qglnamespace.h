/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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

#ifndef QGLNAMESPACE_H
#define QGLNAMESPACE_H

#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QSize;

namespace QGL
{
    enum VertexAttribute {
        Position,
        Normal,
        Color,
        TextureCoord0,
        TextureCoord1,
        TextureCoord2,
        CustomVertex0,
        CustomVertex1,
        UserVertex
    };

    enum Face
    {
        FrontFaces                  = 0x0404, // GL_FRONT
        BackFaces                   = 0x0405, // GL_BACK
        AllFaces                    = 0x0408  // GL_FRONT_AND_BACK
    };

    enum DrawingMode
    {
        Points                      = 0x0000, // GL_POINTS
        Lines                       = 0x0001, // GL_LINES
        LineLoop                    = 0x0002, // GL_LINE_LOOP
        LineStrip                   = 0x0003, // GL_LINE_STRIP
        Triangles                   = 0x0004, // GL_TRIANGLES
        TriangleStrip               = 0x0005, // GL_TRIANGLE_STRIP
        TriangleFan                 = 0x0006, // GL_TRIANGLE_FAN
        LinesAdjacency              = 0x000A, // GL_LINES_ADJACENCY
        LineStripAdjacency          = 0x000B, // GL_LINE_STRIP_ADJACENCY
        TrianglesAdjacency          = 0x000C, // GL_TRIANGLES_ADJACENCY
        TriangleStripAdjacency      = 0x000D  // GL_TRIANGLE_STRIP_ADJACENCY
    };

    enum StandardEffect
    {
        FlatColor,
        FlatPerVertexColor,
        FlatReplaceTexture2D,
        FlatDecalTexture2D,
        LitMaterial,
        LitDecalTexture2D,
        LitModulateTexture2D
    };

    enum TextureWrap
    {
        Repeat                      = 0x2901,   // GL_REPEAT
        Clamp                       = 0x2900,   // GL_CLAMP
        ClampToBorder               = 0x812D,   // GL_CLAMP_TO_BORDER
        ClampToEdge                 = 0x812F,   // GL_CLAMP_TO_EDGE
        MirroredRepeat              = 0x8370    // GL_MIRRORED_REPEAT
    };

    enum Eye
    {
        NoEye,
        LeftEye,
        RightEye
    };

    enum Smoothing
    {
        NoSmoothing,
        Smooth,
        Faceted
    };

    enum Mouse3DKeys
    {
        Key_Fit                 = 0x01200002,
        Key_TopView             = 0x01200003,
        Key_LeftView            = 0x01200004,
        Key_RightView           = 0x01200005,
        Key_FrontView           = 0x01200006,
        Key_BottomView          = 0x01200007,
        Key_BackView            = 0x01200008,
        Key_RotateCW90          = 0x01200009,
        Key_RotateCCW90         = 0x0120000a,
        Key_ISO1                = 0x0120000b,
        Key_ISO2                = 0x0120000c,
        Key_Button1             = 0x0120000d,
        Key_Button2             = 0x0120000e,
        Key_Button3             = 0x0120000f,
        Key_Button4             = 0x01200010,
        Key_Button5             = 0x01200011,
        Key_Button6             = 0x01200012,
        Key_Button7             = 0x01200013,
        Key_Button8             = 0x01200014,
        Key_Button9             = 0x01200015,
        Key_Button10            = 0x01200016,
        Key_Rotations           = 0x0120001b,
        Key_Translations        = 0x0120001c,
        Key_DominantAxis        = 0x0120001d,
        Key_IncreaseSensitivity = 0x0120001e,
        Key_DecreaseSensitivity = 0x0120001f
    };

    Q_QT3D_EXPORT int nextPowerOfTwo(int value);
    Q_QT3D_EXPORT QSize nextPowerOfTwo(const QSize& value);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
