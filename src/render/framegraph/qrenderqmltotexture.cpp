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

#include "qrenderqmltotexture.h"
#include "qrenderqmltotexture_p.h"

#include <Qt3DCore/QPropertyUpdatedChange>

#include <QtGui/QOpenGLContext>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {


/*!
    \class Qt3DRender::QRenderQmlToTexture
    \inmodule Qt3DRender

    \brief This class enables rendering qml into a texture, which then can be
    used as a part of 3D scene.

    The component uses QQuickRenderControl to render the given QML source into an
    offscreen surface, which is attached to a texture provided by the user. This allows the
    component to directly render into the texture without intermediate copy and the user to
    freely specify how the texture is used in the 3D scene.

    \since 5.9
*/

/*!
    \qmltype RenderQmlToTexture
    \inqmlmodule Qt3D.Render
    \since 5.9
    \inherits
    \instantiates Qt3DRender::QRenderQmlToTexture
    \brief RenderQmlToTexture
 */

/*!
    \qmlproperty Qt3DRender::QAbstractTexture Qt3D.Render::RenderQmlToTexture::texture
    Holds the texture being rendered to.
 */

/*!
    \qmlproperty QUrl Qt3D.Render::RenderQmlToTexture::source
    Holds the qml source url.
 */

/*!
    \qmlproperty bool Qt3D.Render::RenderQmlToTexture::renderOnce
    Holds whether the first rendered image to the texture is also the last, after which the
    renderer releases resources needed for the rendering and the rendering is no longer possible
    with this RenderQmlToTexture object.
 */

/*!
    \qmlproperty bool Qt3D.Render::RenderQmlToTexture::loaded
     Hold whether the source has been loaded.
 */

class RenderControl : public QQuickRenderControl
{
public:
    RenderControl(QWindow *w) : m_window(w) { }
    QWindow *renderWindow(QPoint *offset) Q_DECL_OVERRIDE;

private:
    QWindow *m_window;
};

QWindow *RenderControl::renderWindow(QPoint *offset)
{
    if (offset)
        *offset = QPoint(0, 0);
    return m_window;
}

/*!
    \internal
     Constructs object shared by the front-end and back-end to synchronize the rendering.
 */
RenderQmlToTextureSharedObject::RenderQmlToTextureSharedObject(RenderQmlToTextureManager *manager)
    : m_quit(false)
    , m_requestSync(false)
    , m_prepared(false)
    , m_initialized(false)
    , m_renderControl(nullptr)
    , m_quickWindow(nullptr)
    , m_renderManager(manager)
    , m_surface(nullptr)
    , m_renderObject(nullptr)
    , m_disallowed(false)
{
}

RenderQmlToTextureSharedObject::~RenderQmlToTextureSharedObject()
{
}

void RenderQmlToTextureSharedObject::cleanup()
{
    delete m_renderControl;
    delete m_quickWindow;
    delete m_surface;
    m_renderControl = nullptr;
    m_quickWindow = nullptr;
    m_surface = nullptr;
    m_initialized = false;
}

bool RenderQmlToTextureSharedObject::canRender() const
{
    return m_initialized && m_prepared && !m_disallowed;
}

bool RenderQmlToTextureSharedObject::isInitialized() const
{
    return m_initialized;
}

void RenderQmlToTextureSharedObject::disallowRender()
{
    m_disallowed = true;
}

void RenderQmlToTextureSharedObject::setInitialized()
{
    m_initialized = true;
}

bool RenderQmlToTextureSharedObject::isPrepared() const
{
    return m_prepared;
}

void RenderQmlToTextureSharedObject::setPrepared()
{
    m_prepared = true;
}

// not protected, call only from main thread
bool RenderQmlToTextureSharedObject::isQuit() const
{
    Q_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());
    return m_quit;
}

// not protected, call only from main thread
void RenderQmlToTextureSharedObject::requestQuit()
{
    Q_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());
    m_quit = true;
    QCoreApplication::postEvent(m_renderObject, new QEvent(QUIT));
}

bool RenderQmlToTextureSharedObject::isSyncRequested() const
{
    return m_requestSync;
}

void RenderQmlToTextureSharedObject::requestRender(bool sync)
{
    m_requestSync = sync;
    QCoreApplication::postEvent(m_renderObject, new QEvent(RENDER));
}

void RenderQmlToTextureSharedObject::waitRender()
{
    m_cond.wait(&m_mutex);
}

void RenderQmlToTextureSharedObject::wakeWaiting()
{
    m_cond.wakeOne();
}

void RenderQmlToTextureSharedObject::clearSyncRequest()
{
    m_requestSync = false;
}

/*!
    \internal
     Constructs qml render manager.
 */
RenderQmlToTextureManager::RenderQmlToTextureManager(QRenderQmlToTexturePrivate *priv)
    : m_priv(priv)
    , m_qmlEngine(nullptr)
    , m_qmlComponent(nullptr)
    , m_rootItem(nullptr)
    , m_source(nullptr)
    , m_texture(nullptr)
    , m_requested(false)
    , m_initialized(false)
    , m_renderSyncRequested(false)
    , m_sharedObject(new RenderQmlToTextureSharedObject(this))
    , m_renderOnce(false)
    , m_backendInitialized(false)
{
    setFormat(QSurfaceFormat::defaultFormat());

    m_sharedObject->m_surface = new QOffscreenSurface;
    m_sharedObject->m_surface->setFormat(QSurfaceFormat::defaultFormat());
    m_sharedObject->m_surface->create();

    // Create render control
    m_sharedObject->m_renderControl = new RenderControl(this);

    // Create window to render the QML with
    m_sharedObject->m_quickWindow = new QQuickWindow(m_sharedObject->m_renderControl);
    m_sharedObject->m_quickWindow->setClearBeforeRendering(true);
    m_sharedObject->m_quickWindow->setDefaultAlphaBuffer(true);

    // Create a QML engine.
    m_qmlEngine = new QQmlEngine;
    if (!m_qmlEngine->incubationController())
        m_qmlEngine->setIncubationController(m_sharedObject->m_quickWindow->incubationController());

    connect(m_sharedObject->m_renderControl, &QQuickRenderControl::renderRequested,
            this, &RenderQmlToTextureManager::requestRender);
    connect(m_sharedObject->m_renderControl, &QQuickRenderControl::sceneChanged,
            this, &RenderQmlToTextureManager::requestRenderSync);
}

RenderQmlToTextureManager::~RenderQmlToTextureManager()
{
    m_sharedObject = nullptr;
}

void RenderQmlToTextureManager::requestRender()
{
    // Don't request render until the backend is initialized.
    if (m_sharedObject->canRender()) {
        if (!m_requested) {
            m_requested = true;
            QCoreApplication::postEvent(this, new QEvent(RENDER));
        }
    }
}

void RenderQmlToTextureManager::requestRenderSync()
{
    // Don't request render until the backed is initialized.
    if (m_sharedObject->canRender()) {
        if (!m_requested) {
            m_requested = true;
            QCoreApplication::postEvent(this, new QEvent(RENDERSYNC));
        }
    } else {
        m_renderSyncRequested = true;
    }
}

void RenderQmlToTextureManager::startIfInitialized()
{
    if (!m_initialized) {
        if (m_backendInitialized && m_source.isValid()) {
            m_qmlComponent = new QQmlComponent(m_qmlEngine, m_source);
            if (m_qmlComponent->isLoading()) {
                connect(m_qmlComponent, &QQmlComponent::statusChanged,
                        this, &RenderQmlToTextureManager::run);
            } else {
                run();
            }
        }
    }
}

void RenderQmlToTextureManager::stopAndClean()
{
    if (m_sharedObject->isInitialized()) {
        QMutexLocker lock(&m_sharedObject->m_mutex);
        m_sharedObject->requestQuit();
        m_sharedObject->m_renderThread->wait();
        m_sharedObject->cleanup();
        delete m_qmlEngine;
        delete m_qmlComponent;
        m_qmlEngine = nullptr;
        m_qmlComponent = nullptr;
    }
}

void RenderQmlToTextureManager::run()
{
    disconnect(m_qmlComponent, &QQmlComponent::statusChanged, this, &RenderQmlToTextureManager::run);

    if (m_qmlComponent->isError()) {
        QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error: errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }

    QObject *rootObject = m_qmlComponent->create();
    if (m_qmlComponent->isError()) {
        QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error: errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }

    m_rootItem = qobject_cast<QQuickItem *>(rootObject);
    if (!m_rootItem) {
        qWarning("QRenderQmlToTexture: Root item is not a QQuickItem.");
        delete rootObject;
        return;
    }

    // The root item is ready. Associate it with the window.
    m_rootItem->setParentItem(m_sharedObject->m_quickWindow->contentItem());

    // Update window size.
    updateSizes();

    m_initialized = true;
    m_sharedObject->setInitialized();

    emit onLoadedChanged();
}

void RenderQmlToTextureManager::updateSizes()
{
    const int width = m_rootItem->width();
    const int height = m_rootItem->height();
    if (width == 0 || height == 0) {
        qWarning() << "QRenderQmlToTexture: Root item size not set.";
        return;
    }
    resize(width, height);
    m_sharedObject->m_quickWindow->setGeometry(0, 0, width, height);
}

void RenderQmlToTextureManager::setTexture(QAbstractTexture *texture)
{
    m_texture = texture;
    startIfInitialized();
}

void RenderQmlToTextureManager::setSource(const QUrl &url)
{
    m_source = url;
    startIfInitialized();
}

bool RenderQmlToTextureManager::event(QEvent *e)
{
    switch (e->type()) {

    case RENDER: {
        // just render request, don't need to call sync in render thread
        QMutexLocker lock(&m_sharedObject->m_mutex);
        m_sharedObject->requestRender(false);

        Qt3DCore::QPropertyUpdatedChangePtr change(new Qt3DCore::QPropertyUpdatedChange(m_priv->m_id));
        change->setPropertyName("dirty");
        change->setValue(QVariant::fromValue(true));
        m_priv->notifyObservers(change);

        m_requested = false;
        return true;
    }

    case RENDERSYNC: {
        // sync and render request, main and render threads must be synchronized
        if (!m_sharedObject->isQuit())
            doRenderSync();
        m_requested = false;
        return true;
    }

    case PREPARE: {
        m_sharedObject->m_renderControl->prepareThread(m_sharedObject->m_renderThread);
        m_sharedObject->setPrepared();

        if (m_renderSyncRequested) {
            if (!m_requested) {
                m_requested = true;
                QCoreApplication::postEvent(this, new QEvent(RENDERSYNC));
            }
            m_renderSyncRequested = false;
        }
        return true;
    }

    case INITIALIZED: {
        // backend is initialized, start the qml
        m_backendInitialized = true;
        startIfInitialized();
        return true;
    }

    case RENDERED: {
        // render is done, excellent, now clean anything not needed anymore.
        stopAndClean();
        return true;
    }

    default:
        break;
    }
    return QWindow::event(e);
}

void RenderQmlToTextureManager::doRenderSync()
{
    QMutexLocker lock(&m_sharedObject->m_mutex);

    m_sharedObject->requestRender(true);
    m_sharedObject->m_renderControl->polishItems();

    Qt3DCore::QPropertyUpdatedChangePtr change(new Qt3DCore::QPropertyUpdatedChange(m_priv->m_id));

    change->setPropertyName("dirty");
    change->setValue(QVariant::fromValue(true));
    m_priv->notifyObservers(change);

    // begin waiting render thread
    m_sharedObject->waitRender();
    m_requested = false;
}

void RenderQmlToTextureManager::cleanup()
{
    stopAndClean();
}


QRenderQmlToTexturePrivate::QRenderQmlToTexturePrivate()
    : QFrameGraphNodePrivate()
    , m_renderManager(new RenderQmlToTextureManager(this))
{
}

QRenderQmlToTexturePrivate::~QRenderQmlToTexturePrivate()
{
    m_renderManager->cleanup();
    delete m_renderManager;
}


RenderQmlToTextureSharedObject *QRenderQmlToTexturePrivate::getSharedObject(QRenderQmlToTexture *rqtt)
{
    return rqtt->d_func()->m_renderManager->m_sharedObject.data();
}


/*!
  The constructor creates an instance with the specified \a parent.
 */
QRenderQmlToTexture::QRenderQmlToTexture(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QRenderQmlToTexturePrivate, parent)
{
    Q_D(QRenderQmlToTexture);
    connect(d->m_renderManager, &RenderQmlToTextureManager::onLoadedChanged,
            this, &QRenderQmlToTexture::sourceLoaded);
}

/*!
  Destructor.
 */
QRenderQmlToTexture::~QRenderQmlToTexture()
{
}

bool QRenderQmlToTexture::loaded() const
{
    Q_D(const QRenderQmlToTexture);
    return d->m_renderManager->m_initialized;
}

/*!
    \property QRenderQmlToTexture::source
    \brief Specifies the url for the qml.
 *
    This property specifies the url to the qml being rendered to the texture.
    The source must specify QQuickItem as a root. The item must specify width
    and height. The rendered qml is scaled to the texture size.
    The property can not be changed after the rendering has been initialized.
 */
QUrl QRenderQmlToTexture::source() const
{
    Q_D(const QRenderQmlToTexture);
    return d->m_renderManager->m_source;
}

void QRenderQmlToTexture::setSource(const QUrl &url)
{
    Q_D(QRenderQmlToTexture);
    if (d->m_renderManager->m_initialized) {
        qWarning() << "Unable to set source after initialization.";
        return;
    }
    d->m_renderManager->setSource(url);
    emit sourceChanged(url);
}

/*!
    \property QRenderQmlToTexture::renderOnce
    \brief Property to specify if the texture will be rendered only once.
 *
    This property specifies that the texture will be rendered only one time.
    Once the rendering has been done, resources reserved for rendering will be
    released and the QRenderQmlToTexture will become unusable.
    If set to false, which is the default, the rendering is continuous.
 */
bool QRenderQmlToTexture::renderOnce() const
{
    Q_D(const QRenderQmlToTexture);
    return d->m_renderManager->m_renderOnce;
}

void QRenderQmlToTexture::setRenderOnce(bool once)
{
    Q_D(const QRenderQmlToTexture);
    if (d->m_renderManager->m_renderOnce != once) {
        d->m_renderManager->m_renderOnce = once;
        emit renderOnceChanged(once);
    }
}

/*!
    \property QRenderQmlToTexture::texture
    \brief The texture being rendered to.
 *
    This property specifies the texture being rendered to. Once the texture has been
    set and the rendering begins, the texture can not be changed anymore.
 */
QAbstractTexture *QRenderQmlToTexture::texture() const
{
    Q_D(const QRenderQmlToTexture);
    return d->m_renderManager->m_texture;
}

void QRenderQmlToTexture::setTexture(QAbstractTexture *texture)
{
    Q_D(QRenderQmlToTexture);
    if (d->m_renderManager->m_initialized) {
        qWarning() << "Unable to set texture after initialization.";
        return;
    }
    if (d->m_renderManager->m_texture != texture) {
        if (d->m_renderManager->m_texture)
            QObject::disconnect(d->m_textureDestroyedConnection);
        if (texture && !texture->parent())
            texture->setParent(this);
        d->m_renderManager->setTexture(texture);
        if (texture)
            d->m_textureDestroyedConnection
                    = QObject::connect(texture, &QAbstractTexture::destroyed,
                                       this, &QRenderQmlToTexture::textureDestroyed);
        emit textureChanged(texture);
    }
}

void QRenderQmlToTexture::textureDestroyed(QObject *object)
{
    Q_D(QRenderQmlToTexture);
    Q_UNUSED(object);
    d->m_renderManager->setTexture(nullptr);
}

Qt3DCore::QNodeCreatedChangeBasePtr QRenderQmlToTexture::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QRenderQmlToTextureData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QRenderQmlToTexture);
    data.renderOnce = d->m_renderManager->m_renderOnce;
    data.textureId = d->m_renderManager->m_texture
                        ? d->m_renderManager->m_texture->id() : Qt3DCore::QNodeId();
    data.sharedObject = d->m_renderManager->m_sharedObject;
    return creationChange;
}

/*!
    \internal
 */
void QRenderQmlToTexture::sourceLoaded()
{
    emit loadedChanged(true);
}

} // namespace Qt3DRender

QT_END_NAMESPACE
