// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "offscreensurfacehelper_p.h"

#include <Qt3DRender/private/abstractrenderer_p.h>
#include <QtGui/qoffscreensurface.h>
#include <QtGui/qsurfaceformat.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qthread.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

/*! \internal */
OffscreenSurfaceHelper::OffscreenSurfaceHelper(AbstractRenderer *renderer,
                                               QObject *parent)
    : QObject(parent)
    , m_renderer(renderer)
    , m_offscreenSurface(nullptr)
{
    Q_ASSERT(renderer);
}

/*!
 * \internal
 * Called in context of main thread to create an offscreen surface
 * which can later be made current with the Qt 3D OpenGL context to
 * then allow graphics resources to be released cleanly.
 */
void OffscreenSurfaceHelper::createOffscreenSurface()
{
    Q_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());
    m_offscreenSurface = new QOffscreenSurface;
    m_offscreenSurface->setParent(this);
    m_offscreenSurface->setFormat(m_renderer->format());
    m_offscreenSurface->setScreen(m_renderer->screen());
    m_offscreenSurface->create();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_offscreensurfacehelper_p.cpp"
