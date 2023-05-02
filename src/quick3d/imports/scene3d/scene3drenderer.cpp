// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "scene3drenderer_p.h"

#include <Qt3DCore/qaspectengine.h>
#include <Qt3DRender/qrenderaspect.h>
#include <QtCore/qthread.h>
#include <qopenglcontext.h>
#include <qopenglframebufferobject.h>
#include <QtQuick/qquickwindow.h>
#include <rhi/qrhi.h>

#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qservicelocator_p.h>

#include <scene3ditem_p.h>
#include <scene3dlogging_p.h>
#include <scene3dsgnode_p.h>

#include <QtQuick/private/qquickwindow_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class ContextSaver
{
public:
    explicit ContextSaver(QOpenGLContext *context = QOpenGLContext::currentContext())
        : m_context(context),
          m_surface(context ? context->surface() : nullptr)
    {
    }

    ~ContextSaver()
    {
        if (m_context && m_context->surface() != m_surface)
            m_context->makeCurrent(m_surface);
    }

    QOpenGLContext *context() const { return m_context; }
    QSurface *surface() const { return m_surface; }

private:
    QOpenGLContext * const m_context;
    QSurface * const m_surface;
};

/*!
    \class Qt3DRender::Scene3DRenderer
    \internal

    \brief The Scene3DRenderer class takes care of rendering a Qt3D scene
    within a Framebuffer object to be used by the QtQuick 2 renderer.

    The Scene3DRenderer class renders a Qt3D scene as provided by a Scene3DItem.
    It owns the aspectEngine even though it doesn't instantiate it.

    The render loop goes as follows:
    \list
    \li The main thread runs, drives Animations, etc. and causes changes to be
    reported to the Qt3D change arbiter. The first change reported will cause
    the scene3drenderer to be marked dirty.
    \li The QtQuick render thread starts a new frame, synchronizes the scene
    graph and emits afterSynchronizing. This will trigger some preparational
    steps for rendering and mark the QSGNode dirty if the Scene3DRenderer is
    dirty.
    \li The QtQuick render loop emits beforeRendering. If we're marked dirty or
    if the renderPolicy is set to Always, we'll ask the Qt3D renderer aspect to
    render. That call is blocking. If the aspect jobs are not done, yet, the
    renderer will exit early and we skip a frame.
    \endlist

    The shutdown procedure is a two steps process that goes as follow:

    \list
    \li The window is closed

    \li This triggers the windowsChanged signal which the Scene3DRenderer
    uses to perform the necessary cleanups in the QSGRenderThread (destroys
    DebugLogger ...) with the shutdown slot (queued connection).

    \li The destroyed signal of the window is also connected to the
    Scene3DRenderer. When triggered in the context of the main thread, the
    cleanup slot is called.
    \endlist

    There is an alternate shutdown procedure in case the QQuickItem is
    destroyed with an active window which can happen in the case where the
    Scene3D is used with a QtQuick Loader

    In that case the shutdown procedure goes the same except that the destroyed
    signal of the window is not called. Therefore the cleanup method is invoked
    to properly destroy the aspect engine.
 */
Scene3DRenderer::Scene3DRenderer()
    : QObject()
    , m_aspectEngine(nullptr)
    , m_renderAspect(nullptr)
    , m_node(nullptr)
    , m_window(nullptr)
    , m_needsShutdown(true)
    , m_shouldRender(false)
    , m_dirtyViews(false)
    , m_skipFrame(false)
    , m_allowRendering(0)
    , m_compositingMode(Scene3DItem::FBO)
{

}

void Scene3DRenderer::init(Qt3DCore::QAspectEngine *aspectEngine,
                           QRenderAspect *renderAspect)
{
    m_aspectEngine = aspectEngine;
    m_renderAspect = renderAspect;
    m_needsShutdown = true;

    // Detect which Rendering backend Qt3D is using
     Qt3DRender::QRenderAspectPrivate *aspectPriv = static_cast<QRenderAspectPrivate*>(QRenderAspectPrivate::get(m_renderAspect));
     Qt3DRender::Render::AbstractRenderer *renderer = aspectPriv->m_renderer;

    const bool isRHI = renderer->api() == API::RHI;
    if (isRHI)
        m_quickRenderer = new Scene3DRenderer::RHIRenderer;
    else
        m_quickRenderer = new Scene3DRenderer::GLRenderer;
    m_quickRenderer->initialize(this, renderer);
}

void Scene3DRenderer::setWindow(QQuickWindow *window)
{
    if (window == m_window)
        return;

    QObject::disconnect(m_window);
    m_window = window;

    if (m_window) {
        QObject::connect(m_window, &QQuickWindow::beforeRendering, this,
                         [this] () { m_quickRenderer->beforeRendering(this); }, Qt::DirectConnection);
        QObject::connect(m_window, &QQuickWindow::beforeRenderPassRecording, this,
                         [this] () { m_quickRenderer->beforeRenderPassRecording(this); }, Qt::DirectConnection);
    } else {
        shutdown();
    }
}

Scene3DRenderer::~Scene3DRenderer()
{
    qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
    shutdown();
}

Scene3DSGNode *Scene3DRenderer::sgNode() const
{
    return m_node;
}

bool Scene3DRenderer::isYUp() const
{
    return m_quickRenderer ? m_quickRenderer->isYUp() : true;
}

// Executed in the QtQuick render thread (which may even be the gui/main with QQuickWidget / RenderControl).
void Scene3DRenderer::shutdown()
{
    if (!m_needsShutdown)
        return;
    m_needsShutdown = false;

    m_quickRenderer->shutdown(this);
    delete m_quickRenderer;
    m_quickRenderer = nullptr;
}

// Render Thread, GUI locked
void Scene3DRenderer::beforeSynchronize()
{
    m_quickRenderer->beforeSynchronize(this);
}

void Scene3DRenderer::allowRender()
{
    m_allowRendering.release(1);
}

void Scene3DRenderer::setCompositingMode(Scene3DItem::CompositingMode mode)
{
    m_compositingMode = mode;
}

void Scene3DRenderer::setSkipFrame(bool skip)
{
    m_skipFrame = skip;
}

void Scene3DRenderer::setMultisample(bool multisample)
{
    m_multisample = multisample;
}

void Scene3DRenderer::setBoundingSize(const QSize &size)
{
    m_boundingRectSize = size;
}

QOpenGLFramebufferObject *Scene3DRenderer::GLRenderer::createMultisampledFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    int samples = QSurfaceFormat::defaultFormat().samples();
    if (samples == -1)
        samples = 4;
    format.setSamples(samples);
    return new QOpenGLFramebufferObject(size, format);
}

QOpenGLFramebufferObject *Scene3DRenderer::GLRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    return new QOpenGLFramebufferObject(size, format);
}

void Scene3DRenderer::GLRenderer::initialize(Scene3DRenderer *scene3DRenderer,
                                             Qt3DRender::Render::AbstractRenderer *renderer)
{
    Q_UNUSED(scene3DRenderer);
    Q_ASSERT(QOpenGLContext::currentContext());
    m_renderer = renderer;
    ContextSaver saver;
    Q_ASSERT(renderer->api() == API::OpenGL);

    m_renderer->setRenderDriver(Qt3DRender::Render::AbstractRenderer::Scene3D);
    m_renderer->setOpenGLContext(saver.context());
    m_renderer->initialize();
}

void Scene3DRenderer::GLRenderer::beforeSynchronize(Scene3DRenderer *scene3DRenderer)
{
    // Check size / multisampling
    QQuickWindow *window = scene3DRenderer->m_window;

    if (!window)
        return;

    // Only render if we are sure aspectManager->processFrame was called prior
    // We could otherwise enter a deadlock state
    if (!scene3DRenderer->m_allowRendering.tryAcquire(std::max(scene3DRenderer->m_allowRendering.available(), 1)))
        return;

    // In the case of OnDemand rendering, we still need to get to this
    // point to ensure we have processed jobs for all aspects.
    // We also still need to call render() to allow proceeding with the
    // next frame. However it won't be performing any 3d rendering at all
    // so we do it here and return early. This prevents a costly QtQuick
    // SceneGraph update for nothing
    if (scene3DRenderer->m_skipFrame) {
        scene3DRenderer->m_skipFrame = false;
        ContextSaver saver;
        m_renderer->render(false);
        return;
    }

    scene3DRenderer->m_shouldRender = true;

    m_multisample = scene3DRenderer->multisample();
    const QSize boundingRectSize = scene3DRenderer->boundingSize();
    const QSize currentSize = boundingRectSize * window->effectiveDevicePixelRatio();
    const bool sizeHasChanged = currentSize != m_lastSize;
    const bool multisampleHasChanged = m_multisample != m_lastMultisample;
    const bool forceRecreate = sizeHasChanged || multisampleHasChanged;
    // Store the current size as a comparison
    // point for the next frame
    m_lastSize = currentSize;
    m_lastMultisample = m_multisample;

    // Rebuild FBO if size/multisampling has changed
    const bool usesFBO = scene3DRenderer->m_compositingMode == Scene3DItem::FBO;
    auto node = scene3DRenderer->m_node;
    if (node == nullptr) {
        node = new Scene3DSGNode();
        scene3DRenderer->m_node = node;
    }
    if (usesFBO) {
        // Rebuild FBO and textures if never created or a resize has occurred
        if ((m_multisampledFBO.isNull() || forceRecreate) && m_multisample) {
            m_multisampledFBO.reset(createMultisampledFramebufferObject(m_lastSize));
            if (m_multisampledFBO->format().samples() == 0 || !QOpenGLFramebufferObject::hasOpenGLFramebufferBlit()) {
                m_multisample = false;
                m_multisampledFBO.reset(nullptr);
            }
        }

        const bool generateNewTexture = m_finalFBO.isNull() || forceRecreate;
        if (generateNewTexture) {
            m_finalFBO.reset(createFramebufferObject(m_lastSize));
            m_textureId = m_finalFBO->texture();
            m_texture.reset(QNativeInterface::QSGOpenGLTexture::fromNative(m_textureId, window, m_finalFBO->size(), QQuickWindow::TextureHasAlphaChannel));
        }

        // Set texture on node
        if (!node->texture() || generateNewTexture)
            node->setTexture(m_texture.data());
    }

    // Mark SGNodes as dirty so that QQuick will trigger some rendering
    if (node)
        node->markDirty(QSGNode::DirtyMaterial);
}

void Scene3DRenderer::GLRenderer::beforeRendering(Scene3DRenderer *scene3DRenderer)
{
    Q_UNUSED(scene3DRenderer);
}

void Scene3DRenderer::GLRenderer::beforeRenderPassRecording(Scene3DRenderer *scene3DRenderer)
{
    // When this is fired, beforeRendering was fired and the RenderPass for QtQuick was
    // started -> meaning the window was cleared but actual draw commands have yet to be
    // recorded.
    // When using the GL Renderer and FBO, that's the state we want to be in.

    QMutexLocker l(&scene3DRenderer->m_windowMutex);
    // Lock to ensure the window doesn't change while we are rendering
    if (!scene3DRenderer->m_window || !scene3DRenderer->m_shouldRender)
        return;
    // Reset flag for next frame
    scene3DRenderer->m_shouldRender = false;
    ContextSaver saver;

    // Create and bind FBO if using the FBO compositing mode
    const bool usesFBO = scene3DRenderer->m_compositingMode == Scene3DItem::FBO;
    if (usesFBO) {
        // Bind FBO
        if (m_multisample) //Only try to use MSAA when available
            m_multisampledFBO->bind();
        else
            m_finalFBO->bind();
    }

    // Render Qt3D Scene
    // Qt3D takes care of resetting the GL state to default values
    m_renderer->render(usesFBO);

    // We may have called doneCurrent() so restore the context if the rendering surface was changed
    // Note: keep in mind that the ContextSave also restores the surface when destroyed
    if (saver.context()->surface() != saver.surface())
        saver.context()->makeCurrent(saver.surface());

    if (usesFBO) {
        if (m_multisample) {
            // Blit multisampled FBO with non multisampled FBO with texture attachment
            const QRect dstRect(QPoint(0, 0), m_finalFBO->size());
            const QRect srcRect(QPoint(0, 0), m_multisampledFBO->size());
            QOpenGLFramebufferObject::blitFramebuffer(m_finalFBO.data(), dstRect,
                                                      m_multisampledFBO.data(), srcRect,
                                                      GL_COLOR_BUFFER_BIT,
                                                      GL_NEAREST,
                                                      0, 0,
                                                      QOpenGLFramebufferObject::DontRestoreFramebufferBinding);
        }

        // Restore QtQuick FBO
        QOpenGLFramebufferObject::bindDefault();

        // Only show the node once Qt3D has rendered to it
        // Avoids showing garbage on the first frame
        if (scene3DRenderer->m_node)
            scene3DRenderer->m_node->show();
    }
}

void Scene3DRenderer::GLRenderer::shutdown(Scene3DRenderer *sceneRenderer)
{
    // Shutdown the Renderer Aspect while the OpenGL context
    // is still valid
    if (sceneRenderer->m_renderAspect)
        m_renderer->shutdown();

    m_finalFBO.reset();
    m_multisampledFBO.reset();
}

void Scene3DRenderer::RHIRenderer::initialize(Scene3DRenderer *scene3DRenderer,
                                              Qt3DRender::Render::AbstractRenderer *renderer)
{
    QQuickWindow *window = scene3DRenderer->m_window;
    Q_ASSERT(window);
    Q_ASSERT(renderer->api() == API::RHI);

    // Retrieve RHI context
    QSGRendererInterface *rif = window->rendererInterface();
    const bool isRhi = QSGRendererInterface::isApiRhiBased(rif->graphicsApi());
    Q_ASSERT(isRhi);
    if (isRhi) {
        m_rhi = static_cast<QRhi *>(rif->getResource(window, QSGRendererInterface::RhiResource));
        if (!m_rhi)
            qFatal("No QRhi from QQuickWindow, this cannot happen");
        m_renderer = renderer;
        m_renderer->setRenderDriver(Qt3DRender::Render::AbstractRenderer::Scene3D);
        m_renderer->setRHIContext(m_rhi);
        m_renderer->initialize();
    }
}

void Scene3DRenderer::RHIRenderer::beforeSynchronize(Scene3DRenderer *scene3DRenderer)
{
    // Check size / multisampling
    QQuickWindow *window = scene3DRenderer->m_window;

    if (!window)
        return;

    // Only render if we are sure aspectManager->processFrame was called prior
    // We could otherwise enter a deadlock state
    if (!scene3DRenderer->m_allowRendering.tryAcquire(std::max(scene3DRenderer->m_allowRendering.available(), 1)))
        return;

    // In the case of OnDemand rendering, we still need to get to this
    // point to ensure we have processed jobs for all aspects.
    // We also still need to call render() to allow proceeding with the
    // next frame. However it won't be performing any 3d rendering at all
    // so we do it here and return early. This prevents a costly QtQuick
    // SceneGraph update for nothing
    if (scene3DRenderer->m_skipFrame) {
        scene3DRenderer->m_skipFrame = false;
        m_renderer->render(false);
        return;
    }

    scene3DRenderer->m_shouldRender = true;

    const QSize boundingRectSize = scene3DRenderer->boundingSize();
    const QSize currentSize = boundingRectSize * window->effectiveDevicePixelRatio();
    const bool sizeHasChanged = currentSize != m_lastSize;
    const bool multisampleHasChanged = scene3DRenderer->multisample() != m_lastMultisample;
    // Store the current size and multisample as a comparison point for the next frame
    m_lastMultisample = scene3DRenderer->multisample();
    m_lastSize = currentSize;

    const bool forceRecreate = sizeHasChanged || multisampleHasChanged;
    const bool usesFBO = scene3DRenderer->m_compositingMode == Scene3DItem::FBO;
    // Not sure how we could support underlay rendering properly given Qt3D RHI will render into its own
    // RHI RenderPasses prior to QtQuick and beginning a new RenderPass clears the screen
    Q_ASSERT(usesFBO);
    const bool generateNewTexture = m_texture.isNull() || forceRecreate;
    const int samples = m_lastMultisample ? 4 : 1;

    if (generateNewTexture) {
        releaseRHIResources();

        // Depth / Stencil
        m_rhiDepthRenderBuffer = m_rhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil, m_lastSize, samples);
        m_rhiDepthRenderBuffer->create();

        // Color Attachment or Color Resolutin texture (if using MSAA)
        m_rhiTexture = m_rhi->newTexture(QRhiTexture::RGBA8, m_lastSize, 1, QRhiTexture::RenderTarget|QRhiTexture::UsedAsTransferSource);
        m_rhiTexture->create();

        // Color Attachment description
        QRhiTextureRenderTargetDescription renderTargetDesc;
        renderTargetDesc.setDepthStencilBuffer(m_rhiDepthRenderBuffer);
        if (samples > 1) {
            m_rhiMSAARenderBuffer = m_rhi->newRenderBuffer(QRhiRenderBuffer::Color, m_lastSize, samples, {}, m_rhiTexture->format());
            m_rhiMSAARenderBuffer->create();
            QRhiColorAttachment att;
            att.setRenderBuffer(m_rhiMSAARenderBuffer);
            att.setResolveTexture(m_rhiTexture);
            renderTargetDesc.setColorAttachments({ att });
        } else {
            renderTargetDesc.setColorAttachments({ m_rhiTexture });
        }

        m_rhiRenderTarget = m_rhi->newTextureRenderTarget(renderTargetDesc);
        m_rhiRenderTargetPassDescriptor = m_rhiRenderTarget->newCompatibleRenderPassDescriptor();
        m_rhiRenderTarget->setRenderPassDescriptor(m_rhiRenderTargetPassDescriptor);
        m_rhiRenderTarget->create();

        // Create QSGTexture from QRhiTexture
        auto *windowPriv = QQuickWindowPrivate::get(window);
        m_texture.reset(windowPriv->createTextureFromNativeTexture(m_rhiTexture->nativeTexture().object,
                                                                   0, m_lastSize, QQuickWindow::TextureHasAlphaChannel));

        // Set the Default RenderTarget to use on the RHI Renderer
        // Note: this will release all pipelines using previousRenderTarget
        m_renderer->setDefaultRHIRenderTarget(m_rhiRenderTarget);
    }

    Scene3DSGNode *node = scene3DRenderer->m_node;
    if (node == nullptr) {
        node = new Scene3DSGNode();
        scene3DRenderer->m_node = node;
    }

    // Set texture on node
    if (!node->texture() || generateNewTexture)
        node->setTexture(m_texture.data());

    // Mark SGNodes as dirty so that QQuick will trigger some rendering
    node->markDirty(QSGNode::DirtyMaterial);
}

void Scene3DRenderer::RHIRenderer::beforeRendering(Scene3DRenderer *scene3DRenderer)
{
    // This is fired before QtQuick starts its RenderPass but after a RHI Command Buffer
    // has been created and after the RHI Frame has begun

    // This means the swap chain has been set up and that we only need to record commands

    // On the Qt3D side this also means we shouldn't be calling m_rhi->beginFrame/endFrame
    // and we should only be rendering using the provided swap chain

    // Therefore, if a QRenderSurfaceSelector is used in the FrameGraph to render to another surface,
    // this will not be supported with Scene3D/RHI


    QMutexLocker l(&scene3DRenderer->m_windowMutex);
    // Lock to ensure the window doesn't change while we are rendering
    if (!scene3DRenderer->m_window || !scene3DRenderer->m_shouldRender)
        return;
    // Reset flag for next frame
    scene3DRenderer->m_shouldRender = false;

    // TO DO: We need to check what we do about 3DViews and Composition Mode as those are not
    // directly applicable with RHI

    // Retrieve Command Buffer used by QtQuick
    QRhiCommandBuffer *cb = nullptr;
    QSGRendererInterface *rif = scene3DRenderer->m_window ->rendererInterface();
    QRhiSwapChain *swapchain = static_cast<QRhiSwapChain *>(rif->getResource(scene3DRenderer->m_window, QSGRendererInterface::RhiSwapchainResource));
    if (swapchain) {
        cb = swapchain->currentFrameCommandBuffer();
    } else {
        cb = static_cast<QRhiCommandBuffer *>(rif->getResource(scene3DRenderer->m_window, QSGRendererInterface::RhiRedirectCommandBuffer));
    }

    Q_ASSERT(cb);
    // Tell Qt3D renderer to use provided command buffer
    m_renderer->setRHICommandBuffer(cb);
    m_renderer->render(false);

    // Only show the node once Qt3D has rendered to it
    // Avoids showing garbage on the first frame
    if (scene3DRenderer->m_node)
        scene3DRenderer->m_node->show();
}

void Scene3DRenderer::RHIRenderer::beforeRenderPassRecording(Scene3DRenderer *scene3DRenderer)
{
    Q_UNUSED(scene3DRenderer);
}

void Scene3DRenderer::RHIRenderer::shutdown(Scene3DRenderer *scene3DRenderer)
{
    if (scene3DRenderer->m_renderAspect)
        m_renderer->shutdown();

    releaseRHIResources();
}

bool Scene3DRenderer::RHIRenderer::isYUp() const
{
    return m_rhi->isYUpInFramebuffer();
}

void Scene3DRenderer::RHIRenderer::releaseRHIResources()
{
    delete m_rhiRenderTarget;
    m_rhiRenderTarget = nullptr;

    delete m_rhiTexture;
    m_rhiTexture = nullptr;

    delete m_rhiDepthRenderBuffer;
    m_rhiDepthRenderBuffer = nullptr;

    delete m_rhiMSAARenderBuffer;
    m_rhiMSAARenderBuffer = nullptr;

    delete m_rhiColorRenderBuffer;
    m_rhiColorRenderBuffer = nullptr;

    delete m_rhiRenderTargetPassDescriptor;
    m_rhiRenderTargetPassDescriptor = nullptr;
}

Scene3DRenderer::QuickRenderer::QuickRenderer() {}
Scene3DRenderer::QuickRenderer::~QuickRenderer() {}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_scene3drenderer_p.cpp"
