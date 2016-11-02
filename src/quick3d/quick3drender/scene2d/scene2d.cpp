/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DQuickRender/qscene2d.h>

#include <private/qscene2d_p.h>
#include <private/scene2d_p.h>
#include <private/graphicscontext_p.h>
#include <private/texture_p.h>
#include <private/nodemanagers_p.h>
#include <private/resourceaccessor_p.h>
#include <private/attachmentpack_p.h>

QT_BEGIN_NAMESPACE

#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 0x88F0
#endif

using namespace Qt3DRender::Quick;

namespace Qt3DRender {

namespace Render {

namespace Quick {

RenderQmlEventHandler::RenderQmlEventHandler(Scene2D *node)
    : QObject()
    , m_node(node)
{
}

// Event handler for the RenderQmlToTexture::renderThread
bool RenderQmlEventHandler::event(QEvent *e)
{
    switch (e->type()) {

    case RENDER: {
        m_node->render();
        return true;
    }

    case INITIALIZE: {
        m_node->initializeRender();
        return true;
    }

    case QUIT: {
        m_node->cleanup();
        return true;
    }

    default:
        break;
    }
    return QObject::event(e);
}

Scene2D::Scene2D()
    : m_context(nullptr)
    , m_shareContext(nullptr)
    , m_sharedObject(nullptr)
    , m_renderThread(nullptr)
    , m_initialized(false)
    , m_renderInitialized(false)
    , m_renderOnce(false)
    , m_fbo(0)
    , m_rbo(0)
{

}

Scene2D::~Scene2D()
{
    // this gets called from aspect thread. Wait for the render thread then delete it.
    if (m_renderThread) {
        m_renderThread->wait(1000);
        delete m_renderThread;
    }
}

void Scene2D::setOutput(Qt3DCore::QNodeId outputId)
{
    m_outputId = outputId;
}

void Scene2D::initializeSharedObject()
{
    if (!m_initialized) {

        // Create render thread
        m_renderThread = new QThread();
        m_renderThread->setObjectName(QStringLiteral("Scene2D::renderThread"));
        m_sharedObject->m_renderThread = m_renderThread;

        // Create event handler for the render thread
        m_sharedObject->m_renderObject = new RenderQmlEventHandler(this);
        m_sharedObject->m_renderObject->moveToThread(m_sharedObject->m_renderThread);
        m_sharedObject->m_renderThread->start();

        // Notify main thread we have been initialized
        QCoreApplication::postEvent(m_sharedObject->m_renderManager, new QEvent(INITIALIZED));

        // Initialize render thread
        QCoreApplication::postEvent(m_sharedObject->m_renderObject, new QEvent(INITIALIZE));

        m_initialized = true;
    }
}

void Scene2D::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QScene2DData>>(change);
    const auto &data = typedChange->data;
    m_renderOnce = data.renderOnce;
    setSharedObject(data.sharedObject);
    setOutput(data.output);
    m_shareContext = renderer()->shareContext();
    m_accessor = resourceAccessor();
}

void Scene2D::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::PropertyUpdated) {
        Qt3DCore::QPropertyUpdatedChangePtr propertyChange
                = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("renderOnce"))
            m_renderOnce = propertyChange->value().toBool();
        else if (propertyChange->propertyName() == QByteArrayLiteral("output")) {
            Qt3DCore::QNodeId outputId = propertyChange->value().value<Qt3DCore::QNodeId>();
            setOutput(outputId);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("sharedObject")) {
            const Scene2DSharedObjectPtr sharedObject
                    = propertyChange->value().value<Scene2DSharedObjectPtr>();
            setSharedObject(sharedObject);
        }
    }
}

void Scene2D::setSharedObject(Qt3DRender::Quick::Scene2DSharedObjectPtr sharedObject)
{
    m_sharedObject = sharedObject;
    if (!m_initialized)
        initializeSharedObject();
}

void Scene2D::initializeRender()
{
    if (!m_renderInitialized) {

        Q_ASSERT(m_shareContext);

        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);

        m_context = new QOpenGLContext();
        m_context->setFormat(format);
        m_context->setShareContext(m_shareContext);
        m_context->create();

        m_context->makeCurrent(m_sharedObject->m_surface);
        m_sharedObject->m_renderControl->initialize(m_context);
        m_context->doneCurrent();

        QCoreApplication::postEvent(m_sharedObject->m_renderManager, new QEvent(PREPARE));
        m_renderInitialized = true;
    }
}

bool Scene2D::updateFbo(QOpenGLTexture *texture)
{
    QOpenGLFunctions *gl = m_context->functions();
    if (m_fbo == 0) {
        gl->glGenFramebuffers(1, &m_fbo);
        gl->glGenRenderbuffers(1, &m_rbo);
    }
    // TODO: Add another codepath when GL_DEPTH24_STENCIL8 is not supported
    gl->glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    gl->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                              m_textureSize.width(), m_textureSize.height());
    gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    gl->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, texture->textureId(), 0);
    gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    GLenum status = gl->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (status != GL_FRAMEBUFFER_COMPLETE)
        return false;
    return true;
}

void Scene2D::syncRenderControl()
{
    if (m_sharedObject->isSyncRequested()) {

        m_sharedObject->clearSyncRequest();

        m_sharedObject->m_renderControl->sync();

        // gui thread can now continue
        m_sharedObject->wakeWaiting();
    }
}

void Scene2D::render()
{
    if (m_initialized && m_renderInitialized && m_sharedObject.data() != nullptr) {

        QMutexLocker lock(&m_sharedObject->m_mutex);

        QOpenGLTexture *texture = nullptr;
        const Qt3DRender::Render::Attachment *attachmentData = nullptr;
        QMutex *textureLock = nullptr;

        m_context->makeCurrent(m_sharedObject->m_surface);
        if (m_accessor->accessResource(m_outputId, (void**)&attachmentData, nullptr)) {
            if (!m_accessor->accessResource(attachmentData->m_textureUuid, (void**)&texture, &textureLock)) {
                // Need to call sync even if the texture is not in use
                syncRenderControl();
                m_context->doneCurrent();
                // TODO: add logging category for scene2d
                //qCWarning(Scene2D) << "Texture not in use.";
                return;
            }
            textureLock->lock();
            const QSize textureSize = QSize(texture->width(), texture->height());
            if (m_attachmentData.m_textureUuid != attachmentData->m_textureUuid
                || m_attachmentData.m_point != attachmentData->m_point
                || m_attachmentData.m_face != attachmentData->m_face
                || m_attachmentData.m_layer != attachmentData->m_layer
                || m_attachmentData.m_mipLevel != attachmentData->m_mipLevel
                || m_textureSize != textureSize) {
                m_textureSize = textureSize;
                m_attachmentData = *attachmentData;
                if (!updateFbo(texture)) {
                    // Need to call sync even if the fbo is not usable
                    syncRenderControl();
                    textureLock->unlock();
                    m_context->doneCurrent();
                    //qCWarning(Scene2D) << "Fbo not initialized.";
                    return;
                }
            }
        }

        if (m_fbo != m_sharedObject->m_quickWindow->renderTargetId())
            m_sharedObject->m_quickWindow->setRenderTarget(m_fbo, m_textureSize);

        // Call disallow rendering while mutex is locked
        if (m_renderOnce)
            m_sharedObject->disallowRender();

        // Sync
        syncRenderControl();

        // The lock is not needed anymore so release it before the following
        // time comsuming operations
        lock.unlock();

        // Render
        m_sharedObject->m_renderControl->render();

        // Tell main thread we are done so it can begin cleanup if this is final frame
        if (m_renderOnce)
            QCoreApplication::postEvent(m_sharedObject->m_renderManager, new QEvent(RENDERED));

        m_sharedObject->m_quickWindow->resetOpenGLState();
        m_context->functions()->glFlush();
        if (texture->isAutoMipMapGenerationEnabled())
            texture->generateMipMaps();
        textureLock->unlock();
        m_context->doneCurrent();
    }
}

void Scene2D::cleanup()
{
    if (m_renderInitialized && m_initialized) {
        m_context->makeCurrent(m_sharedObject->m_surface);
        m_sharedObject->m_renderControl->invalidate();
        m_context->functions()->glDeleteFramebuffers(1, &m_fbo);
        m_context->functions()->glDeleteRenderbuffers(1, &m_rbo);
        m_context->doneCurrent();
        m_sharedObject->m_renderThread->quit();
        delete m_sharedObject->m_renderObject;
        m_sharedObject->m_renderObject = nullptr;
        delete m_context;
        m_context = nullptr;
        m_sharedObject = nullptr;
        m_renderInitialized = false;
        m_initialized = false;
    }
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
