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

#include "qcuboidmesh.h"

#include <Qt3DRenderer/qattribute.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qbufferfunctor.h>
#include <Qt3DRenderer/private/renderlogging_p.h>

#include <limits>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace {

enum PlaneNormal {
    PositiveX,
    NegativeX,
    PositiveY,
    NegativeY,
    PositiveZ,
    NegativeZ
};

void createPlaneVertexData(float w, float h, const QSize &resolution,
                           PlaneNormal normal, float planeDistance,
                           float *vertices)
{
    const float a0 = -w / 2.0f;
    const float b0 = -h / 2.0f;
    const float da = w / (resolution.width() - 1);
    const float db = h / (resolution.height() - 1);
    const float du = 1.0 / (resolution.width() - 1);
    const float dv = 1.0 / (resolution.height() - 1);
    float n = 1.0f;

    switch (normal) {
    case NegativeX:
        n = -1.0f; // fall through
    case PositiveX: {
        // Iterate over z
        for (int j = 0; j < resolution.height(); ++j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over y
            for (int i = 0; i < resolution.width(); ++i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = planeDistance;
                *vertices++ = a;
                *vertices++ = b;

                // texture coordinates
                *vertices++ = u;
                *vertices++ = v;

                // normal
                *vertices++ = n;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;

                // tangent
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
                *vertices++ = 1.0f;
            }
        }
        break;
    }

    case NegativeY:
        n = -1.0f;
    case PositiveY: {
        // Iterate over z
        for (int j = 0; j < resolution.height(); ++j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over x
            // This iterates in the opposite sense to the other directions
            // so that the winding order is correct
            for (int i = resolution.width() - 1; i >= 0; --i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = a;
                *vertices++ = planeDistance;
                *vertices++ = b;

                // texture coordinates
                *vertices++ = u;
                *vertices++ = v;

                // normal
                *vertices++ = 0.0f;
                *vertices++ = n;
                *vertices++ = 0.0f;

                // tangent
                *vertices++ = 1.0f;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
            }
        }
        break;
    }

    case NegativeZ:
        n = -1.0f;
    case PositiveZ: {
        // Iterate over y
        for (int j = 0; j < resolution.height(); ++j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over x
            for (int i = 0; i < resolution.width(); ++i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = a;
                *vertices++ = b;
                *vertices++ = planeDistance;

                // texture coordinates
                *vertices++ = u;
                *vertices++ = v;

                // normal
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = n;

                // tangent
                *vertices++ = 1.0f;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
            }
        }
        break;
    }
    } // switch (normal)
}

void createPlaneIndexData(PlaneNormal normal, const QSize &resolution, quint16 *indices, quint16 &baseVertex)
{
    float n = 1.0f;

    switch (normal) {
    case NegativeX:
    case NegativeY:
    case NegativeZ:
        n = -1.0f;
        break;
    default:
        break;
    }

    // Populate indices taking care to get correct CCW winding on all faces
    if (n > 0.0f) {
        for (int j = 0; j < resolution.height() - 1; ++j) {
            const int rowStartIndex = j * resolution.width() + baseVertex;
            const int nextRowStartIndex = (j + 1) * resolution.width() + baseVertex;

            // Iterate over x
            for (int i = 0; i < resolution.width() - 1; ++i) {
                // Split quad into two triangles
                *indices++ = rowStartIndex + i;
                *indices++ = rowStartIndex + i + 1;
                *indices++ = nextRowStartIndex + i;

                *indices++ = nextRowStartIndex + i;
                *indices++ = rowStartIndex + i + 1;
                *indices++ = nextRowStartIndex + i + 1;
            }
        }
    } else {
        for (int j = 0; j < resolution.height() - 1; ++j) {
            const int rowStartIndex = j * resolution.width() + baseVertex;
            const int nextRowStartIndex = (j + 1) * resolution.width() + baseVertex;

            // Iterate over x
            for (int i = 0; i < resolution.width() - 1; ++i) {
                // Split quad into two triangles
                *indices++ = rowStartIndex + i;
                *indices++ = nextRowStartIndex + i;
                *indices++ = rowStartIndex + i + 1;

                *indices++ = nextRowStartIndex + i;
                *indices++ = nextRowStartIndex + i + 1;
                *indices++ = rowStartIndex + i + 1;
            }
        }
    }
    baseVertex += resolution.width() * resolution.height();
}

QByteArray createCuboidVertexData(float xExtent,
                                  float yExtent,
                                  float zExtent,
                                  const QSize &yzResolution,
                                  const QSize &xzResolution,
                                  const QSize &xyResolution)
{
    Q_ASSERT(xExtent > 0.0f && yExtent > 0.0f && zExtent > 0.0);
    Q_ASSERT(yzResolution.width() >= 2 && yzResolution.height() >=2);
    Q_ASSERT(xzResolution.width() >= 2 && xzResolution.height() >=2);
    Q_ASSERT(xyResolution.width() >= 2 && xyResolution.height() >=2);

    const int yzVerts = yzResolution.width() * yzResolution.height();
    const int xzVerts = xzResolution.width() * xzResolution.height();
    const int xyVerts = xyResolution.width() * xyResolution.height();
    const int nVerts = 2 * (yzVerts + xzVerts + xyVerts);

    const quint32 elementSize = 3 + 3 + 2 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray vertexBytes;
    vertexBytes.resize(stride * nVerts);
    float* vertices = reinterpret_cast<float*>(vertexBytes.data());

    createPlaneVertexData(yExtent, zExtent, yzResolution, PositiveX, xExtent * 0.5f, vertices);
    vertices += yzVerts * elementSize;
    createPlaneVertexData(yExtent, zExtent, yzResolution, NegativeX, -xExtent * 0.5f, vertices);
    vertices += yzVerts * elementSize;
    createPlaneVertexData(xExtent, zExtent, xzResolution, PositiveY, yExtent * 0.5f, vertices);
    vertices += xzVerts * elementSize;
    createPlaneVertexData(xExtent, zExtent, xzResolution, NegativeY, -yExtent * 0.5f, vertices);
    vertices += xzVerts * elementSize;
    createPlaneVertexData(xExtent, yExtent, xyResolution, PositiveZ, zExtent * 0.5f, vertices);
    vertices += xyVerts * elementSize;
    createPlaneVertexData(xExtent, yExtent, xyResolution, NegativeZ, -zExtent * 0.5f, vertices);

    return vertexBytes;
}

QByteArray createCuboidIndexData(const QSize &yzResolution,
                                 const QSize &xzResolution,
                                 const QSize &xyResolution)
{
    Q_ASSERT(yzResolution.width() >= 2 && yzResolution.height() >= 2);
    Q_ASSERT(xzResolution.width() >= 2 && xzResolution.height() >= 2);
    Q_ASSERT(xyResolution.width() >= 2 && xyResolution.height() >= 2);

    const int yzIndices = 2 * 3 * (yzResolution.width() - 1) * (yzResolution.height() - 1);
    const int xzIndices = 2 * 3 * (xzResolution.width() - 1) * (xzResolution.height() - 1);
    const int xyIndices = 2 * 3 * (xyResolution.width() - 1) * (xyResolution.height() - 1);
    const int indexCount = 2 * (yzIndices + xzIndices + xyIndices);

    QByteArray indexData;
    indexData.resize(indexCount * sizeof(quint16));
    quint16 *indices = reinterpret_cast<quint16 *>(indexData.data());
    quint16 baseIndex = 0;

    createPlaneIndexData(PositiveX, yzResolution, indices, baseIndex);
    indices += yzIndices;
    createPlaneIndexData(NegativeX, yzResolution, indices, baseIndex);
    indices += yzIndices;
    createPlaneIndexData(PositiveY, xzResolution, indices, baseIndex);
    indices += xzIndices;
    createPlaneIndexData(NegativeY, xzResolution, indices, baseIndex);
    indices += xzIndices;
    createPlaneIndexData(PositiveZ, xyResolution, indices, baseIndex);
    indices += xyIndices;
    createPlaneIndexData(NegativeZ, xyResolution, indices, baseIndex);

    return indexData;
}

} // anonymous

class CuboidVertexBufferFunctor : public QBufferFunctor
{
public:
    explicit CuboidVertexBufferFunctor(float xExtent,
                                       float yExtent,
                                       float zExtent,
                                       const QSize &yzResolution,
                                       const QSize &xzResolution,
                                       const QSize &xyResolution)
        : m_xExtent(xExtent)
        , m_yExtent(yExtent)
        , m_zExtent(zExtent)
        , m_yzFaceResolution(yzResolution)
        , m_xzFaceResolution(xzResolution)
        , m_xyFaceResolution(xyResolution)
    {}

    ~CuboidVertexBufferFunctor() {}

    QByteArray operator()() Q_DECL_FINAL
    {
        return createCuboidVertexData(m_xExtent, m_yExtent, m_zExtent,
                                     m_yzFaceResolution, m_xzFaceResolution, m_xyFaceResolution);
    }

    bool operator ==(const QBufferFunctor &other) const Q_DECL_FINAL
    {
        const CuboidVertexBufferFunctor *otherFunctor = functor_cast<CuboidVertexBufferFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_xExtent == m_xExtent &&
                    otherFunctor->m_yExtent == m_yExtent &&
                    otherFunctor->m_zExtent == m_zExtent &&
                    otherFunctor->m_yzFaceResolution == m_yzFaceResolution &&
                    otherFunctor->m_xzFaceResolution == m_xzFaceResolution &&
                    otherFunctor->m_xyFaceResolution == m_xyFaceResolution);
        return false;
    }

    QT3D_FUNCTOR(CuboidVertexBufferFunctor)

private:
    float m_xExtent;
    float m_yExtent;
    float m_zExtent;
    QSize m_yzFaceResolution;
    QSize m_xzFaceResolution;
    QSize m_xyFaceResolution;
};

class CuboidIndexBufferFunctor : public QBufferFunctor
{
public:
    explicit CuboidIndexBufferFunctor(const QSize &yzResolution,
                                      const QSize &xzResolution,
                                      const QSize &xyResolution)
        : m_yzFaceResolution(yzResolution)
        , m_xzFaceResolution(xzResolution)
        , m_xyFaceResolution(xyResolution)
    {}

    ~CuboidIndexBufferFunctor() {}

    QByteArray operator()() Q_DECL_FINAL
    {
        return createCuboidIndexData(m_yzFaceResolution, m_xzFaceResolution, m_xyFaceResolution);
    }

    bool operator ==(const QBufferFunctor &other) const Q_DECL_FINAL
    {
        const CuboidIndexBufferFunctor *otherFunctor = functor_cast<CuboidIndexBufferFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_yzFaceResolution == m_yzFaceResolution &&
                    otherFunctor->m_xzFaceResolution == m_xzFaceResolution &&
                    otherFunctor->m_xyFaceResolution == m_xyFaceResolution);
        return false;
    }

    QT3D_FUNCTOR(CuboidIndexBufferFunctor)

private:
    QSize m_yzFaceResolution;
    QSize m_xzFaceResolution;
    QSize m_xyFaceResolution;
};

/*!
    \class Qt3D::QCuboidMeshPrivate
    \internal
*/
class CuboidGeometry : public QGeometry
{
    Q_OBJECT

public:
    explicit CuboidGeometry(QNode *parent = 0)
        : QGeometry(parent)
        , m_xExtent(1.0f)
        , m_yExtent(1.0f)
        , m_zExtent(1.0f)
        , m_yzFaceResolution(2, 2)
        , m_xzFaceResolution(2, 2)
        , m_xyFaceResolution(2, 2)
        , m_positionAttribute(new QAttribute(this))
        , m_normalAttribute(new QAttribute(this))
        , m_texCoordAttribute(new QAttribute(this))
        , m_tangentAttribute(new QAttribute(this))
        , m_indexAttribute(new QAttribute(this))
        , m_vertexBuffer(new QBuffer(QBuffer::VertexBuffer, this))
        , m_indexBuffer(new QBuffer(QBuffer::IndexBuffer, this))
    {
        // vec3 pos vec2 tex vec3 normal vec4 tangent
        const quint32 stride = (3 + 2 + 3 + 4) * sizeof(float);
        const int yzIndices = 2 * 3 * (m_yzFaceResolution.width() - 1) * (m_yzFaceResolution.height() - 1);
        const int xzIndices = 2 * 3 * (m_xzFaceResolution.width() - 1) * (m_xzFaceResolution.height() - 1);
        const int xyIndices = 2 * 3 * (m_xyFaceResolution.width() - 1) * (m_xyFaceResolution.height() - 1);
        const int yzVerts = m_yzFaceResolution.width() * m_yzFaceResolution.height();
        const int xzVerts = m_xzFaceResolution.width() * m_xzFaceResolution.height();
        const int xyVerts = m_xyFaceResolution.width() * m_xyFaceResolution.height();

        const int nVerts = 2 * (yzVerts + xzVerts + xyVerts);
        const int indexCount = 2 * (yzIndices + xzIndices + xyIndices);

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

        m_indexAttribute->setCount(indexCount);

        m_vertexBuffer->setBufferFunctor(QBufferFunctorPtr(new CuboidVertexBufferFunctor(m_xExtent, m_yExtent, m_zExtent,
                                                                                         m_yzFaceResolution, m_xzFaceResolution, m_xyFaceResolution)));
        m_indexBuffer->setBufferFunctor(QBufferFunctorPtr(new CuboidIndexBufferFunctor(m_yzFaceResolution, m_xzFaceResolution, m_xyFaceResolution)));

        addAttribute(m_positionAttribute);
        addAttribute(m_texCoordAttribute);
        addAttribute(m_normalAttribute);
        addAttribute(m_tangentAttribute);
        addAttribute(m_indexAttribute);
    }

    ~CuboidGeometry()
    {
        QGeometry::cleanup();
    }

    void updateIndices()
    {
        const int yzIndices = 2 * 3 * (m_yzFaceResolution.width() - 1) * (m_yzFaceResolution.height() - 1);
        const int xzIndices = 2 * 3 * (m_xzFaceResolution.width() - 1) * (m_xzFaceResolution.height() - 1);
        const int xyIndices = 2 * 3 * (m_xyFaceResolution.width() - 1) * (m_xyFaceResolution.height() - 1);
        const int indexCount = 2 * (yzIndices + xzIndices + xyIndices);

        m_indexAttribute->setCount(indexCount);
        m_indexBuffer->setBufferFunctor(QBufferFunctorPtr(new CuboidIndexBufferFunctor(m_yzFaceResolution, m_xzFaceResolution, m_xyFaceResolution)));
    }

    void updateVertices()
    {
        const int yzVerts = m_yzFaceResolution.width() * m_yzFaceResolution.height();
        const int xzVerts = m_xzFaceResolution.width() * m_xzFaceResolution.height();
        const int xyVerts = m_xyFaceResolution.width() * m_xyFaceResolution.height();
        const int nVerts = 2 * (yzVerts + xzVerts + xyVerts);

        m_positionAttribute->setCount(nVerts);
        m_normalAttribute->setCount(nVerts);
        m_texCoordAttribute->setCount(nVerts);
        m_tangentAttribute->setCount(nVerts);

        m_vertexBuffer->setBufferFunctor(QBufferFunctorPtr(new CuboidVertexBufferFunctor(m_xExtent, m_yExtent, m_zExtent,
                                                                                         m_yzFaceResolution, m_xzFaceResolution, m_xyFaceResolution)));
    }

    void setXExtent(float xExtent)
    {
        if (m_xExtent != xExtent) {
            m_xExtent = xExtent;
            updateVertices();
            emit xExtentChanged();
        }
    }

    void setYExtent(float yExtent)
    {
        if (m_yExtent != yExtent) {
            m_yExtent = yExtent;
            updateVertices();
            emit yExtentChanged();
        }
    }

    void setZExtent(float zExtent)
    {
        if (m_zExtent != zExtent) {
            m_zExtent = zExtent;
            updateVertices();
            emit zExtentChanged();
        }
    }

    void setYZMeshResolution(const QSize &resolution)
    {
        if (m_yzFaceResolution != resolution) {
            m_yzFaceResolution = resolution;
            updateVertices();
            updateIndices();
            emit yzMeshResolutionChanged();
        }
    }

    void setXZMeshResolution(const QSize &resolution)
    {
        if (m_xzFaceResolution != resolution) {
            m_xzFaceResolution = resolution;
            updateVertices();
            updateIndices();
            emit xzMeshResolutionChanged();
        }
    }


    void setXYMeshResolution(const QSize &resolution)
    {
        if (m_xyFaceResolution != resolution) {
            m_xyFaceResolution = resolution;
            updateVertices();
            updateIndices();
            emit xyMeshResolutionChanged();
        }
    }

    float xExtent() const { return m_xExtent; }
    float yExtent() const { return m_yExtent; }
    float zExtent() const { return m_zExtent; }
    QSize yzMeshResolution() const  { return m_yzFaceResolution; }
    QSize xyMeshResolution() const { return m_xyFaceResolution; }
    QSize xzMeshResolution() const { return m_xzFaceResolution; }

Q_SIGNALS:
    void xExtentChanged();
    void yExtentChanged();
    void zExtentChanged();

    void yzMeshResolutionChanged();
    void xzMeshResolutionChanged();
    void xyMeshResolutionChanged();

private:
    // Dimensions
    float m_xExtent;
    float m_yExtent;
    float m_zExtent;

    // Resolutions of faces with normal x, y, and z
    QSize m_yzFaceResolution;
    QSize m_xzFaceResolution;
    QSize m_xyFaceResolution;

    QAttribute *m_positionAttribute;
    QAttribute *m_normalAttribute;
    QAttribute *m_texCoordAttribute;
    QAttribute *m_tangentAttribute;
    QAttribute *m_indexAttribute;
    QBuffer *m_vertexBuffer;
    QBuffer *m_indexBuffer;
};

QCuboidMesh::QCuboidMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    CuboidGeometry *geometry = new CuboidGeometry(this);
    QObject::connect(geometry, &CuboidGeometry::xExtentChanged, this, &QCuboidMesh::xExtentChanged);
    QObject::connect(geometry, &CuboidGeometry::yExtentChanged, this, &QCuboidMesh::yExtentChanged);
    QObject::connect(geometry, &CuboidGeometry::zExtentChanged, this, &QCuboidMesh::zExtentChanged);
    QObject::connect(geometry, &CuboidGeometry::xyMeshResolutionChanged, this, &QCuboidMesh::xyMeshResolutionChanged);
    QObject::connect(geometry, &CuboidGeometry::xzMeshResolutionChanged, this, &QCuboidMesh::xzMeshResolutionChanged);
    QObject::connect(geometry, &CuboidGeometry::yzMeshResolutionChanged, this, &QCuboidMesh::yzMeshResolutionChanged);
    QGeometryRenderer::setGeometry(geometry);
}

QCuboidMesh::~QCuboidMesh()
{
    QNode::cleanup();
}

void QCuboidMesh::setXExtent(float xExtent)
{
    static_cast<CuboidGeometry *>(geometry())->setXExtent(xExtent);
}

float QCuboidMesh::xExtent() const
{
    return static_cast<CuboidGeometry *>(geometry())->xExtent();
}

void QCuboidMesh::setYExtent(float yExtent)
{
    static_cast<CuboidGeometry *>(geometry())->setYExtent(yExtent);
}

float QCuboidMesh::yExtent() const
{
    return static_cast<CuboidGeometry *>(geometry())->yExtent();
}

void QCuboidMesh::setZExtent(float zExtent)
{
    static_cast<CuboidGeometry *>(geometry())->setZExtent(zExtent);
}

float QCuboidMesh::zExtent() const
{
    return static_cast<CuboidGeometry *>(geometry())->zExtent();
}

void QCuboidMesh::setYZMeshResolution(const QSize &resolution)
{
    static_cast<CuboidGeometry *>(geometry())->setYZMeshResolution(resolution);
}

QSize QCuboidMesh::yzMeshResolution() const
{
    return static_cast<CuboidGeometry *>(geometry())->yzMeshResolution();
}

void QCuboidMesh::setXZMeshResolution(const QSize &resolution)
{
    static_cast<CuboidGeometry *>(geometry())->setXZMeshResolution(resolution);
}

QSize QCuboidMesh::xzMeshResolution() const
{
    return static_cast<CuboidGeometry *>(geometry())->xzMeshResolution();
}

void QCuboidMesh::setXYMeshResolution(const QSize &resolution)
{
    static_cast<CuboidGeometry *>(geometry())->setXYMeshResolution(resolution);
}

QSize QCuboidMesh::xyMeshResolution() const
{
    return static_cast<CuboidGeometry *>(geometry())->xyMeshResolution();
}

} // namespace Qt3D

QT_END_NAMESPACE

#include "qcuboidmesh.moc"
