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

#include "qspheremesh.h"
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/qbufferfunctor.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qattribute.h>

#include <qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace {

QByteArray createSphereMeshVertexData(float radius, int rings, int slices)
{
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    const int nVerts  = (slices + 1) * (rings + 1);
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
        const float cosPhi = qCos( phi );
        const float sinPhi = qSin( phi );
        const float v = 1.0f - static_cast<float>( lat ) * dv;

        // Iterate over longitudes (slices)
        for ( int lon = 0; lon < slices + 1; ++lon )
        {
            const float theta = static_cast<float>( lon ) * dTheta;
            const float cosTheta = qCos( theta );
            const float sinTheta = qSin( theta );
            const float u = static_cast<float>( lon ) * du;

            *fptr++ = radius * cosTheta * cosPhi;
            *fptr++ = radius * sinPhi;
            *fptr++ = radius * sinTheta * cosPhi;

            *fptr++ = u;
            *fptr++ = v;

            *fptr++   = cosTheta * cosPhi;
            *fptr++ = sinPhi;
            *fptr++ = sinTheta * cosPhi;

            *fptr++ = sinTheta;
            *fptr++ = 0.0;
            *fptr++ = -cosTheta;
            *fptr++ = 1.0;
        }
    }
    return bufferBytes;
}

QByteArray createSphereMeshIndexData(int rings, int slices)
{
    int faces = (slices * 2) * (rings - 2); // two tris per slice, for all middle rings
    faces += 2 * slices; // tri per slice for both top and bottom

    QByteArray indexBytes;
    const int indices = faces * 3;
    Q_ASSERT(indices < 65536);
    indexBytes.resize(indices * sizeof(quint16));
    quint16 *indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

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

    return indexBytes;
}

} // anonymous

class SphereVertexDataFunctor : public QBufferFunctor
{
public:
    SphereVertexDataFunctor(int rings, int slices, float radius)
        : m_rings(rings)
        , m_slices(slices)
        , m_radius(radius)
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        return createSphereMeshVertexData(m_radius, m_rings, m_slices);
    }

    bool operator ==(const QBufferFunctor &other) const Q_DECL_OVERRIDE
    {
        const SphereVertexDataFunctor *otherFunctor = functor_cast<SphereVertexDataFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_rings == m_rings &&
                    otherFunctor->m_slices == m_slices &&
                    otherFunctor->m_radius == m_radius);
        return false;
    }

    QT3D_FUNCTOR(SphereVertexDataFunctor)

private:
    int m_rings;
    int m_slices;
    float m_radius;
};

class SphereIndexDataFunctor : public QBufferFunctor
{
public:
    SphereIndexDataFunctor(int rings, int slices)
        : m_rings(rings)
        , m_slices(slices)
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        return createSphereMeshIndexData(m_rings, m_slices);
    }

    bool operator ==(const QBufferFunctor &other) const Q_DECL_OVERRIDE
    {
        const SphereIndexDataFunctor *otherFunctor = functor_cast<SphereIndexDataFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_rings == m_rings &&
                    otherFunctor->m_slices == m_slices);
        return false;
    }

    QT3D_FUNCTOR(SphereIndexDataFunctor)

private:
    int m_rings;
    int m_slices;
};

class SphereGeometry : public QGeometry
{
    Q_OBJECT
public:
    explicit SphereGeometry(QNode *parent = 0)
        : QGeometry(parent)
        , m_generateTangents(false)
        , m_rings(16)
        , m_slices(16)
        , m_radius(1.0f)
        , m_positionAttribute(new QAttribute(this))
        , m_normalAttribute(new QAttribute(this))
        , m_texCoordAttribute(new QAttribute(this))
        , m_tangentAttribute(new QAttribute(this))
        , m_indexAttribute(new QAttribute(this))
        , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
        , m_indexBuffer(new QBuffer(QBuffer::IndexBuffer, this))
    {
        // vec3 pos, vec2 tex, vec3 normal, vec4 tangent
        const quint32 elementSize = 3 + 2 + 3 + 4;
        const quint32 stride = elementSize * sizeof(float);
        const int nVerts = (m_slices + 1) * (m_rings + 1);
        const int faces = (m_slices * 2) * (m_rings - 2) + (2 * m_slices);

        m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
        m_positionAttribute->setDataType(QAttribute::Float);
        m_positionAttribute->setDataSize(3);
        m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
        m_positionAttribute->setBuffer(m_vertexBuffer);
        m_positionAttribute->setByteStride(stride);
        m_positionAttribute->setCount(nVerts);

        m_texCoordAttribute->setName(QAttribute::defaultTextureCoordinateAttributeName());
        m_texCoordAttribute->setDataType(QAttribute::Float);
        m_texCoordAttribute->setDataSize(2);
        m_texCoordAttribute->setAttributeType(QAttribute::VertexAttribute);
        m_texCoordAttribute->setBuffer(m_vertexBuffer);
        m_texCoordAttribute->setByteStride(stride);
        m_texCoordAttribute->setByteOffset(3 * sizeof(float));
        m_texCoordAttribute->setCount(nVerts);

        m_normalAttribute->setName(QAttribute::defaultNormalAttributeName());
        m_normalAttribute->setDataType(QAttribute::Float);
        m_normalAttribute->setDataSize(3);
        m_normalAttribute->setAttributeType(QAttribute::VertexAttribute);
        m_normalAttribute->setBuffer(m_vertexBuffer);
        m_normalAttribute->setByteStride(stride);
        m_normalAttribute->setByteOffset(5 * sizeof(float));
        m_normalAttribute->setCount(nVerts);

        m_tangentAttribute->setName(QAttribute::defaultTangentAttributeName());
        m_tangentAttribute->setDataType(QAttribute::Float);
        m_tangentAttribute->setDataSize(4);
        m_tangentAttribute->setAttributeType(QAttribute::VertexAttribute);
        m_tangentAttribute->setBuffer(m_vertexBuffer);
        m_tangentAttribute->setByteStride(stride);
        m_tangentAttribute->setByteOffset(8 * sizeof(float));
        m_tangentAttribute->setCount(nVerts);

        m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
        m_indexAttribute->setDataType(QAttribute::UnsignedShort);
        m_indexAttribute->setBuffer(m_indexBuffer);

        m_indexAttribute->setCount(faces * 3);

        m_vertexBuffer->setBufferFunctor(QBufferFunctorPtr(new SphereVertexDataFunctor(m_rings, m_slices, m_radius)));
        m_indexBuffer->setBufferFunctor(QBufferFunctorPtr(new SphereIndexDataFunctor(m_rings, m_slices)));

        addAttribute(m_positionAttribute);
        addAttribute(m_texCoordAttribute);
        addAttribute(m_normalAttribute);
        if (m_generateTangents)
            addAttribute(m_tangentAttribute);
        addAttribute(m_indexAttribute);
    }

    ~SphereGeometry()
    {
        QGeometry::cleanup();
    }

    void updateVertices()
    {
        const int nVerts = (m_slices + 1) * (m_rings + 1);
        m_positionAttribute->setCount(nVerts);
        m_texCoordAttribute->setCount(nVerts);
        m_normalAttribute->setCount(nVerts);
        m_tangentAttribute->setCount(nVerts);

        m_vertexBuffer->setBufferFunctor(QBufferFunctorPtr(new SphereVertexDataFunctor(m_rings, m_slices, m_radius)));
    }

    void updateIndices()
    {
        const int faces = (m_slices * 2) * (m_rings - 2) + (2 * m_slices);
        m_indexAttribute->setCount(faces * 3);
        m_indexBuffer->setBufferFunctor(QBufferFunctorPtr(new SphereIndexDataFunctor(m_rings, m_slices)));
    }

    void setRings(int rings)
    {
        if (rings != m_rings) {
            m_rings = rings;
            updateVertices();
            updateIndices();
            emit ringsChanged();
        }
    }

    void setSlices(int slices)
    {
        if (slices != m_slices) {
            m_slices = slices;
            updateVertices();
            updateIndices();
            emit slicesChanged();
        }
    }

    void setRadius(float radius)
    {
        if (radius != m_radius) {
            m_radius = radius;
            updateVertices();
            emit radiusChanged();
        }
    }

    void setGenerateTangents(bool gen)
    {
        if (m_generateTangents != gen) {
            if (m_generateTangents)
                removeAttribute(m_tangentAttribute);
            m_generateTangents = gen;
            if (m_generateTangents)
                addAttribute(m_tangentAttribute);
            emit generateTangentsChanged();
        }
    }

    bool generateTangents() const { return m_generateTangents; }
    int rings() const { return m_rings; }
    int slices() const { return m_slices; }
    float radius() const { return m_radius; }

Q_SIGNALS:
    void radiusChanged();
    void ringsChanged();
    void slicesChanged();
    void generateTangentsChanged();

private:
    bool m_generateTangents;
    int m_rings;
    int m_slices;
    float m_radius;
    QAttribute *m_positionAttribute;
    QAttribute *m_normalAttribute;
    QAttribute *m_texCoordAttribute;
    QAttribute *m_tangentAttribute;
    QAttribute *m_indexAttribute;
    QBuffer *m_vertexBuffer;
    QBuffer *m_indexBuffer;
};

QSphereMesh::QSphereMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    SphereGeometry *geometry = new SphereGeometry(this);
    QObject::connect(geometry, &SphereGeometry::radiusChanged, this, &QSphereMesh::radiusChanged);
    QObject::connect(geometry, &SphereGeometry::ringsChanged, this, &QSphereMesh::ringsChanged);
    QObject::connect(geometry, &SphereGeometry::slicesChanged, this, &QSphereMesh::slicesChanged);
    QObject::connect(geometry, &SphereGeometry::generateTangentsChanged, this, &QSphereMesh::generateTangentsChanged);
    QGeometryRenderer::setGeometry(geometry);
}

QSphereMesh::~QSphereMesh()
{
    QNode::cleanup();
}

void QSphereMesh::setRings(int rings)
{
    static_cast<SphereGeometry *>(geometry())->setRings(rings);
}

void QSphereMesh::setSlices(int slices)
{
    static_cast<SphereGeometry *>(geometry())->setSlices(slices);
}

void QSphereMesh::setRadius(float radius)
{
    static_cast<SphereGeometry *>(geometry())->setRadius(radius);
}

void QSphereMesh::setGenerateTangents(bool gen)
{
    static_cast<SphereGeometry *>(geometry())->setGenerateTangents(gen);
}

bool QSphereMesh::generateTangents() const
{
    return static_cast<SphereGeometry *>(geometry())->generateTangents();
}

int QSphereMesh::rings() const
{
    return static_cast<SphereGeometry *>(geometry())->rings();
}

int QSphereMesh::slices() const
{
    return static_cast<SphereGeometry *>(geometry())->slices();
}

float QSphereMesh::radius() const
{
    return static_cast<SphereGeometry *>(geometry())->radius();
}

} //Qt3D

QT_END_NAMESPACE

#include "qspheremesh.moc"
