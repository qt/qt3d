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

#include "qglbezierscenehandler.h"
#include "qglbezierscene.h"
#include "qglbezierpatches.h"
#include "qvector3darray.h"
#include <QtCore/qtextstream.h>

QT_BEGIN_NAMESPACE

QGLAbstractScene *QGLBezierSceneHandler::read()
{
    QTextStream stream(device());
    QGLBezierPatches patches;

    // Read the number of patches from the first line.
    int patchCount;
    stream >> patchCount;
    stream.skipWhiteSpace();
    if (patchCount <= 0)
        return 0;

    // Read the patch indices.
    int depth = 0;
    QArray<int> indices;
    for (int patch = 0; patch < patchCount; ++patch) {
        bool eoln = false;
        for (int index = 0; index < 16; ++index) {
            int value;
            stream >> value;
            indices.append(value - 1);

            QChar sep;
            stream >> sep;
            if (sep == QLatin1Char('\n') || sep == QLatin1Char('\r')) {
                eoln = true;
                break;
            }
            if (sep != QLatin1Char(','))
                return 0;
        }
        if (!eoln) {
            // Optional sub-division depth on the end.
            int value;
            stream >> value;
            if (value > depth)
                depth = value;
        }
        stream.skipWhiteSpace();
    }

    // Read the number of vertices.
    int vertexCount;
    stream >> vertexCount;
    stream.skipWhiteSpace();
    if (vertexCount <= 0)
        return 0;

    // Read the vertex values.
    QVector3DArray vertices;
    for (int vertex = 0; vertex < vertexCount; ++vertex) {
        bool eoln = false;
        qreal coords[3] = {0.0f, 0.0f, 0.0f};
        for (int index = 0; index < 3; ++index) {
            stream >> coords[index];

            QChar sep;
            stream >> sep;
            if (sep == QLatin1Char('\n') || sep == QLatin1Char('\r')) {
                eoln = true;
                break;
            }
            if (sep != QLatin1Char(','))
                return 0;
        }
        vertices.append(coords[0], coords[1], coords[2]);
        if (!eoln) {
            // Optional normal on the end: read it but discard.
            for (int index = 0; index < 3; ++index) {
                stream >> coords[index];

                QChar sep;
                stream >> sep;
                if (sep == QLatin1Char('\n') || sep == QLatin1Char('\r'))
                    break;
                if (sep != QLatin1Char(','))
                    return 0;
            }
        }
        stream.skipWhiteSpace();
    }

    // Check for options at the end of the stream.
    if (!stream.atEnd()) {
        QChar ch;
        stream >> ch;
        if (ch == QLatin1Char('#')) {
            QString options = stream.readLine();
            if (options.contains(QLatin1String("teapot-adjust"))) {
                // Perform the "teapot adjustment" to convert the raw
                // teapot data into something more suitable for applications.
                // i.e., do the equivalent of the following transformation:
                //     matrix.rotate(270.0f, 1.0f, 0.0f, 0.0f);
                //     matrix.scale(0.5f, 0.5f, 0.5f);
                //     matrix.translate(0.0f, 0.0f, -1.5f);
                for (int vertex = 0; vertex < vertexCount; ++vertex) {
                    QVector3D vec = vertices[vertex];
                    qreal x = vec.x();
                    qreal y = vec.y();
                    qreal z = vec.z();
                    z -= 1.5f;
                    x *= 0.5f;
                    y *= 0.5f;
                    z *= 0.5f;
                    qreal y2 = z;
                    qreal z2 = -y;
                    y = y2;
                    z = z2;
                    vertices[vertex] = QVector3D(x, y, z);
                }
            }
            if (options.contains(QLatin1String("reverse-patches"))) {
                // Reverse the patch order to convert clockwise
                // patches into standard anti-clockwise patches.
                QArray<int> newIndices;
                for (int patch = 0; patch < patchCount; ++patch) {
                    int temp[16];
                    for (int index = 0; index < 16; ++index)
                        temp[index] = indices[patch * 16 + index];
                    for (int i = 0; i < 16; ++i)
                        newIndices.append(temp[(i & 0x0C) + (3 - (i % 4))]);
                }
                indices = newIndices;
            }
        }
    }

    // Create the geometry node from the Bezier patch data.
    if (depth != 0)
        patches.setSubdivisionDepth(depth);
    QVector3DArray positions;
    for (int pindex = 0; pindex < indices.size(); ++pindex)
        positions += vertices[indices[pindex]];
    patches.setPositions(positions);
    QGLBuilder geometry;
    geometry << patches;

    // Create a scene with a single object containing the geometry.
    return new QGLBezierScene(geometry.finalizedSceneNode());
}

QGLAbstractScene *QGLBezierSceneHandler::download()
{
    qWarning() << "Network loading is not supported for .bez files.";
    return NULL;
}

QT_END_NAMESPACE
