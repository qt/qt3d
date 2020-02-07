/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "geometryrenderer_p.h"
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/qboundingvolume_p.h>
#include <Qt3DRender/private/qgeometryrenderer_p.h>
#include <Qt3DRender/private/qmesh_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qtypedpropertyupdatechange_p.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <QtCore/qcoreapplication.h>

#include <memory>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

GeometryRenderer::GeometryRenderer()
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
    , m_primitiveType(QGeometryRenderer::Triangles)
    , m_dirty(false)
    , m_manager(nullptr)
{
}

GeometryRenderer::~GeometryRenderer()
{
}

void GeometryRenderer::cleanup()
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
    m_primitiveType = QGeometryRenderer::Triangles;
    m_geometryId = Qt3DCore::QNodeId();
    m_dirty = false;
    m_geometryFactory.reset();
    qDeleteAll(m_triangleVolumes);
    m_triangleVolumes.clear();
}

void GeometryRenderer::setManager(GeometryRendererManager *manager)
{
    m_manager = manager;
}

void GeometryRenderer::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QGeometryRenderer *node = qobject_cast<const QGeometryRenderer *>(frontEnd);
    if (!node)
        return;

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
    m_dirty |= m_primitiveType != node->primitiveType();
    m_primitiveType = node->primitiveType();
    m_dirty |= (node->geometry() && m_geometryId != node->geometry()->id()) || (!node->geometry() && !m_geometryId.isNull());
    m_geometryId = node->geometry() ? node->geometry()->id() : Qt3DCore::QNodeId();
    QGeometryFactoryPtr newFunctor = node->geometryFactory();
    const bool functorDirty = ((m_geometryFactory && !newFunctor)
                               || (!m_geometryFactory && newFunctor)
                               || (m_geometryFactory && newFunctor && !(*newFunctor == *m_geometryFactory)));
    if (functorDirty) {
        m_dirty = true;
        m_geometryFactory = newFunctor;
        if (m_geometryFactory && m_manager != nullptr) {
            m_manager->addDirtyGeometryRenderer(peerId());

            const bool isQMeshFunctor = m_geometryFactory->id() == Qt3DRender::functorTypeId<MeshLoaderFunctor>();
            if (isQMeshFunctor) {
                const QMesh *meshNode = static_cast<const QMesh *>(node);
                QMeshPrivate *dmeshNode = QMeshPrivate::get(const_cast<QMesh *>(meshNode));
                dmeshNode->setStatus(QMesh::Loading);
            }
        }
    }

    markDirty(AbstractRenderer::GeometryDirty);
}

GeometryFunctorResult GeometryRenderer::executeFunctor()
{
    Q_ASSERT(m_geometryFactory);

    // What kind of functor are we dealing with?
    const bool isQMeshFunctor = m_geometryFactory->id() == Qt3DRender::functorTypeId<MeshLoaderFunctor>();

    if (isQMeshFunctor) {
        QSharedPointer<MeshLoaderFunctor> meshLoader = qSharedPointerCast<MeshLoaderFunctor>(m_geometryFactory);

        // Set the aspect engine to allow remote downloads
        if (meshLoader->nodeManagers() == nullptr)
            meshLoader->setNodeManagers(m_renderer->nodeManagers());

        if (meshLoader->downloaderService() == nullptr) {
            Qt3DCore::QServiceLocator *services = m_renderer->services();
            meshLoader->setDownloaderService(services->service<Qt3DCore::QDownloadHelperService>(Qt3DCore::QServiceLocator::DownloadHelperService));
        };
    }

    // Load geometry
    QGeometry *geometry = (*m_geometryFactory)();
    QMesh::Status meshLoaderStatus = QMesh::None;

    // If the geometry is null, then we were either unable to load it (Error)
    // or the mesh is located at a remote url and needs to be downloaded first (Loading)
    if (geometry != nullptr) {
        // Move the QGeometry object to the main thread and notify the
        // corresponding QGeometryRenderer
        const auto appThread = QCoreApplication::instance()->thread();
        geometry->moveToThread(appThread);
    }

    // Send Status
    if (isQMeshFunctor) {
        QSharedPointer<MeshLoaderFunctor> meshLoader = qSharedPointerCast<MeshLoaderFunctor>(m_geometryFactory);
        meshLoaderStatus = meshLoader->status();
    }

    return { geometry, meshLoaderStatus };
}

void GeometryRenderer::unsetDirty()
{
    m_dirty = false;
}


void GeometryRenderer::setTriangleVolumes(const QVector<RayCasting::QBoundingVolume *> &volumes)
{
    qDeleteAll(m_triangleVolumes);
    m_triangleVolumes = volumes;
}

QVector<RayCasting::QBoundingVolume *> GeometryRenderer::triangleData() const
{
    return m_triangleVolumes;
}

GeometryRendererFunctor::GeometryRendererFunctor(AbstractRenderer *renderer, GeometryRendererManager *manager)
    : m_manager(manager)
    , m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *GeometryRendererFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    GeometryRenderer *geometryRenderer = m_manager->getOrCreateResource(change->subjectId());
    geometryRenderer->setManager(m_manager);
    geometryRenderer->setRenderer(m_renderer);
    return geometryRenderer;
}

Qt3DCore::QBackendNode *GeometryRendererFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_manager->lookupResource(id);
}

void GeometryRendererFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_manager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
