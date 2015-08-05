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
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qbufferfunctor.h>
#include <Qt3DRenderer/qattribute.h>
#include <QVector3D>
#include <qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace {

QByteArray createTorusVertexData(double radius, double minorRadius,
                                 int rings, int sides)
{
    const int nVerts  = sides * (rings + 1);
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal
    const quint32 elementSize = 3 + 2 + 3;
    const quint32 stride = elementSize * sizeof(float);
    bufferBytes.resize(stride * nVerts);

    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float ringFactor = (M_PI * 2) / static_cast<float>( rings );
    const float sideFactor = (M_PI * 2) / static_cast<float>( sides );

    for (int ring = 0; ring <= rings; ++ring) {
        const float u = ring * ringFactor;
        const float cu = qCos( u );
        const float su = qSin( u );

        for (int side = 0; side < sides; ++side) {
            const float v = side * sideFactor;
            const float cv = qCos( v );
            const float sv = qSin( v );
            const float r = (radius + minorRadius * cv);

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

    return bufferBytes;
}

QByteArray createTorusIndexData(int rings, int sides)
{
    QByteArray indexBytes;
    int faces = (sides * 2) * rings; // two tris per side, for all rings
    int indices = faces * 3;
    Q_ASSERT(indices < 65536);
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    for (int ring = 0; ring < rings; ++ring) {
        const int ringStart = ring * sides;
        const int nextRingStart = (ring + 1) * sides;
        for (int side = 0; side < sides; ++side) {
            const int nextSide = (side + 1) % sides;
            *indexPtr++ = (ringStart + side);
            *indexPtr++ = (nextRingStart + side);
            *indexPtr++ = (nextRingStart + nextSide);
            *indexPtr++ = ringStart + side;
            *indexPtr++ = nextRingStart + nextSide;
            *indexPtr++ = (ringStart + nextSide);
        }
    }

    return indexBytes;
}

} // anonymous

class TorusVertexDataFunctor : public QBufferFunctor
{
public:
    TorusVertexDataFunctor(int rings, int slices, float radius, float minorRadius)
        : m_rings(rings)
        , m_sides(slices)
        , m_radius(radius)
        , m_minorRadius(minorRadius)
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        return createTorusVertexData(m_radius, m_minorRadius, m_rings, m_sides);
    }

    bool operator ==(const QBufferFunctor &other) const Q_DECL_OVERRIDE
    {
        const TorusVertexDataFunctor *otherFunctor = functor_cast<TorusVertexDataFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_rings == m_rings &&
                    otherFunctor->m_sides == m_sides &&
                    otherFunctor->m_radius == m_radius &&
                    otherFunctor->m_minorRadius == m_minorRadius);
        return false;
    }

    QT3D_FUNCTOR(TorusVertexDataFunctor)

private:
    int m_rings;
    int m_sides;
    float m_radius;
    float m_minorRadius;
};

class TorusIndexDataFunctor : public QBufferFunctor
{
public:
    TorusIndexDataFunctor(int rings, int slices)
        : m_rings(rings)
        , m_sides(slices)
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        return createTorusIndexData(m_rings, m_sides);
    }

    bool operator ==(const QBufferFunctor &other) const Q_DECL_OVERRIDE
    {
        const TorusIndexDataFunctor *otherFunctor = functor_cast<TorusIndexDataFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_rings == m_rings &&
                    otherFunctor->m_sides == m_sides);
        return false;
    }

    QT3D_FUNCTOR(TorusIndexDataFunctor)

private:
    int m_rings;
    int m_sides;
};

class TorusGeometry : public QGeometry
{
    Q_OBJECT
public:
    explicit TorusGeometry(QNode *parent = 0)
        : QGeometry(parent)
        , m_rings(16)
        , m_slices(16)
        , m_radius(1.0f)
        , m_minorRadius(1.0f)
        , m_positionAttribute(new QAttribute(this))
        , m_normalAttribute(new QAttribute(this))
        , m_texCoordAttribute(new QAttribute(this))
        , m_indexAttribute(new QAttribute(this))
        , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
        , m_indexBuffer(new QBuffer(QBuffer::IndexBuffer, this))
    {
        // vec3 pos, vec2 tex, vec3 normal
        const quint32 elementSize = 3 + 2 + 3;
        const quint32 stride = elementSize * sizeof(float);
        const int nVerts = (m_slices + 1) * (m_rings + 1);
        const int faces = (m_slices * 2) * m_rings;

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

        m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
        m_indexAttribute->setDataType(QAttribute::UnsignedShort);
        m_indexAttribute->setBuffer(m_indexBuffer);

        m_indexAttribute->setCount(faces * 3);

        m_vertexBuffer->setBufferFunctor(QBufferFunctorPtr(new TorusVertexDataFunctor(m_rings, m_slices, m_radius, m_minorRadius)));
        m_indexBuffer->setBufferFunctor(QBufferFunctorPtr(new TorusIndexDataFunctor(m_rings, m_slices)));

        addAttribute(m_positionAttribute);
        addAttribute(m_texCoordAttribute);
        addAttribute(m_normalAttribute);
        addAttribute(m_indexAttribute);
    }

    ~TorusGeometry()
    {
        QGeometry::cleanup();
    }

    void updateVertices()
    {
        const int nVerts = (m_slices + 1) * (m_rings + 1);
        m_positionAttribute->setCount(nVerts);
        m_texCoordAttribute->setCount(nVerts);
        m_normalAttribute->setCount(nVerts);

        m_vertexBuffer->setBufferFunctor(QBufferFunctorPtr(new TorusVertexDataFunctor(m_rings, m_slices, m_radius, m_minorRadius)));
    }

    void updateIndices()
    {
        const int faces = (m_slices * 2) * m_rings;
        m_indexAttribute->setCount(faces * 3);
        m_indexBuffer->setBufferFunctor(QBufferFunctorPtr(new TorusIndexDataFunctor(m_rings, m_slices)));
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

    void setMinorRadius(float minorRadius)
    {
        if (minorRadius != m_minorRadius) {
            m_minorRadius = minorRadius;
            updateVertices();
            emit minorRadiusChanged();
        }
    }

    int rings() const { return m_rings; }
    int slices() const { return m_slices; }
    float radius() const { return m_radius; }
    float minorRadius() const { return m_minorRadius; }

Q_SIGNALS:
    void radiusChanged();
    void ringsChanged();
    void slicesChanged();
    void minorRadiusChanged();

private:
    int m_rings;
    int m_slices;
    float m_radius;
    float m_minorRadius;
    QAttribute *m_positionAttribute;
    QAttribute *m_normalAttribute;
    QAttribute *m_texCoordAttribute;
    QAttribute *m_indexAttribute;
    QBuffer *m_vertexBuffer;
    QBuffer *m_indexBuffer;
};


QTorusMesh::QTorusMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    TorusGeometry *geometry = new TorusGeometry(this);
    QObject::connect(geometry, &TorusGeometry::radiusChanged, this, &QTorusMesh::radiusChanged);
    QObject::connect(geometry, &TorusGeometry::ringsChanged, this, &QTorusMesh::ringsChanged);
    QObject::connect(geometry, &TorusGeometry::slicesChanged, this, &QTorusMesh::slicesChanged);
    QObject::connect(geometry, &TorusGeometry::minorRadiusChanged, this, &QTorusMesh::minorRadiusChanged);

    QGeometryRenderer::setGeometry(geometry);
}

QTorusMesh::~QTorusMesh()
{
    QGeometryRenderer::cleanup();
}

void QTorusMesh::setRings(int rings)
{
    static_cast<TorusGeometry *>(geometry())->setRings(rings);
}

void QTorusMesh::setSlices(int slices)
{
    static_cast<TorusGeometry *>(geometry())->setSlices(slices);
}

void QTorusMesh::setRadius(float radius)
{
    static_cast<TorusGeometry *>(geometry())->setRadius(radius);
}

void QTorusMesh::setMinorRadius(float minorRadius)
{
    static_cast<TorusGeometry *>(geometry())->setMinorRadius(minorRadius);
}

int QTorusMesh::rings() const
{
    return static_cast<TorusGeometry *>(geometry())->rings();
}

int QTorusMesh::slices() const
{
    return static_cast<TorusGeometry *>(geometry())->slices();
}

float QTorusMesh::radius() const
{
    return static_cast<TorusGeometry *>(geometry())->radius();
}

float QTorusMesh::minorRadius() const
{
    return static_cast<TorusGeometry *>(geometry())->minorRadius();
}

} // Qt3D

QT_END_NAMESPACE

#include "qtorusmesh.moc"
