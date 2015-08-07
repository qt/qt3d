/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#include "platformsurfacefilter_p.h"

#include <Qt3DRenderer/private/renderer_p.h>

#include <QMetaObject>
#include <QPlatformSurfaceEvent>
#include <QOffscreenSurface>
#include <QtGui/qwindow.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

PlatformSurfaceFilter::PlatformSurfaceFilter(Renderer *renderer,
                                             QObject *parent)
    : QObject(parent)
    , m_obj(Q_NULLPTR)
    , m_surface(Q_NULLPTR)
    , m_renderer(renderer)
{
    Q_ASSERT(m_renderer);
    qRegisterMetaType<QSurface *>("QSurface*");
}

PlatformSurfaceFilter::~PlatformSurfaceFilter()
{
    if (m_obj)
        m_obj->removeEventFilter(this);
}

void PlatformSurfaceFilter::setWindow(QWindow *window)
{
    setSurface(window);
}

void PlatformSurfaceFilter::setOffscreenSurface(QOffscreenSurface *offscreen)
{
    setSurface(offscreen);
}

bool PlatformSurfaceFilter::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == m_obj && e->type() == QEvent::PlatformSurface) {
        QPlatformSurfaceEvent *ev = static_cast<QPlatformSurfaceEvent *>(e);

        switch (ev->surfaceEventType()) {
        case QPlatformSurfaceEvent::SurfaceCreated:
            setRendererSurface(m_surface);
            break;

        case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed:
            setRendererSurface(Q_NULLPTR);
            break;

        default:
            qCritical("Unknown surface type");
            Q_UNREACHABLE();
        }
    }
    return false;
}

void PlatformSurfaceFilter::setRendererSurface(QSurface *surface)
{
    // Tell the renderer about the surface on which to render. This function
    // is called in the context of the main thread and internally
    // the renderer uses a private thread to submit OpenGL calls. The surface
    // pointer within the renderer is protected by a mutex that is locked for
    // the duration of a frame. In this way, the renderer can be sure to have
    // a valid surface for the duration of the frame for which it is submitting
    // draw calls. Only when the frame finishes and the mutex is unlocked does
    // this call to Renderer::setSurface continue. Thereby blocking the main
    // thread from destroying the platform surface before we are ready.
    m_renderer->setSurface(surface);
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
