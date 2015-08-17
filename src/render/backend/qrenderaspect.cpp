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

#include "qrenderaspect.h"
#include "qrenderaspect_p.h"

#include <Qt3DRenderer/private/texturedatamanager_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/scenemanager_p.h>
#include <Qt3DRenderer/private/geometryrenderermanager_p.h>

#include <Qt3DRenderer/qabstractsceneloader.h>
#include <Qt3DRenderer/qcameraselector.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qlayer.h>
#include <Qt3DRenderer/qlayerfilter.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qparametermapping.h>
#include <Qt3DRenderer/qrenderpassfilter.h>
#include <Qt3DRenderer/qrendertargetselector.h>
#include <Qt3DRenderer/qtechniquefilter.h>
#include <Qt3DRenderer/qviewport.h>
#include <Qt3DRenderer/qrendertarget.h>
#include <Qt3DRenderer/qclearbuffer.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qshaderdata.h>
#include <Qt3DRenderer/qstateset.h>
#include <Qt3DRenderer/qnodraw.h>
#include <Qt3DCore/qcameralens.h>
#include <Qt3DRenderer/qattribute.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qgeometry.h>
#include <Qt3DRenderer/qgeometryrenderer.h>

#include <Qt3DRenderer/private/cameraselectornode_p.h>
#include <Qt3DRenderer/private/layerfilternode_p.h>
#include <Qt3DRenderer/private/renderannotation_p.h>
#include <Qt3DRenderer/private/renderentity_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/rendershaderdata_p.h>
#include <Qt3DRenderer/private/renderpassfilternode_p.h>
#include <Qt3DRenderer/private/rendertargetselectornode_p.h>
#include <Qt3DRenderer/private/techniquefilternode_p.h>
#include <Qt3DRenderer/private/viewportnode_p.h>
#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DRenderer/private/scenemanager_p.h>
#include <Qt3DRenderer/private/clearbuffer_p.h>
#include <Qt3DRenderer/private/sortmethod_p.h>
#include <Qt3DRenderer/private/sortcriterion_p.h>
#include <Qt3DRenderer/private/framegraphsubtreeselector_p.h>
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/private/rendernodefunctor_p.h>
#include <Qt3DRenderer/private/framegraphnode_p.h>
#include <Qt3DRenderer/private/loadtexturedatajob_p.h>
#include <Qt3DRenderer/private/updateboundingvolumejob_p.h>
#include <Qt3DRenderer/private/updateworldtransformjob_p.h>
#include <Qt3DRenderer/private/framecleanupjob_p.h>
#include <Qt3DRenderer/private/rendertextureimage_p.h>
#include <Qt3DRenderer/private/statesetnode_p.h>
#include <Qt3DRenderer/private/nodraw_p.h>
#include <Qt3DRenderer/private/vsyncframeadvanceservice_p.h>
#include <Qt3DRenderer/private/renderattribute_p.h>
#include <Qt3DRenderer/private/renderbuffer_p.h>
#include <Qt3DRenderer/private/rendergeometry_p.h>
#include <Qt3DRenderer/private/rendergeometryrenderer_p.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qnodevisitor.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/qaspectfactory.h>
#include <Qt3DCore/qservicelocator.h>

#include <QDebug>
#include <QOffscreenSurface>
#include <QThread>
#include <QWindow>

static void initResources()
{
    Q_INIT_RESOURCE(render);
}

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QRenderAspectPrivate
    \internal
*/
QRenderAspectPrivate::QRenderAspectPrivate(QRenderAspect::RenderType type)
    : QAbstractAspectPrivate()
    , m_renderer(new Render::Renderer(type))
    , m_surfaceEventFilter(new Render::PlatformSurfaceFilter(m_renderer))
    , m_surface(Q_NULLPTR)
    , m_time(0)
    , m_initialized(false)
{
    initResources();
    m_aspectType = QAbstractAspect::AspectRenderer;
}

void QRenderAspectPrivate::setSurface(QSurface *surface)
{
    if (m_surface == surface)
        return;

    m_surface = surface;

    // If we have a new surface, install the platform surface event filter onto it
    // so that we get informed when the underlying platform surface is about to be
    // deleted and we can tell the renderer about it before it's too late.
    if (m_surface) {
        bool hasPlatformSurface = false;
        switch (m_surface->surfaceClass()) {
        case QSurface::Window: {
            QWindow *window = static_cast<QWindow *>(m_surface);
            m_surfaceEventFilter->setWindow(window);
            hasPlatformSurface = (window->handle() != Q_NULLPTR);
            break;
        }

        case QSurface::Offscreen: {
            QOffscreenSurface *offscreen = static_cast<QOffscreenSurface *>(m_surface);
            m_surfaceEventFilter->setOffscreenSurface(offscreen);
            hasPlatformSurface = (offscreen->handle() != Q_NULLPTR);
            break;
        }
        }

        // If the window/offscreen surface has a native surface, tell the renderer
        if (hasPlatformSurface)
            m_renderer->setSurface(surface);
    }
}

QRenderAspect::QRenderAspect(QObject *parent)
    : QAbstractAspect(*new QRenderAspectPrivate(Threaded), parent)
{
    // Won't return until the private RenderThread in Renderer has been created
    // The Renderer is set to wait the surface with a wait condition
    // Threads modifying the Renderer should be synchronized using the Renderer's mutex
    registerBackendTypes();
}

QRenderAspect::QRenderAspect(QRenderAspect::RenderType type, QObject *parent)
    : QAbstractAspect(*new QRenderAspectPrivate(type), parent)
{
    registerBackendTypes();
}

/*! \internal */
QRenderAspect::QRenderAspect(QRenderAspectPrivate &dd, QObject *parent)

    : QAbstractAspect(dd, parent)
{
    registerBackendTypes();
}

void QRenderAspect::registerBackendTypes()
{
    Q_D(QRenderAspect);
    registerBackendType<QEntity>(QBackendNodeFunctorPtr(new Render::RenderEntityFunctor(d->m_renderer)));
    registerBackendType<QTransform>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderTransform, Render::TransformManager>(d->m_renderer->transformManager())));
    registerBackendType<QMaterial>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderMaterial, Render::MaterialManager>(d->m_renderer->materialManager())));
    registerBackendType<QTechnique>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderTechnique, Render::TechniqueManager>(d->m_renderer->techniqueManager())));
    registerBackendType<QAbstractTextureProvider>(QBackendNodeFunctorPtr(new Render::RenderTextureFunctor(d->m_renderer->textureManager(), d->m_renderer->textureImageManager(), d->m_renderer->textureDataManager())));
    registerBackendType<QShaderProgram>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderShader, Render::ShaderManager>(d->m_renderer->shaderManager())));
    registerBackendType<QEffect>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderEffect, Render::EffectManager>(d->m_renderer->effectManager())));
    registerBackendType<QAnnotation>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderAnnotation, Render::CriterionManager>(d->m_renderer->criterionManager())));
    registerBackendType<QCameraLens>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderCameraLens, Render::CameraManager>(d->m_renderer->cameraManager())));
    registerBackendType<QLayer>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderLayer, Render::LayerManager>(d->m_renderer->layerManager())));
    registerBackendType<QRenderPass>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderRenderPass, Render::RenderPassManager>(d->m_renderer->renderPassManager())));
    registerBackendType<Render::QAbstractSceneLoader>(QBackendNodeFunctorPtr(new Render::RenderSceneFunctor(d->m_renderer->sceneManager())));
    registerBackendType<QRenderTarget>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderTarget, Render::RenderTargetManager>(d->m_renderer->renderTargetManager())));
    registerBackendType<QRenderAttachment>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderAttachment, Render::AttachmentManager>(d->m_renderer->attachmentManager())));
    registerBackendType<QSortCriterion>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::SortCriterion, Render::SortCriterionManager>(d->m_renderer->sortCriterionManager())));
    registerBackendType<QClearBuffer>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::ClearBuffer, QClearBuffer>(d->m_renderer->frameGraphManager())));
    registerBackendType<QTechniqueFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::TechniqueFilter, QTechniqueFilter>(d->m_renderer->frameGraphManager())));
    registerBackendType<QViewport>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::ViewportNode, QViewport>(d->m_renderer->frameGraphManager())));
    registerBackendType<QRenderPassFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::RenderPassFilter, QRenderPassFilter>(d->m_renderer->frameGraphManager())));
    registerBackendType<QCameraSelector>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::CameraSelector, QCameraSelector>(d->m_renderer->frameGraphManager())));
    registerBackendType<QRenderTargetSelector>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::RenderTargetSelector, QRenderTargetSelector>(d->m_renderer->frameGraphManager())));
    registerBackendType<QLayerFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::LayerFilterNode, QLayerFilter>(d->m_renderer->frameGraphManager())));
    registerBackendType<QSortMethod>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::SortMethod, QSortMethod>(d->m_renderer->frameGraphManager())));
    registerBackendType<QFrameGraphSelector>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::FrameGraphSubtreeSelector, QFrameGraphSelector>(d->m_renderer->frameGraphManager())));
    registerBackendType<QFrameGraph>(QBackendNodeFunctorPtr(new Render::FrameGraphComponentFunctor(d->m_renderer)));
    registerBackendType<QParameter>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderParameter, Render::ParameterManager>(d->m_renderer->parameterManager())));
    registerBackendType<QShaderData>(QBackendNodeFunctorPtr(new Render::RenderShaderDataFunctor(d->m_renderer->shaderDataManager())));
    registerBackendType<QAbstractTextureImage>(QBackendNodeFunctorPtr(new Render::RenderTextureImageFunctor(d->m_renderer->textureManager(), d->m_renderer->textureImageManager(), d->m_renderer->textureDataManager())));
    registerBackendType<QStateSet>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::StateSetNode, QStateSet>(d->m_renderer->frameGraphManager())));
    registerBackendType<QNoDraw>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::NoDraw, QNoDraw>(d->m_renderer->frameGraphManager())));
    registerBackendType<QBuffer>(QBackendNodeFunctorPtr(new Render::RenderBufferFunctor(d->m_renderer->bufferManager())));
    registerBackendType<QAttribute>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderAttribute, Render::AttributeManager>(d->m_renderer->attributeManager())));
    registerBackendType<QGeometry>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderGeometry, Render::GeometryManager>(d->m_renderer->geometryManager())));
    registerBackendType<QGeometryRenderer>(QBackendNodeFunctorPtr(new Render::RenderGeometryRendererFunctor(d->m_renderer->geometryRendererManager())));
}

void QRenderAspect::renderInitialize(QOpenGLContext *context)
{
    Q_D(QRenderAspect);
    d->m_renderer->initialize(context);
}

void QRenderAspect::renderSynchronous()
{
    Q_D(QRenderAspect);
    d->m_renderer->doRender();
}

/*!
 * Only called when rendering with QtQuick 2 and a Scene3D item
 */
void QRenderAspect::renderShutdown()
{
    Q_D(QRenderAspect);
    d->m_renderer->shutdown();
}

QVector<QAspectJobPtr> QRenderAspect::jobsToExecute(qint64 time)
{
    Q_D(QRenderAspect);
    d->m_time = time;

    // Create jobs that will get exectued by the threadpool
    QVector<QAspectJobPtr> jobs;

    // Create jobs to load in any meshes that are pending
    if (d->m_renderer != Q_NULLPTR && d->m_renderer->isRunning()) {

        // Create the jobs to build the frame
        d->m_framePreparationJob.reset(new Render::FramePreparationJob(d->m_renderer->renderSceneRoot()));
        d->m_cleanupJob.reset(new Render::FrameCleanupJob(d->m_renderer));
        d->m_worldTransformJob.reset(new Render::UpdateWorldTransformJob(d->m_renderer->renderSceneRoot()));
        d->m_boundingVolumeJob.reset(new Render::UpdateBoundingVolumeJob(d->m_renderer->renderSceneRoot()));

        const QVector<QNodeId> texturesPending = d->m_renderer->textureDataManager()->texturesPending();
        Q_FOREACH (const QNodeId &textureId, texturesPending) {
            Render::LoadTextureDataJobPtr loadTextureJob(new Render::LoadTextureDataJob(textureId));
            loadTextureJob->setRenderer(d->m_renderer);
            jobs.append(loadTextureJob);
        }
        // TO DO: Have 2 jobs queue
        // One for urgent jobs that are mandatory for the rendering of a frame
        // Another for jobs that can span across multiple frames (Scene/Mesh loading)
        const QVector<Render::LoadSceneJobPtr> sceneJobs = d->m_renderer->sceneManager()->pendingSceneLoaderJobs();
        Q_FOREACH (Render::LoadSceneJobPtr job, sceneJobs) {
            job->setRenderer(d->m_renderer);
            jobs.append(job);
        }

        const QVector<QAspectJobPtr> bufferJobs = d->m_renderer->createRenderBufferJobs();
        jobs.append(bufferJobs);

        const QVector<QAspectJobPtr> geometryJobs = d->m_renderer->createGeometryRendererJobs();
        jobs.append(geometryJobs);

        // Create jobs to update transforms and bounding volumes
        // We can only update bounding volumes once all world transforms are known
        d->m_boundingVolumeJob->addDependency(d->m_worldTransformJob);
        d->m_framePreparationJob->addDependency(d->m_worldTransformJob);

        // Add all jobs to queue
        jobs.append(d->m_worldTransformJob);
        jobs.append(d->m_boundingVolumeJob);
        jobs.append(d->m_framePreparationJob);

        // Traverse the current framegraph and create jobs to populate
        // RenderBins with RenderCommands
        QVector<QAspectJobPtr> renderBinJobs = d->m_renderer->createRenderBinJobs();
        // TODO: Add wrapper around ThreadWeaver::Collection
        for (int i = 0; i < renderBinJobs.size(); ++i) {
            QAspectJobPtr renderBinJob = renderBinJobs.at(i);
            renderBinJob->addDependency(d->m_boundingVolumeJob);
            jobs.append(renderBinJob);
            d->m_cleanupJob->addDependency(renderBinJob);
        }
        jobs.append(d->m_cleanupJob);
    }
    return jobs;
}

void QRenderAspect::sceneNodeAdded(QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
    QNode *n = nodePtr.data();
    QNodeVisitor visitor;
    visitor.traverse(n, this, &QRenderAspect::visitNode);
}

void QRenderAspect::sceneNodeRemoved(QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
    QNode *n = nodePtr.data();
    QAbstractAspect::clearBackendNode(n);
}

qint64 QRenderAspect::time() const
{
    Q_D(const QRenderAspect);
    return d->m_time;
}

void QRenderAspect::setRootEntity(QEntity *rootObject)
{
    // setSceneGraphRoot is synchronized using the Renderer's mutex
    Q_D(QRenderAspect);
    QNodeVisitor visitor;
    visitor.traverse(rootObject, this, &QRenderAspect::visitNode);
    d->m_renderer->setSceneGraphRoot(d->m_renderer->renderNodesManager()->lookupResource(rootObject->id()));
}

void QRenderAspect::onInitialize(const QVariantMap &data)
{
    // TODO: Remove the m_initialized variable and split out onInitialize()
    // and setting a resource (the QSurface) on the aspects.
    Q_D(QRenderAspect);
    if (!d->m_initialized) {

        // Register the VSyncFrameAdvanceService to drive the aspect manager loop
        // depending on the vsync
        services()->registerServiceProvider(QServiceLocator::FrameAdvanceService,
                                            d->m_renderer->vsyncFrameAdvanceService());

        d->m_renderer->setQRenderAspect(this);
        d->m_renderer->createAllocators();
        d->m_initialized = true;
    }

    QSurface *surface = Q_NULLPTR;
    const QVariant &v = data.value(QStringLiteral("surface"));
    if (v.isValid())
        surface = v.value<QSurface *>();

    if (surface)
        d->setSurface(surface);
}

void QRenderAspect::onStartup()
{
}

void QRenderAspect::onShutdown()
{
}

void QRenderAspect::onCleanup()
{
    Q_D(QRenderAspect);
    delete d->m_renderer;
    d->m_renderer = Q_NULLPTR;
}

void QRenderAspect::visitNode(QNode *node)
{
    QAbstractAspect::createBackendNode(node);
}

} // Qt3D

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("render", QT_PREPEND_NAMESPACE(Qt3D), QRenderAspect)
