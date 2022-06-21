// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qscene2d.h"
#include "qscene2d_p.h"
#include "scene2d_p.h"
#include "scene2dmanager_p.h"
#include "scene2devent_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

namespace Quick {

class RenderControl : public QQuickRenderControl
{
public:
    RenderControl(QWindow *w) : m_window(w) { }
    QWindow *renderWindow(QPoint *offset) override;

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
     Constructs qml render manager.
 */
Scene2DManager::Scene2DManager(QScene2DPrivate *priv)
    : m_rootItem(nullptr)
    , m_item(nullptr)
    , m_priv(priv)
    , m_sharedObject(new Scene2DSharedObject(this))
    , m_renderPolicy(QScene2D::Continuous)
    , m_requested(false)
    , m_initialized(false)
    , m_renderSyncRequested(false)
    , m_backendInitialized(false)
    , m_mouseEnabled(true)
{
    m_sharedObject->m_surface = new QOffscreenSurface;
    m_sharedObject->m_surface->setFormat(QSurfaceFormat::defaultFormat());
    m_sharedObject->m_surface->create();

    // Create render control
    m_sharedObject->m_renderControl = new RenderControl(nullptr);

    // Create window to render the QML with
    m_sharedObject->m_quickWindow = new QQuickWindow(m_sharedObject->m_renderControl);
    // TODOQT6 see qtdeclarative 0d43e21429ee23442ec3a99f641a5665d66e75e2
    //m_sharedObject->m_quickWindow->setClearBeforeRendering(true);
    m_sharedObject->m_quickWindow->setColor(Qt::transparent);

    connect(m_sharedObject->m_renderControl, &QQuickRenderControl::renderRequested,
            this, &Scene2DManager::requestRender);
    connect(m_sharedObject->m_renderControl, &QQuickRenderControl::sceneChanged,
            this, &Scene2DManager::requestRenderSync);
}

Scene2DManager::~Scene2DManager()
{
    m_sharedObject = nullptr;
}

void Scene2DManager::requestRender()
{
    // Don't request render until the backend is initialized.
    if (m_sharedObject->canRender()) {
        if (!m_requested) {
            m_requested = true;
            QCoreApplication::postEvent(this, new Scene2DEvent(Scene2DEvent::Render));
        }
    }
}

void Scene2DManager::requestRenderSync()
{
    // Don't request render until the backed is initialized.
    if (m_sharedObject->canRender()) {
        if (!m_requested) {
            m_requested = true;
            QCoreApplication::postEvent(this, new Scene2DEvent(Scene2DEvent::RenderSync));
        }
    } else {
        m_renderSyncRequested = true;
    }
}

void Scene2DManager::startIfInitialized()
{
    if (!m_initialized && m_backendInitialized && m_item != nullptr) {
        m_rootItem = m_item;

        // Associate root item with the window.
        m_rootItem->setParentItem(m_sharedObject->m_quickWindow->contentItem());

        // Update window size.
        updateSizes();

        m_initialized = true;
        m_sharedObject->setInitialized();

        // Request render if we have already been requested and preparation has already been done
        if (m_sharedObject->isPrepared() && m_renderSyncRequested) {
            if (!m_requested) {
                m_requested = true;
                QCoreApplication::postEvent(this, new Scene2DEvent(Scene2DEvent::RenderSync));
            }
            m_renderSyncRequested = false;
        }
    }
}

void Scene2DManager::stopAndClean()
{
    if (m_sharedObject->isInitialized()) {
        QMutexLocker lock(&m_sharedObject->m_mutex);
        m_sharedObject->requestQuit();
        m_sharedObject->wait();
        m_sharedObject->cleanup();
    }
}

void Scene2DManager::updateSizes()
{
    const int width = m_rootItem->width();
    const int height = m_rootItem->height();
    if (width == 0 || height == 0) {
        qWarning() << "QScene2D: Root item size not set.";
        return;
    }
    m_sharedObject->m_quickWindow->setGeometry(0, 0, width, height);
}

void Scene2DManager::setItem(QQuickItem *item)
{
    m_item = item;
    startIfInitialized();
}

bool Scene2DManager::event(QEvent *e)
{
    switch (static_cast<Scene2DEvent::Type>(e->type())) {

    case Scene2DEvent::Render: {
        // just render request, don't need to call sync in render thread
        QMutexLocker lock(&m_sharedObject->m_mutex);
        m_sharedObject->requestRender(false);
        m_requested = false;
        return true;
    }

    case Scene2DEvent::RenderSync: {
        // sync and render request, main and render threads must be synchronized
        if (!m_sharedObject->isQuit())
            doRenderSync();
        m_requested = false;
        return true;
    }

    case Scene2DEvent::Prepare: {
        m_sharedObject->m_renderControl->prepareThread(m_sharedObject->m_renderThread);
        m_sharedObject->setPrepared();

        if (m_renderSyncRequested) {
            if (!m_requested) {
                m_requested = true;
                QCoreApplication::postEvent(this, new Scene2DEvent(Scene2DEvent::RenderSync));
            }
            m_renderSyncRequested = false;
        }
        return true;
    }

    case Scene2DEvent::Initialized: {
        // backend is initialized, start the qml
        m_backendInitialized = true;
        startIfInitialized();
        return true;
    }

    case Scene2DEvent::Rendered: {
        // render is done, excellent, now clean anything not needed anymore.
        stopAndClean();
        return true;
    }

    default:
        break;
    }
    return QObject::event(e);
}

void Scene2DManager::doRenderSync()
{
    QMutexLocker lock(&m_sharedObject->m_mutex);

    m_sharedObject->requestRender(true);
    m_sharedObject->m_renderControl->polishItems();

    // begin waiting render thread
    m_sharedObject->wait();
    m_requested = false;
}

void Scene2DManager::cleanup()
{
    stopAndClean();
}

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_scene2dmanager_p.cpp"
