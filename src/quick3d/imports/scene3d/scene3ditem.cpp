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

#include "scene3ditem_p.h"
#include "scene3dcleaner_p.h"
#include "scene3dlogging_p.h"
#include "scene3drenderer_p.h"
#include "scene3dsgnode_p.h"

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/qframegraph.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/qinputsettings.h>


#include <QtQuick/qquickwindow.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DCore::Scene3DItem
    \internal

    \brief The Qt3DCore::Scene3DItem class is a QQuickItem subclass used to integrate
    a Qt3D scene into a QtQuick 2 scene.

    The Qt3DCore::Scene3DItem class renders a Qt3D scene, provided by a Qt3DCore::QEntity
    into a multisampled Framebuffer object that is later blitted into a non
    multisampled Framebuffer object to be then renderer through the use of a
    Qt3DCore::Scene3DSGNode with premultiplied alpha.
 */
Scene3DItem::Scene3DItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_entity(Q_NULLPTR)
    , m_aspectEngine(new Qt3DCore::QAspectEngine())
    , m_renderAspect(new QRenderAspect(QRenderAspect::Synchronous))
    , m_renderer(Q_NULLPTR)
    , m_rendererCleaner(new Scene3DCleaner())
    , m_multisample(true)
    , m_cameraAspectRatioMode(AutomaticAspectRatio)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::MouseButtonMask);
    setAcceptHoverEvents(true);

    // TO DO: register the event source in the main thread

    m_aspectEngine->registerAspect(m_renderAspect);
}

Scene3DItem::~Scene3DItem()
{
    // When the window is closed, it first destroys all of its children. At
    // this point, Scene3DItem is destroyed but the Renderer, AspectEngine and
    // Scene3DSGNode still exist and will perform their cleanup on their own.
}

QStringList Scene3DItem::aspects() const
{
    return m_aspects;
}

Qt3DCore::QEntity *Scene3DItem::entity() const
{
    return m_entity;
}

void Scene3DItem::setAspects(const QStringList &aspects)
{
    if (!m_aspects.isEmpty()) {
        qWarning() << "Aspects already set on the Scene3D, ignoring";
        return;
    }

    m_aspects = aspects;

    // Aspects are owned by the aspect engine
    Q_FOREACH (const QString &aspect, m_aspects) {
        if (aspect == QStringLiteral("render")) // This one is hardwired anyway
            continue;
        if (aspect == QStringLiteral("input"))  {
            m_aspectEngine->registerAspect(new Qt3DInput::QInputAspect);
            continue;
        }

        m_aspectEngine->registerAspect(aspect);
    }

    emit aspectsChanged();
}

void Scene3DItem::setEntity(Qt3DCore::QEntity *entity)
{
    if (entity == m_entity)
        return;

    m_entity = entity;
    emit entityChanged();
}

void Scene3DItem::setCameraAspectRatioMode(CameraAspectRatioMode mode)
{
    if (m_cameraAspectRatioMode == mode)
        return;

    m_cameraAspectRatioMode = mode;
    setCameraAspectModeHelper();
    emit cameraAspectRatioModeChanged(mode);
}

Scene3DItem::CameraAspectRatioMode Scene3DItem::cameraAspectRatioMode() const
{
    return m_cameraAspectRatioMode;
}

void Scene3DItem::applyRootEntityChange()
{
    if (m_aspectEngine->rootEntity() != m_entity) {
        m_aspectEngine->setRootEntity(m_entity);

        // Set the render surface
        if (!m_entity)
            return;

        setWindowSurface(m_entity);

        if (m_cameraAspectRatioMode == AutomaticAspectRatio) {
            // Set aspect ratio of first camera to match the window
            QList<Qt3DRender::QCamera *> cameras
                = m_entity->findChildren<Qt3DRender::QCamera *>();
            if (cameras.isEmpty()) {
                qWarning() << "No camera found and automatic aspect ratio requested";
            } else {
                m_camera = cameras.first();
                setCameraAspectModeHelper();
            }
        }

        // Set ourselves up as a source of input events for the input aspect
        Qt3DInput::QInputSettings *inputSettings = m_entity->findChild<Qt3DInput::QInputSettings *>();
        if (inputSettings) {
            inputSettings->setEventSource(this);
        } else {
            qWarning() << "No Input Settings found, keyboard and mouse events won't be handled";
        }
    }
}

void Scene3DItem::setWindowSurface(QObject *rootObject)
{
    // Find surface selector in framegraph and set ourselves up as the
    // render surface there
    Qt3DRender::QFrameGraph *frameGraphComponent
        = rootObject->findChild<Qt3DRender::QFrameGraph *>();
    if (!frameGraphComponent) {
        qWarning() << "No frame graph component found";
        return;
    }

    Qt3DCore::QNode *frameGraphRoot = frameGraphComponent->activeFrameGraph();
    if (!frameGraphRoot) {
        qWarning() << "No active frame graph found";
        return;
    }

    Qt3DRender::QRenderSurfaceSelector *surfaceSelector
        = frameGraphRoot->findChild<Qt3DRender::QRenderSurfaceSelector *>();
    if (!surfaceSelector) {
        qWarning() << "No render surface selector found in frame graph";
        return;
    }

    // Set the item's window surface if it appears
    // the surface wasn't set on the surfaceSelector
    if (!surfaceSelector->window())
        surfaceSelector->setWindow(this->window());
}

void Scene3DItem::setCameraAspectModeHelper()
{
    switch (m_cameraAspectRatioMode) {
    case AutomaticAspectRatio:
        connect(this, &Scene3DItem::widthChanged, this, &Scene3DItem::updateCameraAspectRatio);
        connect(this, &Scene3DItem::heightChanged, this, &Scene3DItem::updateCameraAspectRatio);
        break;
    case UserAspectRatio:
        disconnect(this, &Scene3DItem::widthChanged, this, &Scene3DItem::updateCameraAspectRatio);
        disconnect(this, &Scene3DItem::heightChanged, this, &Scene3DItem::updateCameraAspectRatio);
        break;
    }
}

void Scene3DItem::updateCameraAspectRatio()
{
    if (m_camera) {
        m_camera->setAspectRatio(static_cast<float>(width()) /
                                 static_cast<float>(height()));
    }
}

/*!
    \return \c true if a multisample renderbuffer is in use.
 */
bool Scene3DItem::multisample() const
{
    return m_multisample;
}

/*!
    Enables or disables the usage of multisample renderbuffers based on \a enable.

    By default multisampling is enabled. If the OpenGL implementation has no
    support for multisample renderbuffers or framebuffer blits, the request to
    use multisampling is ignored.

    \note Refrain from changing the value frequently as it involves expensive
    and potentially slow initialization of framebuffers and other OpenGL
    resources.
 */
void Scene3DItem::setMultisample(bool enable)
{
    if (m_multisample != enable) {
        m_multisample = enable;
        emit multisampleChanged();
        update();
    }
}

QSGNode *Scene3DItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    // If the node already exists
    // we delete it and recreate it
    // as we need to resize the FBO
    if (node) {
        delete node;
        node = Q_NULLPTR;
    }

    if (m_renderer == Q_NULLPTR) {
        m_renderer = new Scene3DRenderer(this, m_aspectEngine, m_renderAspect);
        m_renderer->setCleanerHelper(m_rendererCleaner);
    }

    // The main thread is blocked, it is now time to sync data between the renderer and the item.
    m_renderer->synchronize();

    Scene3DSGNode *fboNode = new Scene3DSGNode();
    fboNode->setRect(boundingRect());
    m_renderer->setSGNode(fboNode);
    return fboNode;
}

void Scene3DItem::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //Prevent subsequent move and release events being disregarded my the default event->ignore() from QQuickItem
}

} // namespace Qt3DRender

QT_END_NAMESPACE
