/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quadplane.h"

/*!
    \class QuadPlane
    \brief The QuadPlane class holds a 3D model of a large flat plane.
    When drawing a large flat plane, such as a wall or a floor, it is desirable
    to decompose it into smaller units to make the shading effects look correct.
    This class composes a plane out of a configurable number of quads.
*/

/*!
    Construct a new QuadPlane with \a size, subdivided \a level times.  By default
    the plane is 100.0f x 100.0f, and is subdivided 3 times - that is into an
    8 x 8 grid.

    It is centered on the origin, and lies in the z = 0 plane.
*/
QuadPlane::QuadPlane(QObject *parent, QSizeF size, int level)
    : QGLSceneNode(parent)
{
    setObjectName(QLatin1String("QuadPlane"));
    if (level > 8)
        level = 8;
    if (level < 1)
        level = 1;
    int divisions = 1;
    for ( ; level--; divisions *= 2) {}  // integer 2**n
    QSizeF div = size / float(divisions);
    QSizeF half = size / 2.0f;
    QGLBuilder builder;
    QGeometryData zip;
    QGeometryData zip2;
    for (int yy = 0; yy <= divisions; ++yy)
    {
        float y = half.height() - float(yy) * div.height();
        float texY = float(yy) / divisions;
        for (int xx = 0; xx <= divisions; ++xx)
        {
            float x = half.width() - float(xx) * div.width();
            float texX = float(xx) / divisions;
            zip.appendVertex(QVector3D(x, y, 0));
            zip.appendTexCoord(QVector2D(1.0f - texX, 1.0f - texY));
        }
        if (yy > 0)
            builder.addQuadsInterleaved(zip, zip2);
        zip2 = zip;
        zip2.detach();
        zip.clear();
    }
    QGLSceneNode *n = builder.finalizedSceneNode();
    addNode(n);
}
