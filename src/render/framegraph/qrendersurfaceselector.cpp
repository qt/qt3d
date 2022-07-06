// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrendersurfaceselector.h"
#include "qrendersurfaceselector_p.h"

#include <QtGui/QWindow>
#include <QtGui/QScreen>
#include <QtGui/QOffscreenSurface>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qrendersettings.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QRenderSurfaceSelector
    \inmodule Qt3DRender
    \brief Provides a way of specifying the render surface.
    \since 5.7

    The Qt3DRender::QRenderSurfaceSelector can be used to select the surface, where
    Qt3D renders the content. The surface can either be window surface or offscreen
    surface. The externalRenderTargetSize is used to specify the actual size of the
    surface when offscreen surface is used.

    When DPI scaling is used by the system, the logical surface size, which is used
    by mouse events, and the actual 'physical' size of the surface can differ.
    The surfacePixelRatio is the factor to convert the logical size to the physical
    size.

    \sa QWindow, QOffscreenSurface, QSurface
 */

/*!
    \qmltype RenderSurfaceSelector
    \inqmlmodule Qt3D.Render
    \since 5.7
    \instantiates Qt3DRender::QRenderSurfaceSelector
    \inherits FrameGraphNode
    \brief Provides a way of specifying the render surface.

    The RenderSurfaceSelector can be used to select the surface, where
    Qt3D renders the content. The surface can either be window surface or offscreen
    surface. The externalRenderTargetSize is used to specify the actual size of the
    render target when offscreen surface is used.

    When DPI scaling is used by the system, the logical surface size, which is used
    by mouse events, and the actual 'physical' size of the surface can differ.
    The surfacePixelRatio is the factor to convert the logical size to the physical
    size.
 */

/*! \qmlproperty QSurface Qt3D.Render::RenderSurfaceSelector::surface
    Holds the surface.
 */

/*! \qmlproperty size Qt3D.Render::RenderSurfaceSelector::externalRenderTargetSize

    Holds the size of the external render target.
 */

/*! \qmlproperty real Qt3D.Render::RenderSurfaceSelector::surfacePixelRatio

    Holds the surfacePixelRatio of the surface.
 */

/*!
     \property QRenderSurfaceSelector::surface
     Holds the surface
 */

/*!
     \property QRenderSurfaceSelector::externalRenderTargetSize
     Holds the size of the external render target.
 */

/*! \property QRenderSurfaceSelector::surfacePixelRatio

    Holds the surfacePixelRatio of the surface.
 */

QRenderSurfaceSelectorPrivate::QRenderSurfaceSelectorPrivate()
    : Qt3DRender::QFrameGraphNodePrivate()
    , m_surface(nullptr)
    , m_surfaceEventFilter(new Qt3DRender::Render::PlatformSurfaceFilter())
    , m_surfacePixelRatio(1.0f)
{
}

QRenderSurfaceSelectorPrivate::~QRenderSurfaceSelectorPrivate()
{
    QObject::disconnect(m_heightConn);
    QObject::disconnect(m_widthConn);
    QObject::disconnect(m_screenConn);
}

QRenderSurfaceSelector *QRenderSurfaceSelectorPrivate::find(QObject *rootObject)
{
    QFrameGraphNode *frameGraphRoot = qobject_cast<QFrameGraphNode *>(rootObject);

    if (!frameGraphRoot) {
        auto rendererSettings = rootObject->findChild<Qt3DRender::QRenderSettings *>();
        if (!rendererSettings) {
            qWarning() << "No renderer settings component found";
            return nullptr;
        }

        frameGraphRoot = rendererSettings->activeFrameGraph();
        if (!frameGraphRoot) {
            qWarning() << "No active frame graph found";
            return nullptr;
        }
    }

    auto surfaceSelector = qobject_cast<Qt3DRender::QRenderSurfaceSelector *>(frameGraphRoot);
    if (!surfaceSelector)
        surfaceSelector = frameGraphRoot->findChild<Qt3DRender::QRenderSurfaceSelector *>();

    if (!surfaceSelector)
        qWarning() << "No render surface selector found in frame graph";

    return surfaceSelector;
}

void QRenderSurfaceSelectorPrivate::setExternalRenderTargetSize(const QSize &size)
{
    m_externalRenderTargetSize = size;
}

/*!
    Constructs QRenderSurfaceSelector with given \a parent.
 */
QRenderSurfaceSelector::QRenderSurfaceSelector(Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(*new QRenderSurfaceSelectorPrivate, parent)
{
}

/*!
    \internal
 */
QRenderSurfaceSelector::~QRenderSurfaceSelector()
{
}

/*!
    \internal
 */
QRenderSurfaceSelector::QRenderSurfaceSelector(QRenderSurfaceSelectorPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(dd, parent)
{
}

QObject *QRenderSurfaceSelector::surface() const
{
    Q_D(const QRenderSurfaceSelector);
    QObject *surfaceObj = nullptr;
    if (!d->m_surface)
        return surfaceObj;

    switch (d->m_surface->surfaceClass()) {
    case QSurface::Window:
        surfaceObj = static_cast<QWindow *>(d->m_surface);
        break;

    case QSurface::Offscreen:
        surfaceObj = static_cast<QOffscreenSurface *>(d->m_surface);
        break;
    }

    return surfaceObj;
}

/*!
    Sets \a surfaceObject.
 */
void QRenderSurfaceSelector::setSurface(QObject *surfaceObject)
{
    Q_D(QRenderSurfaceSelector);
    QSurface *surface = nullptr;
    if (surfaceObject) {
        QWindow *window = qobject_cast<QWindow *>(surfaceObject);
        if (window) {
            surface = static_cast<QSurface *>(window);
        } else {
            QOffscreenSurface *offscreen = qobject_cast<QOffscreenSurface *>(surfaceObject);
            if (offscreen)
                surface = static_cast<QSurface *>(offscreen);
        }

        Q_ASSERT_X(surface, Q_FUNC_INFO, "surfaceObject is not a valid QSurface    object");
    }

    if (d->m_surface == surface)
        return;

    if (d->m_surface && d->m_surface->surfaceClass() == QSurface::Window) {
        QWindow *prevWindow = static_cast<QWindow *>(d->m_surface);
        if (prevWindow) {
            QObject::disconnect(d->m_widthConn);
            QObject::disconnect(d->m_heightConn);
            QObject::disconnect(d->m_screenConn);
        }
    }
    d->m_surface = surface;

    // The platform surface filter only deals with QObject
    // We assume therefore that our surface is actually a QObject underneath
    if (d->m_surface) {
        switch (d->m_surface->surfaceClass()) {
        case QSurface::Window: {
            QWindow *window = static_cast<QWindow *>(d->m_surface);
            d->m_surfaceEventFilter->setSurface(window);

            if (window) {
                d->m_widthConn = QObject::connect(window, &QWindow::widthChanged, [=] (int) {
                    d->update();
                });
                d->m_heightConn = QObject::connect(window, &QWindow::heightChanged, [=] (int) {
                    d->update();
                });
                d->m_screenConn = QObject::connect(window, &QWindow::screenChanged, [this] (QScreen *screen) {
                    if (screen && !qFuzzyCompare(surfacePixelRatio(), float(screen->devicePixelRatio())))
                        setSurfacePixelRatio(float(screen->devicePixelRatio()));
                });
                setSurfacePixelRatio(float(window->devicePixelRatio()));
            }

            break;
        }
        case QSurface::Offscreen: {
            d->m_surfaceEventFilter->setSurface(static_cast<QOffscreenSurface *>(d->m_surface));
            break;
        }

        default:
            Q_UNREACHABLE();
            break;
        }
    } else {
        QWindow *nullWindow = nullptr;
        d->m_surfaceEventFilter->setSurface(nullWindow);
    }
    emit surfaceChanged(surfaceObject);
}

QSize QRenderSurfaceSelector::externalRenderTargetSize() const
{
    Q_D(const QRenderSurfaceSelector);
    return d->externalRenderTargetSize();
}

void QRenderSurfaceSelector::setSurfacePixelRatio(float ratio)
{
    Q_D(QRenderSurfaceSelector);
    if (qFuzzyCompare(d->m_surfacePixelRatio, ratio))
        return;
    d->m_surfacePixelRatio = ratio;
    emit surfacePixelRatioChanged(ratio);
}

float QRenderSurfaceSelector::surfacePixelRatio() const
{
    Q_D(const QRenderSurfaceSelector);
    return d->m_surfacePixelRatio;
}
/*!
    Sets render target \a size if different than underlying surface size.
    Tells picking the correct size.
 */
void QRenderSurfaceSelector::setExternalRenderTargetSize(const QSize &size)
{
    Q_D(QRenderSurfaceSelector);
    if (size != d->m_externalRenderTargetSize) {
        d->setExternalRenderTargetSize(size);
        emit externalRenderTargetSizeChanged(size);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qrendersurfaceselector.cpp"
