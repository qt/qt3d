// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
#include <QtQml/private/qqmlglobal_p.h>
#include <QtQuick/qquickwindow.h>
#include <QtQuick/qquickrendercontrol.h>

#include <Qt3DRender/private/qrendersurfaceselector_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/qt3drender-config.h>
#include <scene3dlogging_p.h>
#include <scene3drenderer_p.h>
#include <scene3dsgnode_p.h>

#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class AspectEngineDestroyer : public QObject
{
    Q_OBJECT

public:
    AspectEngineDestroyer()
        : QObject()
    {}

    ~AspectEngineDestroyer()
    {
    }

    void reset(int targetCount)
    {
        m_allowed = 0;
        m_targetAllowed = targetCount;
    }

    void allowRelease()
    {
        ++m_allowed;
        if (m_allowed == m_targetAllowed) {
            if (QThread::currentThread() == thread())
                delete this;
            else
                deleteLater();
        }
    }

    void setSGNodeAlive(bool alive) { m_sgNodeAlive = alive; }
    bool sgNodeAlive() const { return m_sgNodeAlive;}

private:
    int m_allowed = 0;
    int m_targetAllowed = 0;
    bool m_sgNodeAlive = false;
};

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
    ...) under the scene.

    \note Śetting the visibility of the Scene3D element to false will halt the
    Qt 3D simulation loop. This means that binding the visible property to an
    expression that depends on property updates driven by the Qt 3D simulation
    loop (FrameAction) will never reavaluates.
 */
Scene3DItem::Scene3DItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_entity(nullptr)
    , m_aspectEngine(nullptr)
    , m_aspectToDelete(nullptr)
    , m_lastManagerNode(nullptr)
    , m_aspectEngineDestroyer()
    , m_multisample(true)
    , m_dirty(true)
    , m_wasFrameProcessed(false)
    , m_wasSGUpdated(false)
    , m_cameraAspectRatioMode(AutomaticAspectRatio)
    , m_compositingMode(FBO)
    , m_framesToRender(ms_framesNeededToFlushPipeline)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::MouseButtonMask);
    setAcceptHoverEvents(true);
    // TO DO: register the event source in the main thread

    // Give a default size so that if nothing is specified by the user
    // we still won't get ignored by the QtQuick SG when in Underlay mode
    setWidth(1);
    setHeight(1);

    if (qgetenv("QT3D_RENDERER").isEmpty()) {
#if QT_CONFIG(qt3d_rhi_renderer)
        qputenv("QT3D_RENDERER", "rhi"); // QtQuick requires RHI
#else
        qputenv("QT3D_RENDERER", "opengl"); // QtQuick requires OpenGL
#endif
    }
}

Scene3DItem::~Scene3DItem()
{
    // The SceneGraph is non deterministic in the order in which it will
    // destroy the QSGNode that were created by the item. This unfortunately
    // makes it difficult to know when it is safe to destroy the QAspectEngine.
    // To track this we use the AspectEngineDestroyer. It allows keeping the
    // AspectEngine alive and deleting later when we know that both Scene3DItem
    // and Scene3DRenderer have been destroyed.

    delete m_aspectToDelete;

    if (m_aspectEngineDestroyer)
        m_aspectEngineDestroyer->allowRelease();
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

    \qmldefault

    The root entity of the 3D scene to be displayed.
 */
Qt3DCore::QEntity *Scene3DItem::entity() const
{
    return m_entity;
}

void Scene3DItem::applyAspects()
{
    if (!m_aspectEngine)
        return;

    // Aspects are owned by the aspect engine
    for (const QString &aspect : std::as_const(m_aspects)) {
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
}

void Scene3DItem::setAspects(const QStringList &aspects)
{
    if (!m_aspects.isEmpty()) {
        qWarning() << "Aspects already set on the Scene3D, ignoring";
        return;
    }

    m_aspects = aspects;
    applyAspects();

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

    The default value is \c FBO.
    \since 5.14
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

void Scene3DItem::applyRootEntityChange()
{
    if (m_aspectEngine->rootEntity().data() != m_entity) {

        Qt3DCore::QEntityPtr entityPtr;
        // We must reuse the QEntityPtr of the old AspectEngine
        // otherwise it will delete the Entity once it gets destroyed
        if (m_aspectToDelete)
            entityPtr = m_aspectToDelete->rootEntity();
        else
            entityPtr = Qt3DCore::QEntityPtr(m_entity);

        m_aspectEngine->setRootEntity(entityPtr);

        /* If we changed window, the old aspect engine must be deleted only after we have set
           the root entity for the new one so that it doesn't delete the root node. */
        if (m_aspectToDelete) {
            delete m_aspectToDelete;
            m_aspectToDelete = nullptr;
        }

        // Set the render surface
        if (!m_entity)
            return;

        setWindowSurface(entity());

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

bool Scene3DItem::needsRender(QRenderAspect *renderAspect)
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

    auto renderAspectPriv = static_cast<QRenderAspectPrivate*>(QRenderAspectPrivate::get(renderAspect));
    const bool dirty = m_dirty
            || (renderAspectPriv
                && renderAspectPriv->m_renderer
                && renderAspectPriv->m_renderer->shouldRender());

    if (m_dirty) {
        --m_framesToRender;
        if (m_framesToRender <= 0)
            m_dirty = false;
    }
    return dirty || m_framesToRender > 0;
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
bool Scene3DItem::prepareQt3DFrame()
{
    static bool dontRenderWhenHidden = !qgetenv("QT3D_SCENE3D_STOP_RENDER_HIDDEN").isEmpty();

    // If we are not visible, don't processFrame changes as we would end up
    // waiting forever for the scene to be rendered which won't happen
    // if the Scene3D item is not visible
    if (!isVisible() && dontRenderWhenHidden)
        return false;
    if (!m_aspectEngine)
        return false;
    Q_ASSERT(QThread::currentThread() == thread());

    // Since we are in manual mode, trigger jobs for the next frame
    Qt3DCore::QAspectEnginePrivate *aspectEnginePriv = static_cast<Qt3DCore::QAspectEnginePrivate *>(QObjectPrivate::get(m_aspectEngine));
    if (!aspectEnginePriv->m_initialized)
        return false;

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

    // Note: it's too early to request an update at this point as
    // beforeSync() triggered by afterAnimating  is considered
    // to be as being part of the current frame update
    return true;
}

void Scene3DItem::requestUpdate()
{
    // When using the FBO mode, only the QQuickItem needs to be updated
    // When using the Underlay mode, the whole windows needs updating
    const bool usesFBO = m_compositingMode == FBO;
    if (usesFBO) {
        QQuickItem::update();
    } else {
        window()->update();
    }
}

void Scene3DItem::updateWindowSurface()
{
    if (!m_entity)
        return;
    Qt3DRender::QRenderSurfaceSelector *surfaceSelector =
        Qt3DRender::QRenderSurfaceSelectorPrivate::find(entity());
    if (surfaceSelector) {
        if (QWindow *rw = QQuickRenderControl::renderWindowFor(this->window())) {
            surfaceSelector->setSurface(rw);
        }
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
            surfaceSelector->setSurface(rw);
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
    Qt3DCore::QEntity *rootEntity = entity();
    if (!rootEntity) {
        return;
    }
    Qt3DRender::QRenderSurfaceSelector *surfaceSelector = Qt3DRender::QRenderSurfaceSelectorPrivate::find(rootEntity);
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

// We want to tie the Scene3DRenderer's lifetime to the QSGNode associated with
// Scene3DItem. This ensures that when the SceneGraph tree gets destroyed, we
// also shutdown Qt3D properly
// Everything this class does happens in the QSGRenderThread
class Scene3DManagerNode : public QSGNode
{
public:
   explicit Scene3DManagerNode(Qt3DCore::QAspectEngine *aspectEngine,
                               AspectEngineDestroyer *destroyer)
        : m_aspectEngine(aspectEngine)
        , m_destroyer(destroyer)
        , m_renderAspect(new QRenderAspect(QRenderAspect::Manual))
        , m_renderer(new Scene3DRenderer())
    {
        m_destroyer->setSGNodeAlive(true);
    }

    ~Scene3DManagerNode()
    {
        // Stop the Qt3D Simulation Loop
        auto engineD = Qt3DCore::QAspectEnginePrivate::get(m_aspectEngine);
        engineD->exitSimulationLoop();

        // Shutdown renderer
        delete m_renderer;

        m_destroyer->setSGNodeAlive(false);

        // Allow AspectEngine destruction
        m_destroyer->allowRelease();
    }

    void init()
    {
        m_aspectEngine->registerAspect(m_renderAspect);
        m_renderer->init(m_aspectEngine, m_renderAspect);
        m_wasInitialized = true;
    }

    inline bool isInitialized() const { return m_wasInitialized; }
    inline QRenderAspect *renderAspect() const { return m_renderAspect; }
    inline Scene3DRenderer *renderer() const { return m_renderer; }
private:
    Qt3DCore::QAspectEngine *m_aspectEngine;
    AspectEngineDestroyer *m_destroyer;
    QRenderAspect *m_renderAspect;
    Scene3DRenderer *m_renderer;
    bool m_wasInitialized = false;
};


// QtQuick SG
// beforeSynchronize                                           // SG Thread (main thread blocked)
// updatePaintNode    (-> Scene3DRenderer::beforeSynchronize)  // SG Thread (main thread blocked)
// beforeRenderering  (-> Scene3DRenderer::beforeSynchronize)  // SG Thread (main thread unblocked)
// afterRenderering                                            // SG Thread (main thread unblocked)
// afterAnimating     (-> Scene3DItem::synchronize())          // Main Thread (SG Thread is not yet at  beforeSynchronize )

// main thread (afterAnimating)
void Scene3DItem::synchronize()
{
    // Request updates for the next frame
    requestUpdate();

    if (!window() || !m_wasSGUpdated ||
        (!m_aspectEngineDestroyer || !m_aspectEngineDestroyer->sgNodeAlive())) {
        m_wasFrameProcessed = false;
        return;
    }

    // Set root Entity on the aspectEngine
    applyRootEntityChange();

    // Update size of the QSurfaceSelector if needed
    setItemAreaAndDevicePixelRatio(boundingRect().size().toSize(),
                                   window()->effectiveDevicePixelRatio());

    // Let Qt3D process the frame and launch jobs
    m_wasFrameProcessed = prepareQt3DFrame();

    m_wasSGUpdated = false;
}

// The synchronization point between the main thread and the render thread
// before any rendering
QSGNode *Scene3DItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    Scene3DManagerNode *managerNode = static_cast<Scene3DManagerNode *>(node);
    QSGRendererInterface::GraphicsApi windowApi = window()->rendererInterface()->graphicsApi();

    // In case we have no GL context, return early
    // m_wasSGUpdated will not be set to true and nothing will take place
    if ((windowApi == QSGRendererInterface::OpenGLRhi ||
         windowApi == QSGRendererInterface::OpenGL) && !QOpenGLContext::currentContext()) {
        QQuickItem::update();
        return node;
    }

    // Scene3DManagerNode gets automatically destroyed on Window changed, SceneGraph invalidation
    if (!managerNode) {
        // Did we have a Scene3DManagerNode in the past?
        if (m_lastManagerNode != nullptr) {
            // If so we need to recreate a new AspectEngine as node was destroyed by sceneGraph
            qCWarning(Scene3D) << "Renderer for Scene3DItem has requested a reset due to the item "
                                  "moving to another window";
            QObject::disconnect(m_windowConnection);
            // We are in the Render thread, and the m_aspectEngineDestroyer lives in the Main
            // thread, so we must avoid sending ChildRemoved or ChildAdded events to it with a
            // QObject::setParent(). QCoreApplication::sendEvent() would fail with "Cannot
            // send events to objects owned by a different thread."
            QQml_setParent_noEvent(m_aspectEngine, nullptr);
            // Note: AspectEngine can only be deleted once we have set the root
            // entity on the new instance
            m_aspectToDelete = m_aspectEngine;
            m_aspectEngine = nullptr;
        }

        // Create or Recreate AspectEngine
        if (m_aspectEngine == nullptr) {
            // Use manual drive mode when using Scene3D
            delete m_aspectEngineDestroyer;
            m_aspectEngineDestroyer = new AspectEngineDestroyer();
            m_aspectEngine = new Qt3DCore::QAspectEngine(m_aspectEngineDestroyer);
            m_aspectEngine->setRunMode(Qt3DCore::QAspectEngine::Manual);
            applyAspects();

            // Needs to belong in the same thread as the item which is the same as
            // the original QAspectEngine
            m_aspectEngineDestroyer->moveToThread(thread());
            m_aspectEngine->moveToThread(thread());

            // To destroy AspectEngine
            m_aspectEngineDestroyer->reset(2);
        }

        // Create new instance and record a pointer (which should only be used
        // to check if we have had a previous manager node)
        managerNode = new Scene3DManagerNode(m_aspectEngine,
                                             m_aspectEngineDestroyer);
        m_lastManagerNode = managerNode;

        // Before Synchronizing is in the SG Thread, we want synchronize to be triggered
        // in the context of the main thread so we use afterAnimating instead
        m_windowConnection = QObject::connect(window(), &QQuickWindow::afterAnimating,
                                    this, &Scene3DItem::synchronize, Qt::DirectConnection);
    }

    Scene3DRenderer *renderer = managerNode->renderer();
    QRenderAspect *renderAspect = managerNode->renderAspect();

    renderer->setBoundingSize(boundingRect().size().toSize());
    renderer->setMultisample(m_multisample);
    // Ensure Renderer is working on current window
    renderer->setWindow(window());
    // Set compositing mode on renderer
    renderer->setCompositingMode(m_compositingMode);

    // If the render aspect wasn't created yet, do so now
    if (!managerNode->isInitialized()) {
        auto *rw = QQuickRenderControl::renderWindowFor(window());
        auto renderAspectPriv = static_cast<QRenderAspectPrivate*>(QRenderAspectPrivate::get(renderAspect));
        renderAspectPriv->m_screen = (rw ? rw->screen() : window()->screen());
        updateWindowSurface();

#if !QT_CONFIG(qt3d_rhi_renderer)
        QSGRendererInterface::GraphicsApi windowApi = window()->rendererInterface()->graphicsApi();

        if (windowApi != QSGRendererInterface::OpenGLRhi &&
            windowApi != QSGRendererInterface::OpenGL) {

            qFatal("Qt3D's RHI Renderer is not enabled, please configure RHI to use the OpenGL backend "
                   "by calling qputenv(\"QSG_RHI_BACKEND\", \"opengl\")");
        }
#endif
        managerNode->init();
        // Note: ChangeArbiter is only set after aspect was registered
        QObject::connect(
                renderAspectPriv->m_aspectManager->changeArbiter(),
                &Qt3DCore::QChangeArbiter::receivedChange, this,
                [this] {
                    m_dirty = true;
                    m_framesToRender = ms_framesNeededToFlushPipeline;
                },
                Qt::DirectConnection);

        // Give the window a nudge to trigger an update.
        QMetaObject::invokeMethod(window(), "requestUpdate", Qt::QueuedConnection);
    }

    const bool usesFBO = m_compositingMode == FBO;
    Scene3DSGNode *fboNode = static_cast<Scene3DSGNode *>(managerNode->firstChild());

    // When using Scene3D in Underlay mode
    // we shouldn't be managing a Scene3DSGNode
    if (!usesFBO) {
        if (fboNode != nullptr) {
            managerNode->removeChildNode(fboNode);
            delete fboNode;
            fboNode = nullptr;
        }
    } else {
        // Regular Scene3D only case
        // Create SGNode if using FBO and no Scene3DViews
        fboNode = renderer->sgNode();
        if (fboNode) {
            if (!fboNode->parent())
                managerNode->appendChildNode(fboNode);

            // Depending on the backend in use, we might or might not have
            // to flip content
            fboNode->setRect(boundingRect(), !renderer->isYUp());
        }
    }

    // Set whether we want the Renderer to be allowed to render or not
    const bool skipFrame = !needsRender(renderAspect);
    renderer->setSkipFrame(skipFrame);
    renderer->allowRender();

    // Let the renderer prepare anything it needs to prior to the rendering
    if (m_wasFrameProcessed)
        renderer->beforeSynchronize();

    // Force window->beforeRendering to be triggered
    managerNode->markDirty(QSGNode::DirtyForceUpdate);

    m_wasSGUpdated = true;

    return managerNode;
}

void Scene3DItem::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //Prevent subsequent move and release events being disregarded my the default event->ignore() from QQuickItem
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_scene3ditem_p.cpp"
#include "scene3ditem.moc"
