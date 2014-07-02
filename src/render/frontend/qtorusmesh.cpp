/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#define _USE_MATH_DEFINES // For MSVC
#include "qtorusmesh.h"
#include "qabstractshapemesh_p.h"
#include "qbuffer.h"
#include "qattribute.h"

#include <cmath>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTorusMeshPrivate : public QAbstractShapeMeshPrivate
{
    QTorusMeshPrivate(QTorusMesh *qq)
        : QAbstractShapeMeshPrivate(qq)
        , m_rings(16)
        , m_slices(16)
        , m_radius(1.0)
        , m_minorRadius(1.0)
    {}

    Q_DECLARE_PUBLIC(QTorusMesh)
    int m_rings;
    int m_slices;
    float m_radius;
    float m_minorRadius;
};

QTorusMesh::QTorusMesh(Node *parent)
    : QAbstractShapeMesh(*new QTorusMeshPrivate(this), parent)
{
}

void QTorusMesh::setRings(int rings)
{
    Q_D(QTorusMesh);
    if (rings != d->m_rings) {
        d->m_rings = rings;
        emit ringsChanged();
    }
}

void QTorusMesh::setSlices(int slices)
{
    Q_D(QTorusMesh);
    if (slices != d->m_slices) {
        d->m_slices = slices;
        emit slicesChanged();
    }
}

void QTorusMesh::setRadius(float radius)
{
    Q_D(QTorusMesh);
    if (radius != d->m_radius) {
        d->m_radius = radius;
        emit radiusChanged();
    }
}

void QTorusMesh::setMinorRadius(float minorRadius)
{
    Q_D(QTorusMesh);
    if (minorRadius != d->m_minorRadius) {
        d->m_minorRadius = minorRadius;
        emit minorRadiusChanged();
    }
}

int QTorusMesh::rings() const
{
    Q_D(const QTorusMesh);
    return d->m_rings;
}

int QTorusMesh::slices() const
{
    Q_D(const QTorusMesh);
    return d->m_slices;
}

float QTorusMesh::radius() const
{
    Q_D(const QTorusMesh);
    return d->m_radius;
}

float QTorusMesh::minorRadius() const
{
    Q_D(const QTorusMesh);
    return d->m_minorRadius;
}

QAbstractMeshDataPtr QTorusMesh::buildMeshdata() const
{
    Q_D(const QTorusMesh);
    return createTorusMesh(d->m_radius, d->m_minorRadius, d->m_rings, d->m_slices);
}

QAbstractMeshDataPtr QTorusMesh::createTorusMesh(double radius, double minorRadius,
                            int rings, int sides)
{
    QAbstractMeshDataPtr mesh(new MeshData(GL_TRIANGLES));

    int nVerts  = sides * ( rings + 1 );
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal
    quint32 elementSize = 3 + 2 + 3;
    quint32 stride = elementSize * sizeof(float);
    bufferBytes.resize(stride * nVerts);

    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    float ringFactor = (M_PI * 2) / static_cast<float>( rings );
    float sideFactor = (M_PI * 2) / static_cast<float>( sides );

    for ( int ring = 0; ring <= rings; ring++ )
    {
        float u = ring * ringFactor;
        float cu = cos( u );
        float su = sin( u );

        for ( int side = 0; side < sides; side++ )
        {
            float v = side * sideFactor;
            float cv = cos( v );
            float sv = sin( v );
            float r = ( radius + minorRadius * cv );

            *fptr++ = r * cu;
            *fptr++ = r * su;
            *fptr++ = minorRadius * sv;


            *fptr++ = u / (M_PI * 2);
            *fptr++ = v / (M_PI * 2);

            QVector3D n(cv * cu * r, cv * su * r, sv * r);
            n.normalize();
            *fptr++ = n.x();
            *fptr++ = n.y();
            *fptr++ = n.z();
        }
    }

    BufferPtr buf(new Buffer(QOpenGLBuffer::VertexBuffer));
    buf->setUsage(QOpenGLBuffer::StaticDraw);
    buf->setData(bufferBytes);

    mesh->addAttribute(QStringLiteral("position"), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, 0, stride)));
    quint32 offset = sizeof(float) * 3;

    mesh->addAttribute(QStringLiteral("texcoord"), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC2, nVerts, offset, stride)));
    offset += sizeof(float) * 2;

    mesh->addAttribute(QStringLiteral("normal"), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride)));
    offset += sizeof(float) * 3;

    QByteArray indexBytes;
    int faces = (sides * 2) * rings; // two tris per side, for all rings
    int indices = faces * 3;
    Q_ASSERT(indices < 65536);
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    for ( int ring = 0; ring < rings; ring++ )
    {
        int ringStart = ring * sides;
        int nextRingStart = ( ring + 1 ) * sides;
        for ( int side = 0; side < sides; side++ )
        {
            int nextSide = ( side + 1 ) % sides;
            *indexPtr++ = ( ringStart + side );
            *indexPtr++ = ( nextRingStart + side );
            *indexPtr++ = ( nextRingStart + nextSide );
            *indexPtr++ = ringStart + side;
            *indexPtr++ = nextRingStart + nextSide;
            *indexPtr++ = ( ringStart + nextSide );
        }
    }

    BufferPtr indexBuffer(new Buffer(QOpenGLBuffer::IndexBuffer));
    indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    indexBuffer->setData(indexBytes);
    mesh->setIndexAttribute(AttributePtr(new Attribute(indexBuffer, GL_UNSIGNED_SHORT, indices, 0, 0)));

    mesh->computeBoundsFromAttribute(QStringLiteral("position"));

    return mesh;
}

} // Qt3D

QT_END_NAMESPACE
