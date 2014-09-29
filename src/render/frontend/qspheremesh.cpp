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

#include "qspheremesh.h"
#include "renderlogging.h"
#include "qabstractshapemesh_p.h"
#include "qbuffer.h"
#include "qattribute.h"

#include <cmath>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class SphereMeshFunctor : public QAbstractMeshFunctor
{
public:
    SphereMeshFunctor(int rings, int slices, float radius, bool generateTangents);
    QAbstractMeshDataPtr operator ()() Q_DECL_OVERRIDE;
    bool operator ==(const QAbstractMeshFunctor &other) const Q_DECL_OVERRIDE;

private:
    int m_rings;
    int m_slices;
    float m_radius;
    bool m_generateTangents;
};

class QSphereMeshPrivate : public QAbstractShapeMeshPrivate
{
    QSphereMeshPrivate(QSphereMesh *qq)
        : QAbstractShapeMeshPrivate(qq)
        , m_generateTangents(false)
        , m_rings(16)
        , m_slices(16)
        , m_radius(1.0)
    {}

    void copy(const QNodePrivate *ref) Q_DECL_OVERRIDE;

    Q_DECLARE_PUBLIC (QSphereMesh)
    bool m_generateTangents;
    int m_rings;
    int m_slices;
    float m_radius;
};

QSphereMesh::QSphereMesh(QNode *parent)
    : QAbstractShapeMesh(*new QSphereMeshPrivate(this), parent)
{
    setDirty(true);
}

void QSphereMeshPrivate::copy(const QNodePrivate *ref)
{
    QAbstractShapeMeshPrivate::copy(ref);
    const QSphereMeshPrivate *mesh = static_cast<const QSphereMeshPrivate *>(ref);
    m_generateTangents = mesh->m_generateTangents;
    m_rings = mesh->m_rings;
    m_slices = mesh->m_slices;
    m_radius = mesh->m_radius;
}

void QSphereMesh::setRings(int rings)
{
    Q_D(QSphereMesh);
    if (rings != d->m_rings) {
        d->m_rings = rings;
        emit ringsChanged();
        QAbstractMesh::setDirty(true);
    }
}

void QSphereMesh::setSlices(int slices)
{
    Q_D(QSphereMesh);
    if (slices != d->m_slices) {
        d->m_slices = slices;
        emit slicesChanged();
        QAbstractMesh::setDirty(true);
    }
}

void QSphereMesh::setRadius(float radius)
{
    Q_D(QSphereMesh);
    if (radius != d->m_radius) {
        d->m_radius = radius;
        emit radiusChanged();
        QAbstractMesh::setDirty(true);
    }
}

void QSphereMesh::setGenerateTangents(bool gen)
{
    Q_D(QSphereMesh);
    if (d->m_generateTangents != gen) {
        d->m_generateTangents = gen;
        emit generateTangentsChanged();
        QAbstractMesh::setDirty(true);
    }
}

bool QSphereMesh::generateTangents() const
{
    Q_D(const QSphereMesh);
    return d->m_generateTangents;
}

QAbstractMeshFunctorPtr QSphereMesh::meshFunctor() const
{
    Q_D(const QSphereMesh);
    return QAbstractMeshFunctorPtr(new SphereMeshFunctor(d->m_rings, d->m_slices, d->m_radius, d->m_generateTangents));
}

QSphereMesh *QSphereMesh::doClone() const
{
    QSphereMesh *clone = new QSphereMesh();
    clone->d_func()->copy(d_func());
    return clone;
}

int QSphereMesh::rings() const
{
    Q_D(const QSphereMesh);
    return d->m_rings;
}

int QSphereMesh::slices() const
{
    Q_D(const QSphereMesh);
    return d->m_slices;
}

float QSphereMesh::radius() const
{
    Q_D(const QSphereMesh);
    return d->m_radius;
}

QMeshDataPtr createSphereMesh(double radius, int rings, int slices, bool hasTangents)
{
    QMeshDataPtr mesh(new QMeshData(GL_TRIANGLES));

    int nVerts  = ( slices + 1 ) * ( rings + 1 ); // One extra line of latitude
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    quint32 elementSize = 3 + 2 + 3 + (hasTangents ? 4 : 0);
    quint32 stride = elementSize * sizeof(float);
    bufferBytes.resize(stride * nVerts);

    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float dTheta = (M_PI * 2) / static_cast<float>( slices );
    const float dPhi = M_PI / static_cast<float>( rings );
    const float du = 1.0f / static_cast<float>( slices );
    const float dv = 1.0f / static_cast<float>( rings );

    // Iterate over latitudes (rings)
    for ( int lat = 0; lat < rings + 1; ++lat )
    {
        const float phi = M_PI_2 - static_cast<float>( lat ) * dPhi;
        const float cosPhi = cosf( phi );
        const float sinPhi = sinf( phi );
        const float v = 1.0f - static_cast<float>( lat ) * dv;

        // Iterate over longitudes (slices)
        for ( int lon = 0; lon < slices + 1; ++lon )
        {
            const float theta = static_cast<float>( lon ) * dTheta;
            const float cosTheta = cosf( theta );
            const float sinTheta = sinf( theta );
            const float u = static_cast<float>( lon ) * du;

            *fptr++ = radius * cosTheta * cosPhi;
            *fptr++ = radius * sinPhi;
            *fptr++ = radius * sinTheta * cosPhi;

            *fptr++ = u;
            *fptr++ = v;

            *fptr++   = cosTheta * cosPhi;
            *fptr++ = sinPhi;
            *fptr++ = sinTheta * cosPhi;

            if (hasTangents) {
                *fptr++ = sinTheta;
                *fptr++ = 0.0;
                *fptr++ = -cosTheta;
                *fptr++ = 1.0;
            }
        }
    }

    BufferPtr buf(new Buffer(QOpenGLBuffer::VertexBuffer));
    buf->setUsage(QOpenGLBuffer::StaticDraw);
    buf->setData(bufferBytes);

    mesh->addAttribute(QAbstractMeshData::defaultPositionAttributeName(), AttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, 0, stride)));
    quint32 offset = sizeof(float) * 3;

    mesh->addAttribute(QAbstractMeshData::defaultTextureCoordinateAttributeName(), AttributePtr(new Attribute(buf, GL_FLOAT_VEC2, nVerts, offset, stride)));
    offset += sizeof(float) * 2;

    mesh->addAttribute(QAbstractMeshData::defaultNormalAttributeName(), AttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride)));
    offset += sizeof(float) * 3;

    if (hasTangents) {
        mesh->addAttribute(QAbstractMeshData::defaultTangentAttributeName(), AttributePtr(new Attribute(buf, GL_FLOAT_VEC4, nVerts, offset, stride)));
        offset += sizeof(float) * 4;
    }

    int faces = (slices * 2) * (rings - 1); // two tris per slice, for all middle rings
    faces += 2 * slices; // tri per slice for both top and bottom

    QByteArray indexBytes;
    int indices = faces * 3;
    Q_ASSERT(indices < 65536);
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    // top cap
    {
        const int nextRingStartIndex = slices + 1;
        for ( int j = 0; j < slices; ++j )
        {
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = 0;
            *indexPtr++ = nextRingStartIndex + j + 1;
        }
    }

    for ( int i = 1; i < (rings - 1); ++i )
    {
        const int ringStartIndex = i * ( slices + 1 );
        const int nextRingStartIndex = ( i + 1 ) * ( slices + 1 );

        for ( int j = 0; j < slices; ++j )
        {
            // Split the quad into two triangles
            *indexPtr++ = ringStartIndex + j;
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex + j + 1;
        }
    }

    // bottom cap
    {
        const int ringStartIndex = (rings - 1) * ( slices + 1);
        const int nextRingStartIndex = (rings) * ( slices + 1);
        for ( int j = 0; j < slices; ++j )
        {
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex;
            *indexPtr++ = ringStartIndex + j;
        }
    }

    BufferPtr indexBuffer(new Buffer(QOpenGLBuffer::IndexBuffer));
    indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    indexBuffer->setData(indexBytes);
    mesh->setIndexAttribute(AttributePtr(new Attribute(indexBuffer, GL_UNSIGNED_SHORT, indices, 0, 0)));

    mesh->computeBoundsFromAttribute(QAbstractMeshData::defaultPositionAttributeName());
    qCDebug(Render::Frontend) << "computed sphere bounds is:" << mesh->boundingBox();

    return mesh;
}

SphereMeshFunctor::SphereMeshFunctor(int rings, int slices, float radius, bool generateTangents)
    : QAbstractMeshFunctor()
    , m_rings(rings)
    , m_slices(slices)
    , m_radius(radius)
    , m_generateTangents(generateTangents)
{
}

QAbstractMeshDataPtr SphereMeshFunctor::operator ()()
{
    return createSphereMesh(m_radius, m_rings, m_slices, m_generateTangents);
}

bool SphereMeshFunctor::operator ==(const QAbstractMeshFunctor &other) const
{
    const SphereMeshFunctor *otherFunctor = dynamic_cast<const SphereMeshFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
        return (otherFunctor->m_rings == m_rings &&
                otherFunctor->m_slices == m_slices &&
                otherFunctor->m_radius == m_radius &&
                otherFunctor->m_generateTangents == m_generateTangents);
    return false;
}

} //Qt3D

QT_END_NAMESPACE
