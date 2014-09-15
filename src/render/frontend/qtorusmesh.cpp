/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qtorusmesh.h"
#include "qabstractshapemesh_p.h"
#include "qbuffer.h"
#include "qattribute.h"

#include <cmath>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class TorusMeshFunctor : public QAbstractMeshFunctor
{
public:
    TorusMeshFunctor(int rings, int slices, float radius, float minorRadius);
    QAbstractMeshDataPtr operator ()() Q_DECL_OVERRIDE;
    bool operator ==(const QAbstractMeshFunctor &other) const Q_DECL_OVERRIDE;

private:
    int m_rings;
    int m_slices;
    float m_radius;
    float m_minorRadius;
};

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

QTorusMesh::QTorusMesh(QNode *parent)
    : QAbstractShapeMesh(*new QTorusMeshPrivate(this), parent)
{
    setDirty(true);
}

void QTorusMesh::copy(const QNode *ref)
{
    Q_D(QTorusMesh);
    QAbstractShapeMesh::copy(ref);
    const QTorusMesh *mesh = qobject_cast<const QTorusMesh *>(ref);
    if (mesh != Q_NULLPTR) {
        d->m_rings = mesh->rings();
        d->m_slices = mesh->slices();
        d->m_radius = mesh->radius();
        d->m_minorRadius = mesh->minorRadius();
    }
}

void QTorusMesh::setRings(int rings)
{
    Q_D(QTorusMesh);
    if (rings != d->m_rings) {
        d->m_rings = rings;
        emit ringsChanged();
        QAbstractMesh::setDirty(true);
    }
}

void QTorusMesh::setSlices(int slices)
{
    Q_D(QTorusMesh);
    if (slices != d->m_slices) {
        d->m_slices = slices;
        emit slicesChanged();
        QAbstractMesh::setDirty(true);
    }
}

void QTorusMesh::setRadius(float radius)
{
    Q_D(QTorusMesh);
    if (radius != d->m_radius) {
        d->m_radius = radius;
        emit radiusChanged();
        QAbstractMesh::setDirty(true);
    }
}

void QTorusMesh::setMinorRadius(float minorRadius)
{
    Q_D(QTorusMesh);
    if (minorRadius != d->m_minorRadius) {
        d->m_minorRadius = minorRadius;
        emit minorRadiusChanged();
        QAbstractMesh::setDirty(true);
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

QAbstractMeshDataPtr createTorusMesh(double radius, double minorRadius,
                            int rings, int sides)
{
    QAbstractMeshDataPtr mesh(new QMeshData(GL_TRIANGLES));

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

    mesh->addAttribute(QAbstractMeshData::defaultPositionAttributeName(), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, 0, stride)));
    quint32 offset = sizeof(float) * 3;

    mesh->addAttribute(QAbstractMeshData::defaultTextureCoordinateAttributeName(), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC2, nVerts, offset, stride)));
    offset += sizeof(float) * 2;

    mesh->addAttribute(QAbstractMeshData::defaultNormalAttributeName(), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride)));
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

    mesh->computeBoundsFromAttribute(QAbstractMeshData::defaultPositionAttributeName());

    return mesh;
}

QAbstractMeshFunctorPtr QTorusMesh::meshFunctor() const
{
    Q_D(const QTorusMesh);
    return QAbstractMeshFunctorPtr(new TorusMeshFunctor(d->m_rings, d->m_slices, d->m_radius, d->m_minorRadius));
}

QTorusMesh *QTorusMesh::doClone(QNode *clonedParent) const
{
    return new QTorusMesh(clonedParent);
}

TorusMeshFunctor::TorusMeshFunctor(int rings, int slices, float radius, float minorRadius)
    : QAbstractMeshFunctor()
    , m_rings(rings)
    , m_slices(slices)
    , m_radius(radius)
    , m_minorRadius(minorRadius)
{
}

QAbstractMeshDataPtr TorusMeshFunctor::operator ()()
{
    return createTorusMesh(m_radius, m_minorRadius, m_rings, m_slices);
}

bool TorusMeshFunctor::operator ==(const QAbstractMeshFunctor &other) const
{
    const TorusMeshFunctor *otherFunctor = dynamic_cast<const TorusMeshFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
        return (otherFunctor->m_radius == m_radius &&
                otherFunctor->m_slices == m_slices &&
                otherFunctor->m_rings == m_rings &&
                otherFunctor->m_minorRadius == m_minorRadius);
    return false;
}

} // Qt3D

QT_END_NAMESPACE
