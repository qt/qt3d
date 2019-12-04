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

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/QAspectEngine>

#if QT_CONFIG(qt3d_input)
#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/qinputsettings.h>
#endif

#if QT_CONFIG(qt3d_logic)
#include <Qt3DLogic/qlogicaspect.h>
#endif

#if QT_CONFIG(qt3d_animation)
#include <Qt3DAnimation/qanimationaspect.h>
#endif

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qrendersurfaceselector.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQuick/qquickwindow.h>
#include <QtQuick/qquickrendercontrol.h>

#include <Qt3DRender/private/qrendersurfaceselector_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <scene3dcleaner_p.h>
#include <scene3dlogging_p.h>
#include <scene3drenderer_p.h>
#include <scene3dsgnode_p.h>
#include <scene3dview_p.h>

#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::Scene3DItem
    \internal

    \brief The Scene3DItem class is a QQuickItem subclass used to integrate
    a Qt3D scene into a QtQuick 2 scene.

    The Scene3DItem class renders a Qt3D scene, provided by a Qt3DCore::QEntity
    into a multisampled Framebuffer object that is later blitted into a
    non-multisampled Framebuffer object to be then rendered through the use of a
    Qt3DCore::Scene3DSGNode with premultiplied alpha.
 */

/*!
    \qmltype Scene3D
    \inherits Item
    \inqmlmodule QtQuick.Scene3D

    \preliminary

    \brief The Scene3D type is used to integrate a Qt3D scene into a QtQuick 2
    scene.

    The Scene3D type renders a Qt3D scene, provided by an \l Entity, into a
    multisampled Framebuffer object. This object is later blitted into a
    non-multisampled Framebuffer object, which is then rendered with
    premultiplied alpha. If multisampling is not required, it can be avoided
    by setting the \l multisample property to \c false. In this case
    Scene3D will render directly into the non-multisampled Framebuffer object.

    If the scene to be rendered includes non-opaque materials, you may need to
    modify those materials with custom blend arguments in order for them to be
    rendered correctly. For example, if working with a \l PhongAlphaMaterial and
    a scene with an opaque clear color, you will likely want to add:

    \qml
    sourceAlphaArg: BlendEquationArguments.Zero
    destinationAlphaArg: BlendEquationArguments.One
    \endqml

    to that material.

    It is not recommended to instantiate more than a single Scene3D instance
    per application. The reason for this is that a Scene3D instance
    instantiates the entire Qt 3D engine (memory managers, thread pool, render
    ...) under the scene. You should instead look into using \l Scene3DView
    instances in conjunction with a single Scene3D instance.

    When using Scene3D with Scene3DViews the following conditions are expected:
    \list
    \li The compositingMode is set to FBO
    \li The Scene3D is sized to occupy the full window size
    \li The Scene3D instance is instantiated prior to any Scene3DView
    \li The Scene3D entity property is left unset
    \endlist

    \note Śetting the visibility of the Scene3D element to false will halt the
    Qt 3D simulation loop. This means that binding the visible property to an
    expression that depends on property updates driven by the Qt 3D simulation
    loop (FrameAction) will never reavaluates.
 */
Scene3DItem::Scene3DItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_entity(nullptr)
    , m_viewHolderEntity(nullptr)
    , m_viewHolderFG(nullptr)
    , m_aspectEngine(new Qt3DCore::QAspectEngine())
    , m_renderAspect(nullptr)
    , m_renderer(nullptr)
    , m_rendererCleaner(new Scene3DCleaner())
    , m_multisample(true)
    , m_dirty(true)
    , m_dirtyViews(false)
    , m_clearsWindowByDefault(true)
    , m_disableClearWindow(false)
    , m_cameraAspectRatioMode(AutomaticAspectRatio)
    , m_compositingMode(FBO)
    , m_dummySurface(nullptr)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::MouseButtonMask);
    // TO DO: register the event source in the main thread

    // Use manual drive mode when using Scene3D
    m_aspectEngine->setRunMode(Qt3DCore::QAspectEngine::Manual);

    // Give a default size so that if nothing is specified by the user
    // we still won't get ignored by the QtQuick SG when in Underlay mode
    setWidth(1);
    setHeight(1);
}

Scene3DItem::~Scene3DItem()
{
    // When the window is closed, it first destroys all of its children. At
    // this point, Scene3DItem is destroyed but the Renderer, AspectEngine and
    // Scene3DSGNode still exist and will perform their cleanup on their own.
}

/*!
    \qmlproperty list<string> Scene3D::aspects

    The list of aspects that should be registered for the 3D scene.

    For example, if the scene makes use of FrameAction, the \c "logic" aspect should be included in the list.

    The \c "render" aspect is hardwired and does not need to be explicitly listed.
*/
QStringList Scene3DItem::aspects() const
{
    return m_aspects;
}

/*!
    \qmlproperty Entity Scene3D::entity

    \default

    The root entity of the 3D scene to be displayed.
 */
Qt3DCore::QEntity *Scene3DItem::entity() const
{
    return m_entity;
}

void Scene3DItem::setAspects(const QStringList &aspects)
{
    if (!m_aspects.isEmpty()) {
        qCWarning(Scene3D) << "Aspects already set on the Scene3D, ignoring";
        return;
    }

    m_aspects = aspects;

    // Aspects are owned by the aspect engine
    for (const QString &aspect : qAsConst(m_aspects)) {
        if (aspect == QLatin1String("render")) // This one is hardwired anyway
            continue;
        if (aspect == QLatin1String("input"))  {
#if QT_CONFIG(qt3d_input)
            m_aspectEngine->registerAspect(new Qt3DInput::QInputAspect);
            continue;
#else
            qFatal("Scene3D requested the Qt 3D input aspect but Qt 3D wasn't configured to build the Qt 3D Input aspect");
#endif
        }
        if (aspect == QLatin1String("logic"))  {
#if QT_CONFIG(qt3d_logic)
            m_aspectEngine->registerAspect(new Qt3DLogic::QLogicAspect);
            continue;
#else
            qFatal("Scene3D requested the Qt 3D logic aspect but Qt 3D wasn't configured to build the Qt 3D Logic aspect");
#endif
        }
        if (aspect == QLatin1String("animation"))  {
#if QT_CONFIG(qt3d_animation)
            m_aspectEngine->registerAspect(new Qt3DAnimation::QAnimationAspect);
            continue;
#else
            qFatal("Scene3D requested the Qt 3D animation aspect but Qt 3D wasn't configured to build the Qt 3D Animation aspect");
#endif
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

void Scene3DItem::setHoverEnabled(bool enabled)
{
    if (enabled != acceptHoverEvents()) {
        setAcceptHoverEvents(enabled);
        emit hoverEnabledChanged();
    }
}

/*!
    \qmlproperty enumeration Scene3D::compositingMode

    \value FBO
           Scene is rendered into a Frame Buffer Object which can be costly on
           some platform and hardware but allows a greater amount of
           flexibility. Automatic aspect ratio. This is the compositing mode to
           choose if your Scene3D element shouldn't occupy the entire screen
           and if you optionally plan on having it resized or animated. In this
           mode, the position of the Scene3D in the QML file controls its
           stacking order with regard to the other Qt Quick elements.

    \value Underlay
           Suitable for full screen 3D scenes where using an FBO might be too
           resource intensive. Scene3D behaves as a QtQuick underlay.

           Please note that when using this mode, the size of the Scene3D and
           its transformations are ignored and the rendering will occupy the
           whole screen. The position of the Scene3D in the QML file won't have
           any effect either. The Qt 3D content will be drawn prior to any Qt
           Quick content. Care has to be taken not to overdraw and hide the Qt
           3D content by overlapping Qt Quick content.

           Additionally when using this mode, the window clearBeforeRendering
           will be set to false automatically.

    \since 5.14
    \default FBO
 */
void Scene3DItem::setCompositingMode(Scene3DItem::CompositingMode mode)
{
    if (m_compositingMode == mode)
        return;
    m_compositingMode = mode;
    emit compositingModeChanged();

    QQuickItem::update();
}

/*!
    \qmlproperty enumeration Scene3D::cameraAspectRatioMode

    \value Scene3D.AutomaticAspectRatio
           Automatic aspect ratio.

    \value Scene3D.UserAspectRatio
           User defined aspect ratio.
    \brief \TODO
 */
Scene3DItem::CameraAspectRatioMode Scene3DItem::cameraAspectRatioMode() const
{
    return m_cameraAspectRatioMode;
}

Scene3DItem::CompositingMode Scene3DItem::compositingMode() const
{
    return m_compositingMode;
}

// MainThread called by Scene3DView
void Scene3DItem::addView(Scene3DView *view)
{
    if (m_views.contains(view))
        return;

    Qt3DRender::QFrameGraphNode *viewFG = view->viewFrameGraph();
    Qt3DCore::QEntity *subtreeRoot = view->viewSubtree();

    if (m_viewHolderEntity == nullptr) {
        m_viewHolderEntity = new Qt3DCore::QEntity;

        if (m_entity != nullptr) {
            qCWarning(Scene3D) << "Scene3DView is not supported if the Scene3D entity property has been set";
        }

        Qt3DRender::QRenderSettings *settings = new Qt3DRender::QRenderSettings();
        Qt3DRender::QRenderSurfaceSelector *surfaceSelector = new Qt3DRender::QRenderSurfaceSelector();
        m_viewHolderFG = surfaceSelector;
        surfaceSelector->setSurface(window());

        // Copy setting properties from first View
        QVector<Qt3DRender::QRenderSettings *> viewRenderSettings = subtreeRoot->componentsOfType<Qt3DRender::QRenderSettings>();
        if (viewRenderSettings.size() > 0) {
            Qt3DRender::QRenderSettings *viewRenderSetting = viewRenderSettings.first();
            settings->setRenderPolicy(viewRenderSetting->renderPolicy());
            settings->pickingSettings()->setPickMethod(viewRenderSetting->pickingSettings()->pickMethod());
            settings->pickingSettings()->setPickResultMode(viewRenderSetting->pickingSettings()->pickResultMode());
        }
        settings->setActiveFrameGraph(m_viewHolderFG);
        m_viewHolderEntity->addComponent(settings);

        setEntity(m_viewHolderEntity);
    }

    // Parent FG and Subtree
    viewFG->setParent(m_viewHolderFG);
    subtreeRoot->setParent(m_viewHolderEntity);

    m_views.push_back(view);
    m_dirtyViews |= true;
}

// MainThread called by Scene3DView
void Scene3DItem::removeView(Scene3DView *view)
{
    if (!m_views.contains(view))
        return;

    Qt3DRender::QFrameGraphNode *viewFG = view->viewFrameGraph();
    Qt3DCore::QEntity *subtreeRoot = view->viewSubtree();

    // Unparent FG and Subtree
    viewFG->setParent(Q_NODE_NULLPTR);
    subtreeRoot->setParent(Q_NODE_NULLPTR);

    m_views.removeOne(view);
    m_dirtyViews |= true;
}

void Scene3DItem::applyRootEntityChange()
{
    if (m_aspectEngine->rootEntity() != m_entity) {
        m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(m_entity));

        // Set the render surface
        if (!m_entity)
            return;

        setWindowSurface(m_entity);

        if (m_cameraAspectRatioMode == AutomaticAspectRatio) {
            // Set aspect ratio of first camera to match the window
            QList<Qt3DRender::QCamera *> cameras
                = m_entity->findChildren<Qt3DRender::QCamera *>();
            if (cameras.isEmpty()) {
                qCDebug(Scene3D) << "No camera found and automatic aspect ratio requested";
            } else {
                m_camera = cameras.first();
                setCameraAspectModeHelper();
            }
        }

#if QT_CONFIG(qt3d_input)
        // Set ourselves up as a source of input events for the input aspect
        Qt3DInput::QInputSettings *inputSettings = m_entity->findChild<Qt3DInput::QInputSettings *>();
        if (inputSettings) {
            inputSettings->setEventSource(this);
        } else {
            qCDebug(Scene3D) << "No Input Settings found, keyboard and mouse events won't be handled";
        }
#endif
    }
}

bool Scene3DItem::needsRender()
{
    // We need the dirty flag which is connected to the change arbiter
    // receiving updates to know whether something in the scene has changed

    // Ideally we would use shouldRender() alone but given that it becomes true
    // only after the arbiter has sync the changes and might be reset before
    // process jobs is completed, we cannot fully rely on it. It would require
    // splitting processFrame in 2 parts.

    // We only use it for cases where Qt3D render may require several loops of
    // the simulation to fully process a frame (e.g shaders are loaded in frame
    // n and we can only build render commands for the new shader at frame n +
    // This is where renderer->shouldRender() comes into play as it knows
    // whether some states remain dirty or not (even after processFrame is
    // called)

    auto renderAspectPriv = static_cast<QRenderAspectPrivate*>(QRenderAspectPrivate::get(m_renderAspect));
    const bool dirty = m_dirty
            || (renderAspectPriv
                && renderAspectPriv->m_renderer
                && renderAspectPriv->m_renderer->shouldRender());
    m_dirty = false;
    return dirty;
}

// This function is triggered in the context of the Main Thread
// when afterAnimating is emitted

// The QtQuick SG proceeds like indicated below:
// afterAnimating (Main Thread)
// beforeSynchronizing (SG Thread and MainThread locked)
// afterSynchronizing (SG Thread and MainThread locked)
// beforeRendering (SG Thread)

// Note: we connect to afterAnimating rather than beforeSynchronizing as a
// direct connection on beforeSynchronizing is executed within the SG Render
// Thread context. This is executed before the RenderThread is asked to
// synchronize and render
// Note: we might still not be done rendering when this is called but
// processFrame will block and wait for renderer to have been finished
void Scene3DItem::onBeforeSync()
{
    // If we are not visible, don't processFrame changes as we would end up
    // waiting forever for the scene to be rendered which won't happen
    // if the Scene3D item is not visible
    if (!isVisible())
        return;

    Q_ASSERT(QThread::currentThread() == thread());

    // Since we are in manual mode, trigger jobs for the next frame
    Qt3DCore::QAspectEnginePrivate *aspectEnginePriv = static_cast<Qt3DCore::QAspectEnginePrivate *>(QObjectPrivate::get(m_aspectEngine));
    if (!aspectEnginePriv->m_initialized || !m_renderer)
        return;

    // Set compositing mode on renderer
    m_renderer->setCompositingMode(m_compositingMode);
    const bool usesFBO = m_compositingMode == FBO;

    // Make renderer aware of any Scene3DView we are dealing with
    if (m_dirtyViews) {
        // Scene3DViews checks
        if (m_entity != m_viewHolderEntity) {
            qCWarning(Scene3D) << "Scene3DView is not supported if the Scene3D entity property has been set";
        }
        if (!usesFBO) {
            qCWarning(Scene3D) << "Scene3DView is only supported when Scene3D compositingMode is set to FBO";
        }
        // The Scene3DRender will take care of providing the texture containing the 3D scene
        m_renderer->setScene3DViews(m_views);
        m_dirtyViews = false;
    }

    Q_ASSERT(m_aspectEngine->runMode() == Qt3DCore::QAspectEngine::Manual);
    m_aspectEngine->processFrame();
    // The above essentially sets the number of RV for the RenderQueue and
    // processes the jobs for the frame (it's blocking) When
    // Scene3DRender::updatePaintNode is called, following this step, we know
    // that the RenderQueue target count has been set and that everything
    // should be ready for rendering


    // processFrame() must absolutely be followed by a single call to
    // render
    // At startup, we have no garantee that the QtQuick Render Thread doesn't
    // start rendering before this function has been called
    // We add in a safety to skip such frames as this could otherwise
    // make Qt3D enter a locked state
    m_renderer->setSkipFrame(!needsRender());
    m_renderer->allowRender();

    // Note: it's too early to request an update at this point as
    // beforeSync() triggered by afterAnimating  is considered
    // to be as being part of the current frame update
}

void Scene3DItem::requestUpdate()
{
    // When using the FBO mode, only the QQuickItem needs to be updated
    // When using the Underlay mode, the whole windows needs updating
    const bool usesFBO = m_compositingMode == FBO;
    if (usesFBO) {
        QQuickItem::update();
        for (Scene3DView *view : m_views)
            view->update();
    } else {
        window()->update();
    }
}

void Scene3DItem::setWindowSurface(QObject *rootObject)
{
    Qt3DRender::QRenderSurfaceSelector *surfaceSelector = Qt3DRender::QRenderSurfaceSelectorPrivate::find(rootObject);

    // Set the item's window surface if it appears
    // the surface wasn't set on the surfaceSelector
    if (surfaceSelector && !surfaceSelector->surface()) {
        // We may not have a real, exposed QQuickWindow when the Quick rendering
        // is redirected via QQuickRenderControl (f.ex. QQuickWidget).
        if (QWindow *rw = QQuickRenderControl::renderWindowFor(this->window())) {
            // rw is the top-level window that is backed by a native window. Do
            // not use that though since we must not clash with e.g. the widget
            // backingstore compositor in the gui thread.
            m_dummySurface = new QOffscreenSurface;
            m_dummySurface->setParent(qGuiApp); // parent to something suitably long-living
            m_dummySurface->setFormat(rw->format());
            m_dummySurface->setScreen(rw->screen());
            m_dummySurface->create();
            surfaceSelector->setSurface(m_dummySurface);
        } else {
            surfaceSelector->setSurface(this->window());
        }
    }
}
/*!
    \qmlmethod void Scene3D::setItemAreaAndDevicePixelRatio(size area, real devicePixelRatio)

    Sets the item area to \a area and the pixel ratio to \a devicePixelRatio.
 */
void Scene3DItem::setItemAreaAndDevicePixelRatio(QSize area, qreal devicePixelRatio)
{
    Qt3DRender::QRenderSurfaceSelector *surfaceSelector = Qt3DRender::QRenderSurfaceSelectorPrivate::find(m_entity);
    if (surfaceSelector) {
        surfaceSelector->setExternalRenderTargetSize(area);
        surfaceSelector->setSurfacePixelRatio(devicePixelRatio);
    }
}

/*!
    \qmlproperty bool Scene3D::hoverEnabled

    \c true if hover events are accepted.
 */
bool Scene3DItem::isHoverEnabled() const
{
    return acceptHoverEvents();
}

void Scene3DItem::setCameraAspectModeHelper()
{
    if (m_compositingMode == FBO) {
        switch (m_cameraAspectRatioMode) {
        case AutomaticAspectRatio:
            connect(this, &Scene3DItem::widthChanged, this, &Scene3DItem::updateCameraAspectRatio);
            connect(this, &Scene3DItem::heightChanged, this, &Scene3DItem::updateCameraAspectRatio);
            // Update the aspect ratio the first time the surface is set
            updateCameraAspectRatio();
            break;
        case UserAspectRatio:
            disconnect(this, &Scene3DItem::widthChanged, this, &Scene3DItem::updateCameraAspectRatio);
            disconnect(this, &Scene3DItem::heightChanged, this, &Scene3DItem::updateCameraAspectRatio);
            break;
        }
    } else {
        // In Underlay mode, we rely on the window for aspect ratio and not the size of the Scene3DItem
        switch (m_cameraAspectRatioMode) {
        case AutomaticAspectRatio:
            connect(window(), &QWindow::widthChanged, this, &Scene3DItem::updateCameraAspectRatio);
            connect(window(), &QWindow::heightChanged, this, &Scene3DItem::updateCameraAspectRatio);
            // Update the aspect ratio the first time the surface is set
            updateCameraAspectRatio();
            break;
        case UserAspectRatio:
            disconnect(window(), &QWindow::widthChanged, this, &Scene3DItem::updateCameraAspectRatio);
            disconnect(window(), &QWindow::heightChanged, this, &Scene3DItem::updateCameraAspectRatio);
            break;
        }
    }
}

void Scene3DItem::updateCameraAspectRatio()
{
    if (m_camera) {
        if (m_compositingMode == FBO)
            m_camera->setAspectRatio(static_cast<float>(width()) /
                                     static_cast<float>(height()));
        else
            m_camera->setAspectRatio(static_cast<float>(window()->width()) /
                                     static_cast<float>(window()->height()));
    }
}

/*!
    \qmlproperty bool Scene3D::multisample

    \c true if a multisample render buffer is requested.

    By default multisampling is enabled. If the OpenGL implementation has no
    support for multisample renderbuffers or framebuffer blits, the request to
    use multisampling is ignored.

    \note Refrain from changing the value frequently as it involves expensive
    and potentially slow initialization of framebuffers and other OpenGL
    resources.
 */
bool Scene3DItem::multisample() const
{
    return m_multisample;
}

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
    // If the render aspect wasn't created yet, do so now
    if (m_renderAspect == nullptr) {
        m_renderAspect = new QRenderAspect(QRenderAspect::Synchronous);
        auto *rw = QQuickRenderControl::renderWindowFor(window());
        static_cast<Qt3DRender::QRenderAspectPrivate *>(Qt3DRender::QRenderAspectPrivate::get(m_renderAspect))->m_screen =
                (rw ? rw->screen() : window()->screen());
        m_aspectEngine->registerAspect(m_renderAspect);

        // Before Synchronizing is in the SG Thread, we want beforeSync to be triggered
        // in the context of the main thread
        QObject::connect(window(), &QQuickWindow::afterAnimating,
                         this, &Scene3DItem::onBeforeSync, Qt::DirectConnection);
        auto renderAspectPriv = static_cast<QRenderAspectPrivate*>(QRenderAspectPrivate::get(m_renderAspect));
        QObject::connect(renderAspectPriv->m_aspectManager->changeArbiter(), &Qt3DCore::QChangeArbiter::receivedChange,
                         this, [this] { m_dirty = true; }, Qt::DirectConnection);
    }

    if (m_renderer == nullptr) {
        m_renderer = new Scene3DRenderer(this, m_aspectEngine, m_renderAspect);
        m_renderer->setCleanerHelper(m_rendererCleaner);
    }
    const bool usesFBO = m_compositingMode == FBO;
    const bool hasScene3DViews = !m_views.empty();
    Scene3DSGNode *fboNode = static_cast<Scene3DSGNode *>(node);

    // When usin Scene3DViews or Scene3D in Underlay mode
    // we shouldn't be managing a Scene3DSGNode
    if (!usesFBO || hasScene3DViews) {
        if (fboNode != nullptr) {
            delete fboNode;
            fboNode = nullptr;
            m_renderer->setSGNode(fboNode);
        }
    } else {
        // Regular Scene3D only case
        // Create SGNode if using FBO and no Scene3DViews
        if (fboNode == nullptr) {
            fboNode = new Scene3DSGNode();
            m_renderer->setSGNode(fboNode);
        }
        fboNode->setRect(boundingRect());
    }

    if (usesFBO) {
        // Reset clear flag if we've set it to false it's still set to that
        if (m_disableClearWindow && !window()->clearBeforeRendering())
            window()->setClearBeforeRendering(m_clearsWindowByDefault);
        m_disableClearWindow = false;
    } else {
        // Record clearBeforeRendering value before we force it to false
        m_clearsWindowByDefault = window()->clearBeforeRendering();
        m_disableClearWindow = true;
        if (m_clearsWindowByDefault)
            window()->setClearBeforeRendering(false);
    }

    // Request update for next frame so that we can check whether we need to
    // render again or not
    static int requestUpdateMethodIdx =  Scene3DItem::staticMetaObject.indexOfMethod("requestUpdate()");
    static QMetaMethod requestUpdateMethod =Scene3DItem::staticMetaObject.method(requestUpdateMethodIdx);
    requestUpdateMethod.invoke(this, Qt::QueuedConnection);

    return fboNode;
}

void Scene3DItem::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //Prevent subsequent move and release events being disregarded my the default event->ignore() from QQuickItem
}

} // namespace Qt3DRender

QT_END_NAMESPACE
