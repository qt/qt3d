/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qtorusmesh.h"
#include "qbuffer.h"
#include "qattribute.h"
#include "qmeshdata.h"
#include <Qt3DRenderer/private/qabstractmesh_p.h>

#include <qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class TorusMeshFunctor : public QAbstractMeshFunctor
{
public:
    TorusMeshFunctor(int rings, int slices, float radius, float minorRadius);
    QMeshDataPtr operator ()() Q_DECL_OVERRIDE;
    bool operator ==(const QAbstractMeshFunctor &other) const Q_DECL_OVERRIDE;

private:
    int m_rings;
    int m_slices;
    float m_radius;
    float m_minorRadius;
};

class QTorusMeshPrivate : public QAbstractMeshPrivate
{
    QTorusMeshPrivate()
        : QAbstractMeshPrivate()
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

void QTorusMesh::copy(const QNode *ref)
{
    QAbstractMesh::copy(ref);
    const QTorusMesh *mesh = static_cast<const QTorusMesh*>(ref);
    d_func()->m_rings = mesh->d_func()->m_rings;
    d_func()->m_slices = mesh->d_func()->m_slices;
    d_func()->m_radius = mesh->d_func()->m_radius;
    d_func()->m_minorRadius = mesh->d_func()->m_minorRadius;
}


QTorusMesh::QTorusMesh(QNode *parent)
    : QAbstractMesh(*new QTorusMeshPrivate, parent)
{
    update();
}
void QTorusMesh::setRings(int rings)
{
    Q_D(QTorusMesh);
    if (rings != d->m_rings) {
        d->m_rings = rings;
        emit ringsChanged();
        QAbstractMesh::update();
    }
}

void QTorusMesh::setSlices(int slices)
{
    Q_D(QTorusMesh);
    if (slices != d->m_slices) {
        d->m_slices = slices;
        emit slicesChanged();
        QAbstractMesh::update();
    }
}

void QTorusMesh::setRadius(float radius)
{
    Q_D(QTorusMesh);
    if (radius != d->m_radius) {
        d->m_radius = radius;
        emit radiusChanged();
        QAbstractMesh::update();
    }
}

void QTorusMesh::setMinorRadius(float minorRadius)
{
    Q_D(QTorusMesh);
    if (minorRadius != d->m_minorRadius) {
        d->m_minorRadius = minorRadius;
        emit minorRadiusChanged();
        QAbstractMesh::update();
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

QMeshDataPtr createTorusMesh(double radius, double minorRadius,
                            int rings, int sides)
{
    QMeshDataPtr mesh(new QMeshData(QMeshData::Triangles));

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
        float cu = qCos( u );
        float su = qSin( u );

        for ( int side = 0; side < sides; side++ )
        {
            float v = side * sideFactor;
            float cv = qCos( v );
            float sv = qSin( v );
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

    mesh->addAttribute(QMeshData::defaultPositionAttributeName(), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, 0, stride)));
    quint32 offset = sizeof(float) * 3;

    mesh->addAttribute(QMeshData::defaultTextureCoordinateAttributeName(), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC2, nVerts, offset, stride)));
    offset += sizeof(float) * 2;

    mesh->addAttribute(QMeshData::defaultNormalAttributeName(), QAbstractAttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride)));
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

    mesh->computeBoundsFromAttribute(QMeshData::defaultPositionAttributeName());

    return mesh;
}

QAbstractMeshFunctorPtr QTorusMesh::meshFunctor() const
{
    Q_D(const QTorusMesh);
    return QAbstractMeshFunctorPtr(new TorusMeshFunctor(d->m_rings, d->m_slices, d->m_radius, d->m_minorRadius));
}

TorusMeshFunctor::TorusMeshFunctor(int rings, int slices, float radius, float minorRadius)
    : QAbstractMeshFunctor()
    , m_rings(rings)
    , m_slices(slices)
    , m_radius(radius)
    , m_minorRadius(minorRadius)
{
}

QMeshDataPtr TorusMeshFunctor::operator ()()
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
