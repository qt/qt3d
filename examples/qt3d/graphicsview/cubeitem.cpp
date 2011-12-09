/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cubeitem.h"
#include "qglpainter.h"
#include "qglcube.h"
#include "qray3d.h"
#include "qplane3d.h"
#include "qtriangle3d.h"
#include "qgraphicsembedscene.h"

const qreal CubeSize = 2.0f;

CubeItem::CubeItem(QGraphicsItem *parent)
    : ModelItem(parent)
{
    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    builder << QGLCube(CubeSize);
    cube = builder.finalizedSceneNode();
}

CubeItem::~CubeItem()
{
    delete cube;
}

void CubeItem::paintGL(QGLPainter *painter)
{
    GLuint textureId = this->textureId();
    if (textureId) {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        painter->setFaceColor(QGL::AllFaces, QColor(0, 0, 0, 200));
        painter->setStandardEffect(QGL::LitDecalTexture2D);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (painter->isFixedFunction())
            glEnable(GL_TEXTURE_2D);
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        cube->draw(painter);
        glCullFace(GL_BACK);
        cube->draw(painter);
        glDisable(GL_CULL_FACE);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (painter->isFixedFunction())
            glDisable(GL_TEXTURE_2D);
    } else {
        painter->setStandardEffect(QGL::LitMaterial);
        cube->draw(painter);
    }
}

static const int vertexDataLen = 6 * 4 * 3;

static const float vertexData[vertexDataLen] = {
    -0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,

    -0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,

    0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,
    0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,

    0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,
    0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,

    0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,

    0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,
    0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize
};

QPointF CubeItem::intersection(const QRay3D &ray, int *actualFace) const
{
    // Determine which face of the cube contains the point.
    QMatrix4x4 mv = camera()->modelViewMatrix();
    QVector3D pt1, pt2, pt3, pt4;
    QVector2D tc1, tc2, tc3;
    bool singleFace = (pressedFace() != -1);
    for (int face = 0; face < 6; ++face) {
        if (singleFace && face != pressedFace())
            continue;

        // Test the two triangles on the face for an intersection.
        pt1 = QVector3D(vertexData[face * 4 * 3],
                        vertexData[face * 4 * 3 + 1],
                        vertexData[face * 4 * 3 + 2]);
        pt2 = QVector3D(vertexData[face * 4 * 3 + 3],
                        vertexData[face * 4 * 3 + 4],
                        vertexData[face * 4 * 3 + 5]);
        pt3 = QVector3D(vertexData[face * 4 * 3 + 6],
                        vertexData[face * 4 * 3 + 7],
                        vertexData[face * 4 * 3 + 8]);
        pt4 = QVector3D(vertexData[face * 4 * 3 + 9],
                        vertexData[face * 4 * 3 + 10],
                        vertexData[face * 4 * 3 + 11]);
        pt1 = mv.map(pt1);
        pt2 = mv.map(pt2);
        pt3 = mv.map(pt3);
        pt4 = mv.map(pt4);
        QTriangle3D triangle(pt1, pt2, pt3);
        qreal t = triangle.intersection(ray);
        if (qIsNaN(t)) {
            triangle.setQ(pt3);
            triangle.setR(pt4);
            t = triangle.intersection(ray);
            if (qIsNaN(t)) {
                if (!singleFace)
                    continue;
                // The mouse probably moved outside the face while the
                // mouse button was held down.  Use the triangle's plane
                // to compute a virtual texture co-ordinate.
                t = triangle.plane().intersection(ray);
            }
            tc1 = QVector2D(1.0f, 0.0f);
            tc2 = QVector2D(0.0f, 1.0f);
            tc3 = QVector2D(0.0f, 0.0f);
        } else {
            tc1 = QVector2D(1.0f, 0.0f);
            tc2 = QVector2D(1.0f, 1.0f);
            tc3 = QVector2D(0.0f, 1.0f);
        }

        // We want the face that is pointing towards the user.
        QVector3D v = QVector3D::crossProduct(pt2 - pt1, pt3 - pt1);
        if (!singleFace && v.z() <= 0.0f)
            continue;

        // Get the texture co-ordinate corresponding to the intersection.
        QVector2D uv = triangle.uv(ray.point(t));
        QVector2D tc =
            uv.x() * tc1 + uv.y() * tc2 + (1 - uv.x() - uv.y()) * tc3;
        *actualFace = face;
        return QPointF(tc.x(), tc.y());
    }

    *actualFace = -1;
    return QPointF();
}
