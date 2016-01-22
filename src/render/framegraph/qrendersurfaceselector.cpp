/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qrendersurfaceselector.h"
#include "qrendersurfaceselector_p.h"

#include <QtGui/qwindow.h>
#include <QtGui/qoffscreensurface.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QRenderSurfaceSelector
 * \inmodule Qt3DRender
 * \brief QRenderSurfaceSelector
 * \since
 *
 * \inherits Qt3DRender::QFrameGraphNode
 *
 */

/*!
 * \qmltype RenderSurfaceSelector
 * \inqmlmodule Qt3D.Render
 * \since
 * \ingroup
 * \instantiates Qt3DRender::QRenderSurfaceSelector
 * \brief RenderSurfaceSelector
 *
 */

/*! \qmlproperty QSurface Qt3D.Render::RenderSurfaceSelector::surface
 *
 * Holds the surface.
 */

/*! \qmlproperty QWindow Qt3D.Render::RenderSurfaceSelector::window
 *
 * Holds the window.
 */

/*! \qmlproperty QSize Qt3D.Render::RenderSurfaceSelector::externalRenderTargetSize
 *
 * Holds the size of the external render target.
 */

QRenderSurfaceSelectorPrivate::QRenderSurfaceSelectorPrivate()
    : Qt3DRender::QFrameGraphNodePrivate()
    , m_surface(Q_NULLPTR)
    , m_surfaceEventFilter(new Qt3DRender::Render::PlatformSurfaceFilter())
{
}

void QRenderSurfaceSelectorPrivate::setExternalRenderTargetSize(const QSize &size)
{
    m_externalRenderTargetSize = size;
}

/*!
 * Constructs QRenderSurfaceSelector with given \a parent.
 */
QRenderSurfaceSelector::QRenderSurfaceSelector(Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(*new QRenderSurfaceSelectorPrivate, parent)
{
}

/*!
 * \internal
 */
QRenderSurfaceSelector::QRenderSurfaceSelector(QRenderSurfaceSelectorPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(dd, parent)
{
}

/*!
 * Destroys QRenderSurfaceSelector.
 */
QRenderSurfaceSelector::~QRenderSurfaceSelector()
{
    QNode::cleanup();
}

QSurface *QRenderSurfaceSelector::surface() const
{
    Q_D(const QRenderSurfaceSelector);
    return d->m_surface;
}

QWindow *QRenderSurfaceSelector::window() const
{
    Q_D(const QRenderSurfaceSelector);
    if (d->m_surface && d->m_surface->surfaceClass() == QSurface::Window)
        return static_cast<QWindow *>(d->m_surface);
    return Q_NULLPTR;
}

/*! \property QRenderSurfaceSelector::surface
 *
 * Sets \a surface.
 */
void QRenderSurfaceSelector::setSurface(QSurface *surface)
{
    Q_D(QRenderSurfaceSelector);
    if (d->m_surface == surface)
        return;

    d->m_surface = surface;
    // The platform surface filter only deals with QObject
    // We assume therefore that our surface is actually a QObject underneath
    if (d->m_surface) {
        switch (d->m_surface->surfaceClass()) {
        case QSurface::Window:
            d->m_surfaceEventFilter->setSurface(static_cast<QWindow *>(d->m_surface));
            break;
        case QSurface::Offscreen:
            d->m_surfaceEventFilter->setSurface(static_cast<QOffscreenSurface *>(d->m_surface));
            break;
        default:
            break;
        }
    } else {
        QWindow *nullWindow = Q_NULLPTR;
        d->m_surfaceEventFilter->setSurface(nullWindow);
    }
    emit surfaceChanged(surface);
}

QSize QRenderSurfaceSelector::externalRenderTargetSize() const
{
    Q_D(const QRenderSurfaceSelector);
    return d->externalRenderTargetSize();
}

/*!
 * Sets render target \a size if different than underlying surface size.
 * Tells picking the correct size.
 */
void QRenderSurfaceSelector::setExternalRenderTargetSize(const QSize &size)
{
    Q_D(QRenderSurfaceSelector);
    d->setExternalRenderTargetSize(size);
    emit externalRenderTargetSizeChanged(size);
}

/*! \property QRenderSurfaceSelector::window
 *
 * Sets \a window.
 */
void QRenderSurfaceSelector::setWindow(QWindow *window)
{
    Q_D(QRenderSurfaceSelector);
    QWindow *currentWindow = Q_NULLPTR;
    if (d->m_surface && d->m_surface->surfaceClass() == QSurface::Window)
        currentWindow = static_cast<QWindow *>(d->m_surface);
    if (currentWindow == window)
        return;

    d->m_surface = window;
    d->m_surfaceEventFilter->setSurface(window);

    if (window) {
        QObject::connect(window, &QWindow::widthChanged, [=] (int width) {
            if (d->m_changeArbiter != Q_NULLPTR) {
                Qt3DCore::QScenePropertyChangePtr change(
                            new Qt3DCore::QScenePropertyChange(
                                Qt3DCore::NodeUpdated,
                                Qt3DCore::QSceneChange::Node,
                                id()));

                change->setPropertyName("width");
                change->setValue(QVariant::fromValue(width));
                d->notifyObservers(change);
            }
        });
        QObject::connect(window, &QWindow::heightChanged, [=] (int height) {
            if (d->m_changeArbiter != Q_NULLPTR) {
                Qt3DCore::QScenePropertyChangePtr change(
                            new Qt3DCore::QScenePropertyChange(
                                Qt3DCore::NodeUpdated,
                                Qt3DCore::QSceneChange::Node,
                                id()));

                change->setPropertyName("height");
                change->setValue(QVariant::fromValue(height));
                d->notifyObservers(change);
            }
        });
    }

    emit windowChanged(window);
    emit surfaceChanged(d->m_surface);
}

/*!
 * \internal
 */
void QRenderSurfaceSelector::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QRenderSurfaceSelector *object = static_cast<const QRenderSurfaceSelector *>(ref);

    d_func()->m_surface = object->d_func()->m_surface;
    d_func()->m_externalRenderTargetSize = object->d_func()->m_externalRenderTargetSize;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
