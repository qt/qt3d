// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "pickingproxy_p.h"
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/qpickingproxy_p.h>
#include <Qt3DCore/private/qgeometryview_p.h>
#include <Qt3DCore/private/qnode_p.h>

#include <memory>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

PickingProxy::PickingProxy()
    : BackendNode(ReadWrite)
    , m_instanceCount(0)
    , m_vertexCount(0)
    , m_indexOffset(0)
    , m_firstInstance(0)
    , m_firstVertex(0)
    , m_indexBufferByteOffset(0)
    , m_restartIndexValue(-1)
    , m_verticesPerPatch(0)
    , m_primitiveRestartEnabled(false)
    , m_dirty(false)
    , m_valid(false)
    , m_primitiveType(Qt3DCore::QGeometryView::Triangles)
    , m_manager(nullptr)
{
}

PickingProxy::~PickingProxy()
{
}

void PickingProxy::cleanup()
{
    BackendNode::setEnabled(false);
    m_instanceCount = 0;
    m_vertexCount = 0;
    m_indexOffset = 0;
    m_firstInstance = 0;
    m_firstVertex = 0;
    m_indexBufferByteOffset = 0;
    m_restartIndexValue = -1;
    m_verticesPerPatch = 0;
    m_primitiveRestartEnabled = false;
    m_primitiveType = QGeometryView::Triangles;
    m_geometryId = Qt3DCore::QNodeId();
    m_dirty = false;
    m_valid = false;
}

void PickingProxy::setManager(PickingProxyManager *manager)
{
    m_manager = manager;
}

void PickingProxy::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QPickingProxy *node = qobject_cast<const QPickingProxy *>(frontEnd);
    if (!node)
        return;

    auto propertyUpdater = [this](const auto *node) {
        m_dirty |= m_instanceCount != node->instanceCount();
        m_instanceCount = node->instanceCount();
        m_dirty |= m_vertexCount != node->vertexCount();
        m_vertexCount = node->vertexCount();
        m_dirty |= m_indexOffset != node->indexOffset();
        m_indexOffset = node->indexOffset();
        m_dirty |= m_firstInstance != node->firstInstance();
        m_firstInstance = node->firstInstance();
        m_dirty |= m_firstVertex != node->firstVertex();
        m_firstVertex = node->firstVertex();
        m_dirty |= m_indexBufferByteOffset != node->indexBufferByteOffset();
        m_indexBufferByteOffset = node->indexBufferByteOffset();
        m_dirty |= m_restartIndexValue != node->restartIndexValue();
        m_restartIndexValue = node->restartIndexValue();
        m_dirty |= m_verticesPerPatch != node->verticesPerPatch();
        m_verticesPerPatch = node->verticesPerPatch();
        m_dirty |= m_primitiveRestartEnabled != node->primitiveRestartEnabled();
        m_primitiveRestartEnabled = node->primitiveRestartEnabled();
        m_dirty |= m_primitiveType != static_cast<QGeometryView::PrimitiveType>(node->primitiveType());
        m_primitiveType = static_cast<QGeometryView::PrimitiveType>(node->primitiveType());
        m_dirty |= (node->geometry() && m_geometryId != node->geometry()->id()) || (!node->geometry() && !m_geometryId.isNull());
        m_geometryId = node->geometry() ? node->geometry()->id() : Qt3DCore::QNodeId();
    };

    m_dirty = false;

    if (!node->view()) {
        m_dirty |= m_valid;
        m_valid = false;
    } else {
        m_dirty |= !m_valid;
        m_valid = true;
        propertyUpdater(node->view());
    }

    markDirty(AbstractRenderer::GeometryDirty);
}

void PickingProxy::unsetDirty()
{
    m_dirty = false;
}

PickingProxyFunctor::PickingProxyFunctor(AbstractRenderer *renderer, PickingProxyManager *manager)
    : m_manager(manager)
    , m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *PickingProxyFunctor::create(Qt3DCore::QNodeId id) const
{
    PickingProxy *node = m_manager->getOrCreateResource(id);
    node->setManager(m_manager);
    node->setRenderer(m_renderer);
    return node;
}

Qt3DCore::QBackendNode *PickingProxyFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_manager->lookupResource(id);
}

void PickingProxyFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_manager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
