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
#include "qgraphicsrotation3d.h"

#include <QImage>
#include <QPainter>

QGLMaterial *qCreateFloor()
{
    quint32 sz = 512;
    QImage image(QSize(sz, sz), QImage::Format_ARGB32);
    QRgb col = qRgba(196, 196, 200, 255);
    image.fill(col);
    QPainter painter(&image);
    QPen pen = painter.pen();
    pen.setColor(QColor(col).darker());
    pen.setWidth(2);
    painter.setPen(pen);
    for (quint32 i = 0; i < sz; i += (sz / 8))
    {
        painter.drawLine(0, i, sz-1, i);
        painter.drawLine(i, 0, i, sz-1);
    }
    painter.end();
    QGLMaterial *mat = new QGLMaterial;
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setImage(image);
    mat->setTexture(tex);
    return mat;
}

QuadPlane::QuadPlane(QObject *parent, QSizeF size, int level)
    : QGLSceneNode(parent)
    , m_texture(0)
{
    setObjectName("QuadPlane");
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
    QGLMaterial *mat = qCreateFloor();
    m_texture = mat->texture();
    n->setMaterial(mat);
    QGraphicsRotation3D *rot = new QGraphicsRotation3D(n);
    rot->setAxis(QVector3D(1, 0, 0));
    rot->setAngle(90.0f);
    n->addTransform(rot);
}

QuadPlane::~QuadPlane()
{
    if (m_texture)
        m_texture->cleanupResources();
}

