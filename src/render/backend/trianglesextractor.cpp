/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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

#include "trianglesextractor_p.h"
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/triangleboundingvolume_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {

bool isTriangleBased(Qt3DRender::QGeometryRenderer::PrimitiveType type) Q_DECL_NOEXCEPT
{
    switch (type) {
    case Qt3DRender::QGeometryRenderer::Triangles:
    case Qt3DRender::QGeometryRenderer::TriangleStrip:
    case Qt3DRender::QGeometryRenderer::TriangleFan:
    case Qt3DRender::QGeometryRenderer::TrianglesAdjacency:
    case Qt3DRender::QGeometryRenderer::TriangleStripAdjacency:
        return true;
    default:
        return false;
    }
}

struct BufferInfo
{
    BufferInfo()
        : type(QAttribute::Float)
        , dataSize(0)
        , count(0)
        , byteStride(0)
        , byteOffset(0)
    {}

    QByteArray data;
    QAttribute::DataType type;
    uint dataSize;
    uint count;
    uint byteStride;
    uint byteOffset;
};

// TO DO: Add methods for triangle strip adjacency
// What about primitive restart ?

// indices, vertices are already offset
template<typename index, typename vertex>
QVector<QBoundingVolume *> traverseTrianglesIndexed(index *indices,
                                                    vertex *vertices,
                                                    const BufferInfo &indexInfo,
                                                    const BufferInfo &vertexInfo,
                                                    const Qt3DCore::QNodeId id)
{
    QVector<QBoundingVolume *> tris;
    uint i = 0;
    const uint verticesStride = vertexInfo.byteStride / sizeof(vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    QVector3D abc[3];
    while (i < indexInfo.count) {
        for (uint u = 0; u < 3; ++u) {
            uint idx = indices[i + u] * verticesStride;
            for (uint j = 0; j < maxVerticesDataSize; ++j) {
                abc[u][j] = vertices[idx + j];
            }
        }
        tris.push_back(new TriangleBoundingVolume(id, abc[2], abc[1], abc[0]));
        i += 3;
    }

    return tris;
}

// vertices are already offset
template<typename vertex>
QVector<QBoundingVolume *> traverseTriangles(vertex *vertices,
                                             const BufferInfo &vertexInfo,
                                             const Qt3DCore::QNodeId id)
{
    QVector<QBoundingVolume *> tris;
    uint i = 0;

    const uint verticesStride = vertexInfo.byteStride / sizeof(vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    QVector3D abc[3];
    while (i < vertexInfo.count) {
        for (uint u = 0; u < 3; ++u) {
            uint idx = (i + u) * verticesStride;
            for (uint j = 0; j < maxVerticesDataSize; ++j) {
                abc[u][j] = vertices[idx + j];
            }
        }
        tris.push_back(new TriangleBoundingVolume(id, abc[2], abc[1], abc[0]));
        i += 3;
    }
    return tris;
}

// indices, vertices are already offset
template<typename index, typename vertex>
QVector<QBoundingVolume *> traverseTriangleStripIndexed(index *indices,
                                                        vertex *vertices,
                                                        const BufferInfo &indexInfo,
                                                        const BufferInfo &vertexInfo,
                                                        const Qt3DCore::QNodeId id)
{
    QVector<QBoundingVolume *> tris;
    uint i = 0;
    const uint verticesStride = vertexInfo.byteStride / sizeof(vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    QVector3D abc[3];
    while (i < indexInfo.count) {
        for (uint u = 0; u < 3; ++u) {
            uint idx = indices[i + u] * verticesStride;
            for (uint j = 0; j < maxVerticesDataSize; ++j) {
                abc[u][j] = vertices[idx + j];
            }
        }
        tris.push_back(new TriangleBoundingVolume(id, abc[2], abc[1], abc[0]));
        ++i;
    }

    return tris;
}

// vertices are already offset
template<typename vertex>
QVector<QBoundingVolume *> traverseTriangleStrip(vertex *vertices,
                                                 const BufferInfo &vertexInfo,
                                                 const Qt3DCore::QNodeId id)
{
    QVector<QBoundingVolume *> tris;
    uint i = 0;

    const uint verticesStride = vertexInfo.byteStride / sizeof(vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    QVector3D abc[3];
    while (i < vertexInfo.count) {
        for (uint u = 0; u < 3; ++u) {
            uint idx = (i + u) * verticesStride;
            for (uint j = 0; j < maxVerticesDataSize; ++j) {
                abc[u][j] = vertices[idx + j];
            }
        }
        tris.push_back(new TriangleBoundingVolume(id, abc[2], abc[1], abc[0]));
        ++i;
    }
    return tris;
}

// indices, vertices are already offset
template<typename index, typename vertex>
QVector<QBoundingVolume *> traverseTriangleFanIndexed(index *indices,
                                                      vertex *vertices,
                                                      const BufferInfo &indexInfo,
                                                      const BufferInfo &vertexInfo,
                                                      const Qt3DCore::QNodeId id)
{
    QVector<QBoundingVolume *> tris;
    const uint verticesStride = vertexInfo.byteStride / sizeof(vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    QVector3D abc[3];

    for (uint j = 0; j < maxVerticesDataSize; ++j) {
        abc[0][j] = vertices[static_cast<int>(indices[0]) * verticesStride + j];
    }

    uint i = 1;
    while (i < indexInfo.count) {
        for (uint u = 0; u < 2; ++u) {
            uint idx = indices[i + u] * verticesStride;
            for (uint j = 0; j < maxVerticesDataSize; ++j) {
                abc[u + 1][j] = vertices[idx + j];
            }
        }
        tris.push_back(new TriangleBoundingVolume(id, abc[2], abc[1], abc[0]));
        i += 2;
    }

    return tris;
}

// vertices are already offset
template<typename vertex>
QVector<QBoundingVolume *> traverseTriangleFan(vertex *vertices,
                                               const BufferInfo &vertexInfo,
                                               const Qt3DCore::QNodeId id)
{
    QVector<QBoundingVolume *> tris;

    const uint verticesStride = vertexInfo.byteStride / sizeof(vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    QVector3D abc[3];

    for (uint j = 0; j < maxVerticesDataSize; ++j) {
        abc[0][j] = vertices[j];
    }

    uint i = 1;
    while (i < vertexInfo.count) {
        for (uint u = 0; u < 2; ++u) {
            uint idx = (i + u) * verticesStride;
            for (uint j = 0; j < maxVerticesDataSize; ++j) {
                abc[u + 1][j] = vertices[idx + j];
            }
        }
        tris.push_back(new TriangleBoundingVolume(id, abc[2], abc[1], abc[0]));
        i += 2;
    }
    return tris;
}

// indices, vertices are already offset
template<typename index, typename vertex>
QVector<QBoundingVolume *> traverseTriangleAdjacencyIndexed(index *indices,
                                                            vertex *vertices,
                                                            const BufferInfo &indexInfo,
                                                            const BufferInfo &vertexInfo,
                                                            const Qt3DCore::QNodeId id)
{
    QVector<QBoundingVolume *> tris;
    uint i = 0;
    const uint verticesStride = vertexInfo.byteStride / sizeof(vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    QVector3D abc[3];
    while (i < indexInfo.count) {
        for (uint u = 0; u < 6; u += 2) {
            uint idx = indices[i + u] * verticesStride;
            for (uint j = 0; j < maxVerticesDataSize; ++j) {
                abc[u / 2][j] = vertices[idx + j];
            }
        }
        tris.push_back(new TriangleBoundingVolume(id, abc[2], abc[1], abc[0]));
        i += 6;
    }

    return tris;
}

// vertices are already offset
template<typename vertex>
QVector<QBoundingVolume *> traverseTriangleAdjacency(vertex *vertices,
                                                     const BufferInfo &vertexInfo,
                                                     const Qt3DCore::QNodeId id)
{
    QVector<QBoundingVolume *> tris;
    uint i = 0;

    const uint verticesStride = vertexInfo.byteStride / sizeof(vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    QVector3D abc[3];
    while (i < vertexInfo.count) {
        for (uint u = 0; u < 6; u += 2) {
            uint idx = (i + u) * verticesStride;
            for (uint j = 0; j < maxVerticesDataSize; ++j) {
                abc[u / 2][j] = vertices[idx + j];
            }
        }
        tris.push_back(new TriangleBoundingVolume(id, abc[2], abc[1], abc[0]));
        i += 6;
    }
    return tris;
}


template <QAttribute::DataType> struct EnumToType;
template <> struct EnumToType<QAttribute::Byte> { typedef const char type; };
template <> struct EnumToType<QAttribute::UnsignedByte> { typedef const uchar type; };
template <> struct EnumToType<QAttribute::Short> { typedef const short type; };
template <> struct EnumToType<QAttribute::UnsignedShort> { typedef const ushort type; };
template <> struct EnumToType<QAttribute::Int> { typedef const int type; };
template <> struct EnumToType<QAttribute::UnsignedInt> { typedef const uint type; };
template <> struct EnumToType<QAttribute::Float> { typedef const float type; };
template <> struct EnumToType<QAttribute::Double> { typedef const double type; };

template<QAttribute::DataType v>
typename EnumToType<v>::type *castToType(const QByteArray &u, uint byteOffset)
{
    return reinterpret_cast< typename EnumToType<v>::type *>(u.constData() + byteOffset);
}

template<typename ReturnType, typename Func>
QVector<QBoundingVolume *> processBuffer(const BufferInfo &info, Func &f)
{
    switch (info.type) {
    case QAttribute::Byte: return f(info, castToType<QAttribute::Byte>(info.data, info.byteOffset));
    case QAttribute::UnsignedByte: return f(info, castToType<QAttribute::UnsignedByte>(info.data, info.byteOffset));
    case QAttribute::Short: return f(info, castToType<QAttribute::Short>(info.data, info.byteOffset));
    case QAttribute::UnsignedShort: return f(info, castToType<QAttribute::UnsignedShort>(info.data, info.byteOffset));
    case QAttribute::Int: return f(info, castToType<QAttribute::Int>(info.data, info.byteOffset));
    case QAttribute::UnsignedInt: return f(info, castToType<QAttribute::UnsignedInt>(info.data, info.byteOffset));
    case QAttribute::Float: return f(info, castToType<QAttribute::Float>(info.data, info.byteOffset));
    case QAttribute::Double: return f(info, castToType<QAttribute::Double>(info.data, info.byteOffset));
    default:
        return ReturnType();
    }
}

template<typename Index, typename ReturnType>
struct IndexedVertexExecutor
{
    template<typename Vertex>
    ReturnType operator ()(const BufferInfo &vertexInfo, Vertex * vertices)
    {
        switch (primitiveType) {
        case Qt3DRender::QGeometryRenderer::Triangles:
            return traverseTrianglesIndexed(indices, vertices, indexBufferInfo, vertexInfo, id);
        case Qt3DRender::QGeometryRenderer::TriangleStrip:
            return traverseTriangleStripIndexed(indices, vertices, indexBufferInfo, vertexInfo, id);
        case Qt3DRender::QGeometryRenderer::TriangleFan:
            return traverseTriangleFanIndexed(indices, vertices, indexBufferInfo, vertexInfo, id);
        case Qt3DRender::QGeometryRenderer::TrianglesAdjacency:
            return traverseTriangleAdjacencyIndexed(indices, vertices, indexBufferInfo, vertexInfo, id);
        case Qt3DRender::QGeometryRenderer::TriangleStripAdjacency:
        default:
            return ReturnType();
        }
    }

    BufferInfo indexBufferInfo;
    Index *indices;
    Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType;
    Qt3DCore::QNodeId id;
};

template<typename ReturnType>
struct IndexExecutor
{
    BufferInfo vertexBufferInfo;

    template<typename Index>
    ReturnType operator ()( const BufferInfo &indexInfo, Index *indices)
    {
        IndexedVertexExecutor<Index, ReturnType> exec;
        exec.primitiveType = primitiveType;
        exec.indices = indices;
        exec.indexBufferInfo = indexInfo;
        exec.id = id;
        return processBuffer<ReturnType>(vertexBufferInfo, exec);
    }

    Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType;
    Qt3DCore::QNodeId id;
};

template<typename ReturnType>
struct VertexExecutor
{
    template<typename Vertex>
    ReturnType operator ()(const BufferInfo &vertexInfo, Vertex *vertices)
    {
        switch (primitiveType) {
        case Qt3DRender::QGeometryRenderer::Triangles:
            return traverseTriangles(vertices, vertexInfo, id);
        case Qt3DRender::QGeometryRenderer::TriangleStrip:
            return traverseTriangleStrip(vertices, vertexInfo, id);
        case Qt3DRender::QGeometryRenderer::TriangleFan:
            return traverseTriangleFan(vertices, vertexInfo, id);
        case Qt3DRender::QGeometryRenderer::TrianglesAdjacency:
            return traverseTriangleAdjacency(vertices, vertexInfo, id);
        case Qt3DRender::QGeometryRenderer::TriangleStripAdjacency:
        default:
            return ReturnType();
        }
    }

    Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType;
    Qt3DCore::QNodeId id;
};

} // anonymous

TrianglesExtractor::TrianglesExtractor(GeometryRenderer *renderer, NodeManagers *manager)
    : m_renderer(renderer)
    , m_manager(manager)
{
}

QVector<QBoundingVolume *> TrianglesExtractor::extract(const Qt3DCore::QNodeId id)
{
    if (m_renderer && m_renderer->instanceCount() == 1 && isTriangleBased(m_renderer->primitiveType())) {
        Attribute *positionAttribute = Q_NULLPTR;
        Attribute *indexAttribute = Q_NULLPTR;
        Buffer *positionBuffer = Q_NULLPTR;
        Buffer *indexBuffer = Q_NULLPTR;
        Geometry *geom = m_manager->lookupResource<Geometry, GeometryManager>(m_renderer->geometryId());

        if (geom) {
            Qt3DRender::Render::Attribute *attribute = Q_NULLPTR;
            Q_FOREACH (const Qt3DCore::QNodeId attrId, geom->attributes()) {
                attribute = m_manager->lookupResource<Attribute, AttributeManager>(attrId);
                if (attribute){
                    if (attribute->name() == QAttribute::defaultPositionAttributeName())
                        positionAttribute = attribute;
                    else if (attribute->attributeType() == QAttribute::IndexAttribute)
                        indexAttribute = attribute;
                }
            }

            if (positionAttribute)
                positionBuffer = m_manager->lookupResource<Buffer, BufferManager>(positionAttribute->bufferId());
            if (indexAttribute)
                indexBuffer = m_manager->lookupResource<Buffer, BufferManager>(indexAttribute->bufferId());

            if (positionBuffer) {

                BufferInfo vertexBufferInfo;
                vertexBufferInfo.data = positionBuffer->data();
                vertexBufferInfo.type = positionAttribute->dataType();
                vertexBufferInfo.byteOffset = positionAttribute->byteOffset();
                vertexBufferInfo.byteStride = positionAttribute->byteStride();
                vertexBufferInfo.dataSize = positionAttribute->dataSize();
                vertexBufferInfo.count = positionAttribute->count();

                if (indexBuffer) { // Indexed

                    BufferInfo indexBufferInfo;
                    indexBufferInfo.data = indexBuffer->data();
                    indexBufferInfo.type = indexAttribute->dataType();
                    indexBufferInfo.byteOffset = indexAttribute->byteOffset();
                    indexBufferInfo.byteStride = indexAttribute->byteStride();
                    indexBufferInfo.count = indexAttribute->count();

                    IndexExecutor<QVector<QBoundingVolume *> > executor;
                    executor.vertexBufferInfo = vertexBufferInfo;
                    executor.primitiveType = m_renderer->primitiveType();
                    executor.id = id;

                    return processBuffer<QVector<QBoundingVolume *> >(indexBufferInfo, executor);

                } else { // Non Indexed

                    // Check into which type the buffer needs to be casted
                    VertexExecutor<QVector<QBoundingVolume *> > executor;
                    executor.primitiveType = m_renderer->primitiveType();
                    executor.id = id;

                    return processBuffer<QVector<QBoundingVolume *> >(vertexBufferInfo, executor);
                }
            }
        }
    }
    return QVector<QBoundingVolume *>();
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
