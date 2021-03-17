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
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DQuickScene2D/qscene2d.h>
#include <Qt3DRender/qpicktriangleevent.h>
#include <Qt3DRender/qobjectpicker.h>

#include <QtCore/qthread.h>
#include <QtCore/qatomic.h>
#include <QtGui/qevent.h>
#include <QtGui/QOpenGLFunctions>

#include <private/qscene2d_p.h>
#include <private/scene2d_p.h>
#include <private/scene2dmanager_p.h>
#include <private/scene2devent_p.h>
#include <private/texture_p.h>
#include <private/nodemanagers_p.h>
#include <private/resourceaccessor_p.h>
#include <private/attachmentpack_p.h>
#include <private/qt3dquickscene2d_logging_p.h>
#include <private/qbackendnode_p.h>
#include <private/qobjectpicker_p.h>
#include <private/qpickevent_p.h>
#include <private/qpicktriangleevent_p.h>
#include <private/entity_p.h>
#include <private/platformsurfacefilter_p.h>
#include <private/trianglesvisitor_p.h>


QT_BEGIN_NAMESPACE

#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 0x88F0
#endif

using namespace Qt3DRender::Quick;

namespace Qt3DRender {

namespace Render {

namespace Quick {

Q_GLOBAL_STATIC(QThread, renderThread)
Q_GLOBAL_STATIC(QAtomicInt, renderThreadClientCount)

RenderQmlEventHandler::RenderQmlEventHandler(Scene2D *node)
    : QObject()
    , m_node(node)
{
}

// Event handler for the RenderQmlToTexture::renderThread
bool RenderQmlEventHandler::event(QEvent *e)
{
    switch (static_cast<Scene2DEvent::Type>(e->type())) {

    case Scene2DEvent::Render: {
        m_node->render();
        return true;
    }

    case Scene2DEvent::Initialize: {
        m_node->initializeRender();
        return true;
    }

    case Scene2DEvent::Quit: {
        m_node->cleanup();
        return true;
    }

    default:
        break;
    }
    return QObject::event(e);
}

Scene2D::Scene2D()
    : Qt3DRender::Render::BackendNode(Qt3DCore::QBackendNode::ReadWrite)
    , m_context(nullptr)
    , m_shareContext(nullptr)
    , m_renderThread(nullptr)
    , m_sharedObject(nullptr)
    , m_fbo(0)
    , m_rbo(0)
    , m_initialized(false)
    , m_renderInitialized(false)
    , m_mouseEnabled(true)
    , m_renderPolicy(Qt3DRender::Quick::QScene2D::Continuous)
{

}

Scene2D::~Scene2D()
{
    for (auto connection: qAsConst(m_connections))
        QObject::disconnect(connection);
    m_connections.clear();
}

void Scene2D::setOutput(Qt3DCore::QNodeId outputId)
{
    m_outputId = outputId;
}

void Scene2D::initializeSharedObject()
{
    if (!m_initialized) {
        // bail out if we're running autotests
        if (!qgetenv("QT3D_SCENE2D_DISABLE_RENDERING").isEmpty())
            return;

        renderThreadClientCount->fetchAndAddAcquire(1);

        renderThread->setObjectName(QStringLiteral("Scene2D::renderThread"));
        m_renderThread = renderThread;
        m_sharedObject->m_renderThread = m_renderThread;

        // Create event handler for the render thread
        m_sharedObject->m_renderObject = new RenderQmlEventHandler(this);
        m_sharedObject->m_renderObject->moveToThread(m_sharedObject->m_renderThread);
        if (!m_sharedObject->m_renderThread->isRunning())
            m_sharedObject->m_renderThread->start();

        // Notify main thread we have been initialized
        QCoreApplication::postEvent(m_sharedObject->m_renderManager,
                                    new Scene2DEvent(Scene2DEvent::Initialized));
        // Initialize render thread
        QCoreApplication::postEvent(m_sharedObject->m_renderObject,
                                    new Scene2DEvent(Scene2DEvent::Initialize));

        m_initialized = true;
    }
}

void Scene2D::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    Qt3DRender::Render::BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QScene2D *node = qobject_cast<const QScene2D *>(frontEnd);
    if (!node)
        return;
    const QScene2DPrivate *dnode = static_cast<const QScene2DPrivate *>(QScene2DPrivate::get(node));

    if (m_mouseEnabled != node->isMouseEnabled()) {
        m_mouseEnabled = node->isMouseEnabled();
        if (!firstTime && m_mouseEnabled && m_cachedPickEvent) {
            handlePickEvent(QEvent::MouseButtonPress, m_cachedPickEvent.data());
            m_cachedPickEvent.clear();
        }
    }

    m_renderPolicy = node->renderPolicy();
    auto id = Qt3DCore::qIdForNode(node->output());
    if (id != m_outputId)
        setOutput(id);

    auto ids = Qt3DCore::qIdsForNodes(node->entities());
    std::sort(std::begin(ids), std::end(ids));
    Qt3DCore::QNodeIdVector addedEntities;
    Qt3DCore::QNodeIdVector removedEntities;
    std::set_difference(std::begin(ids), std::end(ids),
                        std::begin(m_entities), std::end(m_entities),
                        std::inserter(addedEntities, addedEntities.end()));
    std::set_difference(std::begin(m_entities), std::end(m_entities),
                        std::begin(ids), std::end(ids),
                        std::inserter(removedEntities, removedEntities.end()));
    for (const auto &id: addedEntities) {
        Qt3DCore::QEntity *entity = qobject_cast<Qt3DCore::QEntity *>(dnode->m_scene->lookupNode(id));
        if (!entity)
            return;

        if (registerObjectPickerEvents(entity))
            m_entities.push_back(id);
        else
            Qt3DCore::QNodePrivate::get(const_cast<Qt3DCore::QNode *>(frontEnd))->update();
    }
    for (const auto &id: removedEntities) {
        m_entities.removeOne(id);
        unregisterObjectPickerEvents(id);
    }
    std::sort(std::begin(m_entities), std::end(m_entities));

    if (firstTime)
        setSharedObject(dnode->m_renderManager->m_sharedObject);
}

void Scene2D::setSharedObject(Qt3DRender::Quick::Scene2DSharedObjectPtr sharedObject)
{
    m_sharedObject = sharedObject;
    if (!m_initialized)
        initializeSharedObject();
}

void Scene2D::initializeRender()
{
    if (!m_renderInitialized && m_sharedObject.data() != nullptr) {
       m_shareContext = renderer()->shareContext();
        if (!m_shareContext){
            qCDebug(Qt3DRender::Quick::Scene2D) << Q_FUNC_INFO << "Renderer not initialized.";
            QCoreApplication::postEvent(m_sharedObject->m_renderObject,
                                        new Scene2DEvent(Scene2DEvent::Initialize));
            return;
        }
        m_context = new QOpenGLContext();
        m_context->setFormat(m_shareContext->format());
        m_context->setShareContext(m_shareContext);
        m_context->create();

        m_context->makeCurrent(m_sharedObject->m_surface);
        m_sharedObject->m_renderControl->initialize(m_context);
#ifdef QT_OPENGL_ES_2_ANGLE
        m_usingAngle = false;
        if (m_context->isOpenGLES()) {
            const char *versionStr = reinterpret_cast<const char *>(
                                                m_context->functions()->glGetString(GL_VERSION));
            if (strstr(versionStr, "ANGLE"))
                m_usingAngle = true;
        }
#endif
        m_context->doneCurrent();

        QCoreApplication::postEvent(m_sharedObject->m_renderManager,
                                    new Scene2DEvent(Scene2DEvent::Prepare));
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
        m_sharedObject->wake();
    }
}

void Scene2D::render()
{
    if (m_initialized && m_renderInitialized && m_sharedObject.data() != nullptr) {

        QMutexLocker lock(&m_sharedObject->m_mutex);

        QOpenGLTexture *texture = nullptr;
        const Qt3DRender::Render::Attachment *attachmentData = nullptr;
        QMutex *textureLock = nullptr;

#ifdef QT_OPENGL_ES_2_ANGLE
        QScopedPointer<SurfaceLocker> surfaceLocker;
        if (m_usingAngle)
            surfaceLocker.reset(new SurfaceLocker(m_sharedObject->m_surface));
#endif
        m_context->makeCurrent(m_sharedObject->m_surface);

        if (resourceAccessor()->accessResource(RenderBackendResourceAccessor::OutputAttachment,
                                               m_outputId, (void**)&attachmentData, nullptr)) {
            if (!resourceAccessor()->accessResource(RenderBackendResourceAccessor::OGLTextureWrite,
                                                    attachmentData->m_textureUuid,
                                                       (void**)&texture, &textureLock)) {
                // Need to call sync even if the texture is not in use
                syncRenderControl();
                m_context->doneCurrent();
                qCDebug(Qt3DRender::Quick::Scene2D) << Q_FUNC_INFO << "Texture not in use.";
                QCoreApplication::postEvent(m_sharedObject->m_renderObject,
                                            new Scene2DEvent(Scene2DEvent::Render));
                return;
            }
#ifdef QT_OPENGL_ES_2_ANGLE
            if (m_usingAngle == false)
                textureLock->lock();
#else
            textureLock->lock();
#endif
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
                    qCWarning(Qt3DRender::Quick::Scene2D) << Q_FUNC_INFO << "Fbo not initialized.";
                    return;
                }
            }
        }

        if (m_fbo != m_sharedObject->m_quickWindow->renderTargetId())
            m_sharedObject->m_quickWindow->setRenderTarget(m_fbo, m_textureSize);

        // Call disallow rendering while mutex is locked
        if (m_renderPolicy == QScene2D::SingleShot)
            m_sharedObject->disallowRender();

        // Sync
        if (m_sharedObject->isSyncRequested()) {

            m_sharedObject->clearSyncRequest();

            m_sharedObject->m_renderControl->sync();
        }

        // Render
        m_sharedObject->m_renderControl->render();

        // Tell main thread we are done so it can begin cleanup if this is final frame
        if (m_renderPolicy == QScene2D::SingleShot)
            QCoreApplication::postEvent(m_sharedObject->m_renderManager,
                                        new Scene2DEvent(Scene2DEvent::Rendered));

        m_sharedObject->m_quickWindow->resetOpenGLState();
        m_context->functions()->glFlush();
        if (texture->isAutoMipMapGenerationEnabled())
            texture->generateMipMaps();
#ifdef QT_OPENGL_ES_2_ANGLE
        if (m_usingAngle == false)
            textureLock->unlock();
#else
        textureLock->unlock();
#endif
        m_context->doneCurrent();

        // gui thread can now continue
        m_sharedObject->wake();
    }
}

// this function gets called while the main thread is waiting
void Scene2D::cleanup()
{
    if (m_renderInitialized && m_initialized) {
        m_context->makeCurrent(m_sharedObject->m_surface);
        m_sharedObject->m_renderControl->invalidate();
        m_context->functions()->glDeleteFramebuffers(1, &m_fbo);
        m_context->functions()->glDeleteRenderbuffers(1, &m_rbo);
        m_context->doneCurrent();
        m_renderInitialized = false;
    }
    if (m_initialized) {
        delete m_sharedObject->m_renderObject;
        m_sharedObject->m_renderObject = nullptr;
        delete m_context;
        m_context = nullptr;
        m_initialized = false;
    }
    if (m_sharedObject) {
        // wake up the main thread
        m_sharedObject->wake();
        m_sharedObject = nullptr;
    }
    if (m_renderThread) {
        renderThreadClientCount->fetchAndSubAcquire(1);
        if (renderThreadClientCount->loadRelaxed() == 0)
            renderThread->quit();
    }
}


bool Scene2D::registerObjectPickerEvents(Qt3DCore::QEntity *qentity)
{
    Entity *entity = nullptr;
    if (!resourceAccessor()->accessResource(RenderBackendResourceAccessor::EntityHandle,
                                            qentity->id(), (void**)&entity, nullptr)) {
        qCWarning(Qt3DRender::Quick::Scene2D) << Q_FUNC_INFO
                                              << "Entity not yet available in backend";
        return false;
    }

    if (!entity->containsComponentsOfType<ObjectPicker>() ||
        !entity->containsComponentsOfType<GeometryRenderer>()) {
        qCWarning(Qt3DRender::Quick::Scene2D) << Q_FUNC_INFO
            << "Entity does not contain required components: ObjectPicker and GeometryRenderer";
        return false;
    }

    QObjectPicker *picker = qentity->componentsOfType<QObjectPicker>().front();
    m_connections << QObject::connect(picker, &QObjectPicker::pressed, qentity, [this](Qt3DRender::QPickEvent *pick) {
        handlePickEvent(QEvent::MouseButtonPress, pick);
    });
    m_connections << QObject::connect(picker, &QObjectPicker::released, qentity, [this](Qt3DRender::QPickEvent *pick) {
        handlePickEvent(QEvent::MouseButtonRelease, pick);
    });
    m_connections << QObject::connect(picker, &QObjectPicker::moved, qentity, [this](Qt3DRender::QPickEvent *pick) {
        handlePickEvent(QEvent::MouseMove, pick);
    });

    Qt3DCore::QBackendNodePrivate *priv = Qt3DCore::QBackendNodePrivate::get(this);
    Qt3DCore::QChangeArbiter *arbiter = static_cast<Qt3DCore::QChangeArbiter*>(priv->m_arbiter);
    arbiter->registerObserver(d_ptr, entity->componentUuid<ObjectPicker>());
    return true;
}

void Scene2D::unregisterObjectPickerEvents(Qt3DCore::QNodeId entityId)
{
    Entity *entity = nullptr;
    if (!resourceAccessor()->accessResource(RenderBackendResourceAccessor::EntityHandle,
                                            entityId, (void**)&entity, nullptr))
        return;

    Qt3DCore::QBackendNodePrivate *priv = Qt3DCore::QBackendNodePrivate::get(this);
    Qt3DCore::QChangeArbiter *arbiter = static_cast<Qt3DCore::QChangeArbiter*>(priv->m_arbiter);
    arbiter->unregisterObserver(d_ptr, entity->componentUuid<ObjectPicker>());
}

void Scene2D::handlePickEvent(int type, const Qt3DRender::QPickEvent *ev)
{
    if (!isEnabled())
        return;
    if (m_mouseEnabled) {
        const QPickTriangleEvent *pickTriangle = static_cast<const QPickTriangleEvent *>(ev);
        Q_ASSERT(pickTriangle->entity());
        Entity *entity = nullptr;
        if (!resourceAccessor()->accessResource(RenderBackendResourceAccessor::EntityHandle,
                                                Qt3DCore::qIdForNode(pickTriangle->entity()),
                                                (void**)&entity, nullptr))
            return;

        CoordinateReader reader(renderer()->nodeManagers());
        if (reader.setGeometry(entity->renderComponent<GeometryRenderer>(),
                               QAttribute::defaultTextureCoordinateAttributeName())) {
            Vector4D c0 = reader.getCoordinate(pickTriangle->vertex1Index());
            Vector4D c1 = reader.getCoordinate(pickTriangle->vertex2Index());
            Vector4D c2 = reader.getCoordinate(pickTriangle->vertex3Index());
            Vector4D ci = c0 * pickTriangle->uvw().x()
                           + c1 * pickTriangle->uvw().y() + c2 * pickTriangle->uvw().z();
            ci.setW(1.0f);

            const QSize size = m_sharedObject->m_quickWindow->size();
            QPointF pos = QPointF(ci.x() * size.width(), (1.0f - ci.y()) * size.height());
            QMouseEvent *mouseEvent
                    = new QMouseEvent(static_cast<QEvent::Type>(type),
                                      pos, pos, pos,
                                      static_cast<Qt::MouseButton>(pickTriangle->button()),
                                      static_cast<Qt::MouseButtons>(pickTriangle->buttons()),
                                      static_cast<Qt::KeyboardModifiers>(pickTriangle->modifiers()),
                                      Qt::MouseEventSynthesizedByApplication);

            QCoreApplication::postEvent(m_sharedObject->m_quickWindow, mouseEvent);
        }
    } else if (type == QEvent::MouseButtonPress) {
        const QPickTriangleEvent *pickTriangle = static_cast<const QPickTriangleEvent *>(ev);
        const QPickTriangleEventPrivate *dpick = QPickTriangleEventPrivate::get(pickTriangle);
        m_cachedPickEvent = QPickEventPtr(dpick->clone());
    } else {
        m_cachedPickEvent.clear();
    }
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
