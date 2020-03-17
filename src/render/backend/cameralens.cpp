/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "cameralens_p.h"
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/computefilteredboundingvolumejob_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qaspectmanager_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {


namespace {

class GetBoundingVolumeWithoutCameraJob : public ComputeFilteredBoundingVolumeJob
{
public:
    GetBoundingVolumeWithoutCameraJob(CameraLens *lens, QNodeId commandId)
        : m_lens(lens), m_requestId(commandId)
    {
    }

protected:
    // called in main thread
    void finished(Qt3DCore::QAspectManager *aspectManager, const Sphere &sphere) override
    {
        m_lens->processViewAllResult(aspectManager, sphere, m_requestId);
    }

private:
    CameraLens *m_lens;
    QNodeId m_requestId;
};

} // namespace

CameraLens::CameraLens()
    : BackendNode(QBackendNode::ReadWrite)
    , m_renderAspect(nullptr)
    , m_exposure(0.0f)
{
}

CameraLens::~CameraLens()
{
    cleanup();
}

void CameraLens::cleanup()
{
    QBackendNode::setEnabled(false);
}

void CameraLens::setRenderAspect(QRenderAspect *renderAspect)
{
    m_renderAspect = renderAspect;
}

Matrix4x4 CameraLens::viewMatrix(const Matrix4x4 &worldTransform)
{
    const Vector4D position = worldTransform * Vector4D(0.0f, 0.0f, 0.0f, 1.0f);
    // OpenGL convention is looking down -Z
    const Vector4D viewDirection = worldTransform * Vector4D(0.0f, 0.0f, -1.0f, 0.0f);
    const Vector4D upVector = worldTransform * Vector4D(0.0f, 1.0f, 0.0f, 0.0f);

    QMatrix4x4 m;
    m.lookAt(convertToQVector3D(Vector3D(position)),
             convertToQVector3D(Vector3D(position + viewDirection)),
             convertToQVector3D(Vector3D(upVector)));

    return Matrix4x4(m);
}

void CameraLens::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QCameraLens *node = qobject_cast<const QCameraLens *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    const Matrix4x4 projectionMatrix(node->projectionMatrix());
    if (projectionMatrix != m_projection) {
        m_projection = projectionMatrix;
        markDirty(AbstractRenderer::AllDirty);
    }

    if (!qFuzzyCompare(node->exposure(), m_exposure)) {
        m_exposure = node->exposure();
        markDirty(AbstractRenderer::AllDirty);
    }

    const QCameraLensPrivate *d = static_cast<const QCameraLensPrivate *>(QNodePrivate::get(node));
    if (d->m_pendingViewAllRequest != m_pendingViewAllRequest) {
        m_pendingViewAllRequest = d->m_pendingViewAllRequest;

        if (m_pendingViewAllRequest)
            computeSceneBoundingVolume(m_pendingViewAllRequest.entityId, m_pendingViewAllRequest.cameraId, m_pendingViewAllRequest.requestId);
    }
}

void CameraLens::computeSceneBoundingVolume(QNodeId entityId,
                                            QNodeId cameraId,
                                            QNodeId requestId)
{
    if (!m_renderer || !m_renderAspect)
        return;
    NodeManagers *nodeManagers = m_renderer->nodeManagers();

    Entity *root = m_renderer->sceneRoot();
    if (!entityId.isNull())
        root = nodeManagers->renderNodesManager()->lookupResource(entityId);
    if (!root)
        return;

    Entity *camNode = nodeManagers->renderNodesManager()->lookupResource(cameraId);
    ComputeFilteredBoundingVolumeJobPtr job(new GetBoundingVolumeWithoutCameraJob(this, requestId));
    job->addDependency(QRenderAspectPrivate::get(m_renderer->aspect())->m_expandBoundingVolumeJob);
    job->setRoot(root);
    job->setManagers(nodeManagers);
    job->ignoreSubTree(camNode);
    m_renderAspect->scheduleSingleShotJob(job);
}

void CameraLens::processViewAllResult(QAspectManager *aspectManager, const Sphere &sphere, QNodeId commandId)
{
    if (!m_pendingViewAllRequest || m_pendingViewAllRequest.requestId != commandId)
        return;
    if (sphere.radius() > 0.f) {
        QCameraLens *lens = qobject_cast<QCameraLens *>(aspectManager->lookupNode(peerId()));
        if (lens) {
            QCameraLensPrivate *dlens = static_cast<QCameraLensPrivate *>(QCameraLensPrivate::get(lens));
            dlens->processViewAllResult(m_pendingViewAllRequest.requestId, { sphere.center().x(), sphere.center().y(), sphere.center().z() }, sphere.radius());
        }
    }
    m_pendingViewAllRequest = {};
}

void CameraLens::setProjection(const Matrix4x4 &projection)
{
    m_projection = projection;
}

void CameraLens::setExposure(float exposure)
{
    m_exposure = exposure;
}

bool CameraLens::viewMatrixForCamera(EntityManager* manager, Qt3DCore::QNodeId cameraId,
                                     Matrix4x4 &viewMatrix, Matrix4x4 &projectionMatrix)
{
    Entity *camNode = manager->lookupResource(cameraId);
    if (!camNode)
        return false;
    Render::CameraLens *lens = camNode->renderComponent<CameraLens>();
    if (!lens || !lens->isEnabled())
        return false;

    viewMatrix = lens->viewMatrix(*camNode->worldTransform());
    projectionMatrix = lens->projection();
    return true;
}

CameraLensFunctor::CameraLensFunctor(AbstractRenderer *renderer, QRenderAspect *renderAspect)
    : m_manager(renderer->nodeManagers()->manager<CameraLens, CameraManager>())
    , m_renderer(renderer)
    , m_renderAspect(renderAspect)
{
}

QBackendNode *CameraLensFunctor::create(const QNodeCreatedChangeBasePtr &change) const
{
    CameraLens *backend = m_manager->getOrCreateResource(change->subjectId());
    backend->setRenderer(m_renderer);
    backend->setRenderAspect(m_renderAspect);
    return backend;
}

QBackendNode *CameraLensFunctor::get(QNodeId id) const
{
    return m_manager->lookupResource(id);
}

void CameraLensFunctor::destroy(QNodeId id) const
{
    m_manager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
