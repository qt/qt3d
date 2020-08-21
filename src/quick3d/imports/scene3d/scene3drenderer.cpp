/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "scene3drenderer_p.h"

#include <Qt3DCore/qaspectengine.h>
#include <Qt3DRender/qrenderaspect.h>
#include <QtCore/qthread.h>
#include <qopenglcontext.h>
#include <qopenglframebufferobject.h>
#include <QtQuick/qquickwindow.h>

#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qservicelocator_p.h>

#include <scene3ditem_p.h>
#include <scene3dlogging_p.h>
#include <scene3dsgnode_p.h>
#include <scene3dview_p.h>

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
    , m_multisampledFBO(nullptr)
    , m_finalFBO(nullptr)
    , m_texture(nullptr)
    , m_node(nullptr)
    , m_window(nullptr)
    , m_multisample(false) // this value is not used, will be synced from the Scene3DItem instead
    , m_lastMultisample(false)
    , m_needsShutdown(true)
    , m_forceRecreate(false)
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

    Q_ASSERT(QOpenGLContext::currentContext());
    ContextSaver saver;
    static_cast<QRenderAspectPrivate*>(QRenderAspectPrivate::get(m_renderAspect))->renderInitialize(saver.context());
}

void Scene3DRenderer::setWindow(QQuickWindow *window)
{
    if (window == m_window)
        return;

    QObject::disconnect(m_window);
    m_window = window;

    if (m_window) {
        QObject::connect(m_window, &QQuickWindow::beforeRendering, this, &Scene3DRenderer::render, Qt::DirectConnection);
    } else {
        shutdown();
    }
}

Scene3DRenderer::~Scene3DRenderer()
{
    qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
    shutdown();
}


QOpenGLFramebufferObject *Scene3DRenderer::createMultisampledFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    int samples = QSurfaceFormat::defaultFormat().samples();
    if (samples == -1)
        samples = 4;
    format.setSamples(samples);
    return new QOpenGLFramebufferObject(size, format);
}

QOpenGLFramebufferObject *Scene3DRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    return new QOpenGLFramebufferObject(size, format);
}

// Executed in the QtQuick render thread (which may even be the gui/main with QQuickWidget / RenderControl).
void Scene3DRenderer::shutdown()
{
    if (!m_needsShutdown)
        return;
    m_needsShutdown = false;
    m_finalFBO.reset();
    m_multisampledFBO.reset();
}

// Render Thread, GUI locked
void Scene3DRenderer::beforeSynchronize()
{
    if (m_window) {
        // Only render if we are sure aspectManager->processFrame was called prior
        // We could otherwise enter a deadlock state
        if (!m_allowRendering.tryAcquire(std::max(m_allowRendering.available(), 1)))
            return;

        // In the case of OnDemand rendering, we still need to get to this
        // point to ensure we have processed jobs for all aspects.
        // We also still need to call render() to allow proceeding with the
        // next frame. However it won't be performing any 3d rendering at all
        // so we do it here and return early. This prevents a costly QtQuick
        // SceneGraph update for nothing
        if (m_skipFrame) {
            m_skipFrame = false;
            ContextSaver saver;
            static_cast<QRenderAspectPrivate*>(QRenderAspectPrivate::get(m_renderAspect))->renderSynchronous(false);
            return;
        }

        m_shouldRender = true;

        // Check size / multisampling
        const QSize currentSize = m_boundingRectSize * m_window->effectiveDevicePixelRatio();
        const bool sizeHasChanged = currentSize != m_lastSize;
        const bool multisampleHasChanged = m_multisample != m_lastMultisample;
        const bool forceRecreate = sizeHasChanged || multisampleHasChanged;
        // Store the current size as a comparison
        // point for the next frame
        m_lastSize = currentSize;
        m_lastMultisample = m_multisample;

        // Rebuild FBO if size/multisampling has changed
        const bool usesFBO = m_compositingMode == Scene3DItem::FBO;
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
                const GLuint textureId = m_finalFBO->texture();
                m_texture.reset(m_window->createTextureFromNativeObject(QQuickWindow::NativeObjectTexture,
                                                                        &textureId, 0,
                                                                        m_finalFBO->size(),
                                                                        QQuickWindow::TextureHasAlphaChannel));
            }

            // We can render either the Scene3D or the Scene3DView but not both
            // at the same time
            Q_ASSERT((m_node == nullptr || m_views.empty()) ||
                     (m_node != nullptr && m_views.empty()) ||
                     (m_node == nullptr && !m_views.empty()));

            // Set texture on node
            if (m_node && (!m_node->texture() || generateNewTexture))
                m_node->setTexture(m_texture.data());

            // Set textures on Scene3DView
            if (m_dirtyViews || generateNewTexture) {
                for (Scene3DView *view : qAsConst(m_views))
                    if (!view->texture() || generateNewTexture)
                        view->setTexture(m_texture.data());
                m_dirtyViews = false;
            }
        }

        // Mark SGNodes as dirty so that QQuick will trigger some rendering
        if (m_node)
            m_node->markDirty(QSGNode::DirtyMaterial);

        for (Scene3DView *view : qAsConst(m_views))
            view->markSGNodeDirty();
    }
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

// Main Thread, Render Thread locked
void Scene3DRenderer::setScene3DViews(const QVector<Scene3DView *> views)
{
    m_views = views;
    m_dirtyViews = true;
}

void Scene3DRenderer::setSGNode(Scene3DSGNode *node)
{
    m_node = node;
}

// Render Thread, Main Thread is unlocked at this point
void Scene3DRenderer::render()
{
    QMutexLocker l(&m_windowMutex);
    // Lock to ensure the window doesn't change while we are rendering
    if (!m_window || !m_shouldRender)
        return;
    m_shouldRender = false;

    ContextSaver saver;

    // The OpenGL state may be dirty from the previous QtQuick nodes, so reset
    // it here to give Qt3D the clean state it expects
    m_window->resetOpenGLState();

    // Create and bind FBO if using the FBO compositing mode
    const bool usesFBO = m_compositingMode == Scene3DItem::FBO;
    if (usesFBO) {
        // Bind FBO
        if (m_multisample) //Only try to use MSAA when available
            m_multisampledFBO->bind();
        else
            m_finalFBO->bind();
    }

    // Render Qt3D Scene
    static_cast<QRenderAspectPrivate*>(QRenderAspectPrivate::get(m_renderAspect))->renderSynchronous(usesFBO);

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
        if (m_node)
            m_node->show();
    }

    // Reset the state used by the Qt Quick scenegraph to avoid any
    // interference when rendering the rest of the UI.
    m_window->resetOpenGLState();
}

} // namespace Qt3DRender

QT_END_NAMESPACE
