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

#include <Qt3DRender/qscene2d.h>
#include <Qt3DRender/private/qscene2d_p.h>
#include <Qt3DRender/private/scene2d_p.h>
#include <Qt3DRender/private/graphicscontext_p.h>
#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

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
    : FrameGraphNode(FrameGraphNode::InvalidNodeType)
    , m_context(nullptr)
    , m_sharedObject(nullptr)
    , m_renderThread(nullptr)
    , m_graphicsContext(nullptr)
    , m_texture(nullptr)
    , m_initialized(false)
    , m_renderInitialized(false)
    , m_renderOnce(false)
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

void Scene2D::setTexture(Qt3DCore::QNodeId textureId)
{
    m_textureId = textureId;
    attach();
    checkInitialized();
}

void Scene2D::checkInitialized()
{
    if (!m_initialized && m_textureId != Qt3DCore::QNodeId()) {

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
    m_renderOnce = m_renderOnce;
    setSharedObject(data.sharedObject);
    setTexture(data.textureId);
}

void Scene2D::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::PropertyUpdated) {
        Qt3DCore::QPropertyUpdatedChangePtr propertyChange
                = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("enabled"))
            setEnabled(propertyChange->value().toBool());
        else if (propertyChange->propertyName() == QByteArrayLiteral("dirty")) {
            // sent to trigger backend update when the texture gets rendered
            // so do nothing here
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("renderOnce"))
            m_renderOnce = propertyChange->value().toBool();
        else if (propertyChange->propertyName() == QByteArrayLiteral("texture")) {
            Qt3DCore::QNodeId textureId = propertyChange->value().value<Qt3DCore::QNodeId>();
            setTexture(textureId);
        }
        markDirty(AbstractRenderer::AllDirty);
    }
    FrameGraphNode::sceneChangeEvent(e);
}

void Scene2D::setSharedObject(Qt3DRender::Scene2DSharedObjectPtr sharedObject)
{
    m_sharedObject = sharedObject;
}

void Scene2D::initializeRender()
{
    if (!m_renderInitialized) {
        Qt3DRender::Render::Renderer *renderer
                = static_cast<Qt3DRender::Render::Renderer *>(this->renderer());
        if (!renderer)
            return;

        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);

        m_context = new QOpenGLContext();
        m_context->setFormat(format);

        m_context->setShareContext(renderer->shareContext());
        m_context->create();

        m_graphicsContext = new GraphicsContext();
        m_graphicsContext->setOpenGLContext(m_context);
        m_graphicsContext->setRenderer(renderer);

        m_graphicsContext->makeCurrent(m_sharedObject->m_surface);
        m_sharedObject->m_renderControl->initialize(m_context);
        m_graphicsContext->doneCurrent();

        QCoreApplication::postEvent(m_sharedObject->m_renderManager, new QEvent(PREPARE));
        m_renderInitialized = true;
    }
}

void Scene2D::attach()
{
    m_attachments = AttachmentPack();
    Attachment attach;
    attach.m_mipLevel = 0;
    attach.m_textureUuid = m_textureId;
    attach.m_point = QRenderTargetOutput::AttachmentPoint::Color0;

//    m_attachments.addAttachment(attach);
}

void Scene2D::render()
{
    if (m_initialized && m_sharedObject && this->isEnabled()) {

        QMutexLocker lock(&m_sharedObject->m_mutex);

        // Lookup backend texture
        if (m_texture == nullptr) {
            m_texture = renderer()->nodeManagers()->textureManager()->lookupResource(m_textureId);
            if (!m_texture) {
                qCDebug(Render::Framegraph) << Q_FUNC_INFO << "Texture not set";
                return;
            }
        }

        m_graphicsContext->makeCurrent(m_sharedObject->m_surface);

        // Don't create the OpenGL texture in this thread.
        const bool canUseTexture = !m_texture->isTextureReset();

        if (canUseTexture) {
            // Activate fbo for the texture
            QOpenGLTexture *glTex = m_texture->getOrCreateGLTexture();
            const QSize textureSize = QSize(glTex->width(), glTex->height());

            GLuint fbo = 0; //m_graphicsContext->activateRenderTargetForQmlRender(this, m_attachments, 0);

            if (fbo != m_sharedObject->m_quickWindow->renderTargetId())
                m_sharedObject->m_quickWindow->setRenderTarget(fbo, textureSize);

            m_texture->textureLock()->lock();
        }
        // Call disallow rendering while mutex is locked
        if (canUseTexture && m_renderOnce)
            m_sharedObject->disallowRender();

        // Need to call sync even if the texture is not in use
        if (m_sharedObject->isSyncRequested()) {

            m_sharedObject->clearSyncRequest();

            m_sharedObject->m_renderControl->sync();

            // gui thread can now continue
            m_sharedObject->wakeWaiting();
            lock.unlock();
        }

        if (canUseTexture) {

            // Render
            m_sharedObject->m_renderControl->render();

            // Tell main thread we are done so it can begin cleanup
            if (m_renderOnce)
                QCoreApplication::postEvent(m_sharedObject->m_renderManager, new QEvent(RENDERED));

            m_sharedObject->m_quickWindow->resetOpenGLState();
            m_context->functions()->glFlush();
            m_texture->textureLock()->unlock();
        }
        m_graphicsContext->doneCurrent();
    }
}

void Scene2D::cleanup()
{
    if (m_renderInitialized && m_initialized) {
        m_context->makeCurrent(m_sharedObject->m_surface);
        m_sharedObject->m_renderControl->invalidate();
        m_context->doneCurrent();
        m_sharedObject->m_renderThread->quit();
        delete m_sharedObject->m_renderObject;
        m_sharedObject->m_renderObject = nullptr;
        delete m_context;
        m_context = nullptr;
        m_sharedObject = nullptr;
        delete m_graphicsContext;
        m_graphicsContext = nullptr;
        m_renderInitialized = false;
        m_initialized = false;
    }
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
