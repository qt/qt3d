// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "pointsvisitor_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/trianglesvisitor_p.h>
#include <Qt3DRender/private/visitorutils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {

// indices, vertices are already offset
template<typename Index, typename Vertex>
void traverseCoordinatesIndexed(Index *indices,
                                Vertex *vertices,
                                const BufferInfo &indexInfo,
                                const BufferInfo &vertexInfo,
                                PointsVisitor *visitor)
{
    uint i = 0;
    const uint verticesStride = vertexInfo.byteStride / sizeof(Vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    uint ndx;
    Vector3D abc;
    while (i < indexInfo.count) {
        ndx = indices[i];
        const uint idx = ndx * verticesStride;
        for (uint j = 0; j < maxVerticesDataSize; ++j) {
            abc[j] = vertices[idx + j];
        }
        visitor->visit(ndx, abc);
        ++i;
    }
}

// vertices are already offset
template<typename Vertex>
void traverseCoordinates(Vertex *vertices,
                         const BufferInfo &vertexInfo,
                         PointsVisitor *visitor)
{
    const uint verticesStride = vertexInfo.byteStride / sizeof(Vertex);
    const uint maxVerticesDataSize = qMin(vertexInfo.dataSize, 3U);

    uint ndx = 0;
    Vector3D abc;
    while (ndx < vertexInfo.count) {
        const uint idx = ndx * verticesStride;
        for (uint j = 0; j < maxVerticesDataSize; ++j)
            abc[j] = vertices[idx + j];
        visitor->visit(ndx, abc);
        ++ndx;
    }
}

template<typename Index, typename Visitor>
struct IndexedVertexExecutor
{
    template<typename Vertex>
    void operator ()(const BufferInfo &vertexInfo, Vertex * vertices)
    {
        traverseCoordinatesIndexed(m_indices, vertices, m_indexBufferInfo, vertexInfo, m_visitor);
    }

    BufferInfo m_indexBufferInfo;
    Index *m_indices;
    Qt3DRender::QGeometryRenderer::PrimitiveType m_primitiveType;
    Visitor* m_visitor;
};

template<typename Visitor>
struct IndexExecutor
{
    template<typename Index>
    void operator ()( const BufferInfo &indexInfo, Index *indices)
    {
        IndexedVertexExecutor<Index, Visitor> exec;
        exec.m_primitiveType = m_primitiveType;
        exec.m_indices = indices;
        exec.m_indexBufferInfo = indexInfo;
        exec.m_visitor = m_visitor;
        Qt3DRender::Render::Visitor::processBuffer(m_vertexBufferInfo, exec);
    }

    BufferInfo m_vertexBufferInfo;
    Qt3DRender::QGeometryRenderer::PrimitiveType m_primitiveType;
    Visitor* m_visitor;
};

template<typename Visitor>
struct VertexExecutor
{
    template<typename Vertex>
    void operator ()(const BufferInfo &vertexInfo, Vertex *vertices)
    {
        switch (m_primitiveType) {
        case Qt3DRender::QGeometryRenderer::Points:
            traverseCoordinates(vertices, vertexInfo, m_visitor);
            return;
        default:
            Q_UNREACHABLE_RETURN();
        }
    }

    Qt3DRender::QGeometryRenderer::PrimitiveType m_primitiveType;
    Visitor* m_visitor;
};

} // anonymous


PointsVisitor::~PointsVisitor()
{

}

void PointsVisitor::apply(const Qt3DCore::QEntity *entity)
{
    GeometryRenderer *renderer = m_manager->geometryRendererManager()->lookupResource(entity->id());
    apply(renderer, entity->id());
}

void PointsVisitor::apply(const GeometryRenderer *renderer, const Qt3DCore::QNodeId id)
{
    m_nodeId = id;
    if (renderer && renderer->instanceCount() == 1) {
        Visitor::visitPrimitives<GeometryRenderer, VertexExecutor<PointsVisitor>,
                                 IndexExecutor<PointsVisitor>, PointsVisitor>(m_manager, renderer, this);
    }
}

void PointsVisitor::apply(const PickingProxy *proxy, const Qt3DCore::QNodeId id)
{
    m_nodeId = id;
    if (proxy && proxy->instanceCount() == 1) {
        Visitor::visitPrimitives<PickingProxy, VertexExecutor<PointsVisitor>,
                                 IndexExecutor<PointsVisitor>, PointsVisitor>(m_manager, proxy, this);
    }
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
