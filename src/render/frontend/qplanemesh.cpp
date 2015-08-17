/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qplanemesh.h"

#include <Qt3DRenderer/qattribute.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qbufferfunctor.h>
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/qgeometry.h>
#include <limits>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace {

QByteArray createPlaneVertexData(float w, float h, const QSize &resolution)
{
    Q_ASSERT(w > 0.0f);
    Q_ASSERT(h > 0.0f);
    Q_ASSERT(resolution.width() >= 2);
    Q_ASSERT(resolution.height() >= 2);

    const int nVerts = resolution.width() * resolution.height();

    // Populate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray bufferBytes;
    bufferBytes.resize(stride * nVerts);
    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float x0 = -w / 2.0f;
    const float z0 = -h / 2.0f;
    const float dx = w / (resolution.width() - 1);
    const float dz = h / (resolution.height() - 1);
    const float du = 1.0 / (resolution.width() - 1);
    const float dv = 1.0 / (resolution.height() - 1);

    // Iterate over z
    for (int j = 0; j < resolution.height(); ++j) {
        const float z = z0 + static_cast<float>(j) * dz;
        const float v = static_cast<float>(j) * dv;

        // Iterate over x
        for (int i = 0; i < resolution.width(); ++i) {
            const float x = x0 + static_cast<float>(i) * dx;
            const float u = static_cast<float>(i) * du;

            // position
            *fptr++ = x;
            *fptr++ = 0.0;
            *fptr++ = z;

            // texture coordinates
            *fptr++ = u;
            *fptr++ = v;

            // normal
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;

            // tangent
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
        }
    }

    return bufferBytes;
}

QByteArray createPlaneIndexData(const QSize &resolution)
{
    // Create the index data. 2 triangles per rectangular face
    const int faces = 2 * (resolution.width() - 1) * (resolution.height() - 1);
    const int indices = 3 * faces;
    Q_ASSERT(indices < std::numeric_limits<quint16>::max());
    QByteArray indexBytes;
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    // Iterate over z
    for (int j = 0; j < resolution.height() - 1; ++j) {
        const int rowStartIndex = j * resolution.width();
        const int nextRowStartIndex = (j + 1) * resolution.width();

        // Iterate over x
        for (int i = 0; i < resolution.width() - 1; ++i) {
            // Split quad into two triangles
            *indexPtr++ = rowStartIndex + i;
            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = rowStartIndex + i + 1;

            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = nextRowStartIndex + i + 1;
            *indexPtr++ = rowStartIndex + i + 1;
        }
    }

    return indexBytes;
}

} // anonymous

class PlaneVertexBufferFunctor : public QBufferFunctor
{
public:
    explicit PlaneVertexBufferFunctor(float w, float h, const QSize &resolution)
        : m_width(w)
        , m_height(h)
        , m_resolution(resolution)
    {}

    ~PlaneVertexBufferFunctor() {}

    QByteArray operator()() Q_DECL_FINAL
    {
        return createPlaneVertexData(m_width, m_height, m_resolution);
    }

    bool operator ==(const QBufferFunctor &other) const Q_DECL_FINAL
    {
        const PlaneVertexBufferFunctor *otherFunctor = functor_cast<PlaneVertexBufferFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_width == m_width &&
                    otherFunctor->m_height == m_height &&
                    otherFunctor->m_resolution == m_resolution);
        return false;
    }

    QT3D_FUNCTOR(PlaneVertexBufferFunctor)

private:
    float m_width;
    float m_height;
    QSize m_resolution;
};

class PlaneIndexBufferFunctor : public QBufferFunctor
{
public:
    explicit PlaneIndexBufferFunctor(const QSize &resolution)
        : m_resolution(resolution)
    {}

    ~PlaneIndexBufferFunctor() {}

    QByteArray operator()() Q_DECL_FINAL
    {
        return createPlaneIndexData(m_resolution);
    }

    bool operator ==(const QBufferFunctor &other) const Q_DECL_FINAL
    {
        const PlaneIndexBufferFunctor *otherFunctor = functor_cast<PlaneIndexBufferFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_resolution == m_resolution);
        return false;
    }

    QT3D_FUNCTOR(PlaneIndexBufferFunctor)

private:
    QSize m_resolution;
};

class PlaneGeometry : public QGeometry
{
    Q_OBJECT
public:
    explicit PlaneGeometry(QNode *parent = 0)
        : QGeometry(parent)
        , m_width(1.0f)
        , m_height(1.0f)
        , m_meshResolution(QSize(2, 2))
        , m_positionAttribute(new QAttribute(this))
        , m_normalAttribute(new QAttribute(this))
        , m_texCoordAttribute(new QAttribute(this))
        , m_tangentAttribute(new QAttribute(this))
        , m_indexAttribute(new QAttribute(this))
        , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
        , m_indexBuffer(new QBuffer(QBuffer::IndexBuffer, this))
    {
        const int nVerts = m_meshResolution.width() * m_meshResolution.height();
        const int stride = (3 + 2 + 3 + 4) * sizeof(float);
        const int faces = 2 * (m_meshResolution.width() - 1) * (m_meshResolution.height() - 1);

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

        // Each primitive has 3 vertives
        m_indexAttribute->setCount(faces * 3);

        m_vertexBuffer->setBufferFunctor(QBufferFunctorPtr(new PlaneVertexBufferFunctor(m_width, m_height, m_meshResolution)));
        m_indexBuffer->setBufferFunctor(QBufferFunctorPtr(new PlaneIndexBufferFunctor(m_meshResolution)));

        addAttribute(m_positionAttribute);
        addAttribute(m_texCoordAttribute);
        addAttribute(m_normalAttribute);
        addAttribute(m_tangentAttribute);
        addAttribute(m_indexAttribute);
    }

    ~PlaneGeometry()
    {
        QGeometry::cleanup();
    }

    void updateVertices()
    {
        const int nVerts = m_meshResolution.width() * m_meshResolution.height();

        m_positionAttribute->setCount(nVerts);
        m_normalAttribute->setCount(nVerts);
        m_texCoordAttribute->setCount(nVerts);
        m_tangentAttribute->setCount(nVerts);

        m_vertexBuffer->setBufferFunctor(QBufferFunctorPtr(new PlaneVertexBufferFunctor(m_width, m_height, m_meshResolution)));
    }

    void updateIndices()
    {
        const int faces = 2 * (m_meshResolution.width() - 1) * (m_meshResolution.height() - 1);
        // Each primitive has 3 vertices
        m_indexAttribute->setCount(faces * 3);
        m_indexBuffer->setBufferFunctor(QBufferFunctorPtr(new PlaneIndexBufferFunctor(m_meshResolution)));
    }

    void setResolution(const QSize &resolution)
    {
        if (m_meshResolution == resolution)
            return;
        m_meshResolution = resolution;
        updateVertices();
        updateIndices();
        emit resolutionChanged();
    }

    void setWidth(float width)
    {
        if (width == m_width)
            return;
        m_width = width;
        updateVertices();
        emit widthChanged();
    }

    void setHeight(float height)
    {
        if (height == m_height)
            return;
        m_height = height;
        updateVertices();
        emit heightChanged();
    }

    QSize resolution() const { return m_meshResolution; }
    int width() const { return m_width; }
    int height() const { return m_height; }

Q_SIGNALS:
    void resolutionChanged();
    void widthChanged();
    void heightChanged();

private:
    float m_width;
    float m_height;
    QSize m_meshResolution;
    QAttribute *m_positionAttribute;
    QAttribute *m_normalAttribute;
    QAttribute *m_texCoordAttribute;
    QAttribute *m_tangentAttribute;
    QAttribute *m_indexAttribute;
    QBuffer *m_vertexBuffer;
    QBuffer *m_indexBuffer;
};

QPlaneMesh::QPlaneMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    PlaneGeometry *geometry = new PlaneGeometry(this);
    QObject::connect(geometry, &PlaneGeometry::widthChanged, this, &QPlaneMesh::widthChanged);
    QObject::connect(geometry, &PlaneGeometry::heightChanged, this, &QPlaneMesh::heightChanged);
    QObject::connect(geometry, &PlaneGeometry::resolutionChanged, this, &QPlaneMesh::meshResolutionChanged);
    QGeometryRenderer::setGeometry(geometry);
}

QPlaneMesh::~QPlaneMesh()
{
    QNode::cleanup();
}

void QPlaneMesh::setWidth(float width)
{
    static_cast<PlaneGeometry *>(geometry())->setWidth(width);
}

float QPlaneMesh::width() const
{
    return static_cast<PlaneGeometry *>(geometry())->width();
}

void QPlaneMesh::setHeight(float height)
{
    static_cast<PlaneGeometry *>(geometry())->setHeight(height);
}

float QPlaneMesh::height() const
{
    return static_cast<PlaneGeometry *>(geometry())->height();
}

void QPlaneMesh::setMeshResolution(const QSize &resolution)
{
    static_cast<PlaneGeometry *>(geometry())->setResolution(resolution);
}

QSize QPlaneMesh::meshResolution() const
{
    return static_cast<PlaneGeometry *>(geometry())->resolution();
}

} // namespace Qt3D

QT_END_NAMESPACE

#include "qplanemesh.moc"
