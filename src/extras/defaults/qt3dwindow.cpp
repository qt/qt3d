// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "qt3dwindow.h"
#include "qt3dwindow_p.h"

#include <QtGui/qtguiglobal.h>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcoreaspect.h>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DRender/qrendersettings.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DInput/qinputaspect.h>
#include <Qt3DInput/qinputsettings.h>
#include <Qt3DLogic/qlogicaspect.h>
#include <Qt3DRender/qcamera.h>
#if QT_CONFIG(vulkan)
#  include <Qt3DRender/private/vulkaninstance_p.h>
#endif
#include <Qt3DRender/qt3drender-config.h>
#include <qopenglcontext.h>
#include <private/qrendersettings_p.h>

#include <QEvent>

#if QT_CONFIG(qt3d_vulkan)
#include <QVulkanInstance>
#endif

static void initResources()
{
#ifdef QT_STATIC
    Q_INIT_RESOURCE(extras);
#endif
}

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

Qt3DWindowPrivate::Qt3DWindowPrivate()
    : m_aspectEngine(new Qt3DCore::QAspectEngine)
    , m_renderAspect(new Qt3DRender::QRenderAspect)
    , m_inputAspect(new Qt3DInput::QInputAspect)
    , m_logicAspect(new Qt3DLogic::QLogicAspect)
    , m_renderSettings(new Qt3DRender::QRenderSettings)
    , m_forwardRenderer(new Qt3DExtras::QForwardRenderer)
    , m_defaultCamera(new Qt3DRender::QCamera)
    , m_inputSettings(new Qt3DInput::QInputSettings)
    , m_root(new Qt3DCore::QEntity)
    , m_userRoot(nullptr)
    , m_initialized(false)
{
}

Qt3DWindow::Qt3DWindow(QScreen *screen, Qt3DRender::API api)
    : QWindow(*new Qt3DWindowPrivate(), nullptr)
{
    Q_D(Qt3DWindow);

    initResources();

    if (!d->parentWindow)
        d->connectToScreen(screen ? screen : d->topLevelScreen.data());

    setupWindowSurface(this, api);

    resize(1024, 768);
    d->m_aspectEngine->registerAspect(new Qt3DCore::QCoreAspect);
    d->m_aspectEngine->registerAspect(d->m_renderAspect);
    d->m_aspectEngine->registerAspect(d->m_inputAspect);
    d->m_aspectEngine->registerAspect(d->m_logicAspect);

    d->m_defaultCamera->setParent(d->m_root);
    d->m_forwardRenderer->setCamera(d->m_defaultCamera);
    d->m_forwardRenderer->setSurface(this);
    d->m_renderSettings->setActiveFrameGraph(d->m_forwardRenderer);
    d->m_inputSettings->setEventSource(this);
}

Qt3DWindow::~Qt3DWindow()
{
    Q_D(Qt3DWindow);
    delete d->m_aspectEngine;
}

/*!
    Registers the specified \a aspect.
*/
void Qt3DWindow::registerAspect(Qt3DCore::QAbstractAspect *aspect)
{
    Q_ASSERT(!isVisible());
    Q_D(Qt3DWindow);
    d->m_aspectEngine->registerAspect(aspect);
}

/*!
    Registers the specified aspect \a name.
*/
void Qt3DWindow::registerAspect(const QString &name)
{
    Q_ASSERT(!isVisible());
    Q_D(Qt3DWindow);
    d->m_aspectEngine->registerAspect(name);
}

/*!
    Sets the specified \a root entity of the scene.
*/
void Qt3DWindow::setRootEntity(Qt3DCore::QEntity *root)
{
    Q_D(Qt3DWindow);
    if (d->m_userRoot != root) {
        if (d->m_userRoot != nullptr)
            d->m_userRoot->setParent(static_cast<Qt3DCore::QNode*>(nullptr));
        if (root != nullptr)
            root->setParent(d->m_root);
        d->m_userRoot = root;
    }
}

/*!
    Activates the specified \a activeFrameGraph.
*/
void Qt3DWindow::setActiveFrameGraph(Qt3DRender::QFrameGraphNode *activeFrameGraph)
{
    Q_D(Qt3DWindow);
    d->m_renderSettings->setActiveFrameGraph(activeFrameGraph);
}

/*!
    Returns the node of the active frame graph.
*/
Qt3DRender::QFrameGraphNode *Qt3DWindow::activeFrameGraph() const
{
    Q_D(const Qt3DWindow);
    return d->m_renderSettings->activeFrameGraph();
}

/*!
    Returns the node of the default framegraph
*/
Qt3DExtras::QForwardRenderer *Qt3DWindow::defaultFrameGraph() const
{
    Q_D(const Qt3DWindow);
    return d->m_forwardRenderer;
}

Qt3DRender::QCamera *Qt3DWindow::camera() const
{
    Q_D(const Qt3DWindow);
    return d->m_defaultCamera;
}

/*!
    Returns the render settings of the 3D Window.
*/
Qt3DRender::QRenderSettings *Qt3DWindow::renderSettings() const
{
    Q_D(const Qt3DWindow);
    return d->m_renderSettings;
}

/*!
    Manages the display events specified in \a e.
*/
void Qt3DWindow::showEvent(QShowEvent *e)
{
    Q_D(Qt3DWindow);
    if (!d->m_initialized) {
        d->m_root->addComponent(d->m_renderSettings);
        d->m_root->addComponent(d->m_inputSettings);
        d->m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(d->m_root));

        d->m_initialized = true;
    }
    QWindow::showEvent(e);
}

/*!
    Resets the aspect ratio of the 3D window.
*/
void Qt3DWindow::resizeEvent(QResizeEvent *)
{
    Q_D(Qt3DWindow);
    d->m_defaultCamera->setAspectRatio(float(width()) / std::max(1.f, static_cast<float>(height())));
}

/*!
    \reimp

    Requests renderer to redraw if we are using OnDemand render policy.
*/
bool Qt3DWindow::event(QEvent *e)
{
    Q_D(Qt3DWindow);
    const bool needsRedraw = (e->type() == QEvent::Expose || e->type() == QEvent::UpdateRequest);
    if (needsRedraw && d->m_renderSettings->renderPolicy() == Qt3DRender::QRenderSettings::OnDemand) {
        Qt3DRender::QRenderSettingsPrivate *p = static_cast<Qt3DRender::QRenderSettingsPrivate *>(
                    Qt3DCore::QNodePrivate::get(d->m_renderSettings));
        p->invalidateFrame();
    }
    return QWindow::event(e);
}

void setupWindowSurface(QWindow *window, Qt3DRender::API api) noexcept
{
    // If the user pass an API through the environment, we use that over the one passed as argument.
    const auto userRequestedApi = qgetenv("QSG_RHI_BACKEND").toLower();
    if (!userRequestedApi.isEmpty()) {
        if (userRequestedApi == QByteArrayLiteral("opengl") ||
            userRequestedApi == QByteArrayLiteral("gl") ||
            userRequestedApi == QByteArrayLiteral("gles2")) {
            api = Qt3DRender::API::OpenGL;
        } else if (userRequestedApi == QByteArrayLiteral("vulkan")) {
            api = Qt3DRender::API::Vulkan;
        } else if (userRequestedApi == QByteArrayLiteral("metal")) {
            api = Qt3DRender::API::Metal;
        } else if (userRequestedApi == QByteArrayLiteral("d3d11")) {
            api = Qt3DRender::API::DirectX;
        } else if (userRequestedApi == QByteArrayLiteral("null")) {
            api = Qt3DRender::API::Null;
        } else if (userRequestedApi == QByteArrayLiteral("auto")) {
            api = Qt3DRender::API::RHI;
        }
    }

    // Default to using RHI backend is not specified We want to set the
    // variable to ensure any 3rd party relying on it to detect which rendering
    // backend is in use will get a valid value.
    bool useRhi = false;
    if (qEnvironmentVariableIsEmpty("QT3D_RENDERER")) {
#if QT_CONFIG(qt3d_rhi_renderer)
        qputenv("QT3D_RENDERER", "rhi");
#else
        qputenv("QT3D_RENDERER", "opengl");
#endif
    }
#if QT_CONFIG(qt3d_rhi_renderer)
    useRhi = qEnvironmentVariable("QT3D_RENDERER") == QStringLiteral("rhi");
#endif

    if (!useRhi)
        api = Qt3DRender::API::OpenGL;

    // We have to set the environment so that the backend is able to read it.
    if (api == Qt3DRender::API::RHI && useRhi) {
#if defined(Q_OS_WIN)
        api = Qt3DRender::API::DirectX;
#elif defined(Q_OS_MACOS) || defined(Q_OS_IOS)
        api = Qt3DRender::API::Metal;
#elif QT_CONFIG(opengl)
        api = Qt3DRender::API::OpenGL;
#else
        api = Qt3DRender::API::Vulkan;
#endif
    }

    switch (api) {
    case Qt3DRender::API::OpenGL:
        qputenv("QSG_RHI_BACKEND", "opengl");
        window->setSurfaceType(QSurface::OpenGLSurface);
        break;
    case Qt3DRender::API::DirectX:
        qputenv("QSG_RHI_BACKEND", "d3d11");
        window->setSurfaceType(QSurface::Direct3DSurface);
        break;
    case Qt3DRender::API::Null:
        qputenv("QSG_RHI_BACKEND", "null");
        window->setSurfaceType(QSurface::OpenGLSurface);
        break;
    case Qt3DRender::API::Metal:
        qputenv("QSG_RHI_BACKEND", "metal");
        window->setSurfaceType(QSurface::MetalSurface);
        break;
#if QT_CONFIG(qt3d_vulkan)
    case Qt3DRender::API::Vulkan:
    {
        qputenv("QSG_RHI_BACKEND", "vulkan");
        window->setSurfaceType(QSurface::VulkanSurface);
        window->setVulkanInstance(&Qt3DRender::staticVulkanInstance());
        break;
    }
#endif
    case Qt3DRender::API::RHI:
    default:
        break;
    }

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    const QByteArray renderingBackend = qgetenv("QT3D_RENDERER");
    const bool usesRHI = renderingBackend.isEmpty() || renderingBackend == QByteArrayLiteral("rhi");
    if (!usesRHI) {
#if QT_CONFIG(opengles2)
        format.setRenderableType(QSurfaceFormat::OpenGLES);
#else
        if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
            format.setVersion(4, 3);
            format.setProfile(QSurfaceFormat::CoreProfile);
        }
#endif
    }

    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    window->setFormat(format);
    QSurfaceFormat::setDefaultFormat(format);
}

} // Qt3DExtras

QT_END_NAMESPACE

#include "moc_qt3dwindow.cpp"
