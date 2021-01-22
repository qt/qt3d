/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "renderthread_p.h"
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <QEventLoop>
#include <QTime>
#include <QMutexLocker>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

RenderThread::RenderThread(AbstractRenderer *renderer)
    : QThread(),
      m_renderer(renderer),
      m_semaphore(0)
{
}

RenderThread::~RenderThread()
{
}

// Called by Renderer in the context of the Aspect Thread
void RenderThread::waitForStart( Priority priority )
{
    qCDebug(Render::Backend) << "Starting Render thread and then going to sleep until it is ready for us...";
    start( priority );
    m_semaphore.acquire();
    qCDebug(Render::Backend) << "Render thread is now ready & calling thread is now awake again";
}

// RenderThread context
void RenderThread::run()
{
    // return control to the aspect thread that created us.
    m_semaphore.release();

    // This call to Renderer::initialize() waits for a surface to be set on the
    // renderer in the context of the Aspect Thread
    m_renderer->initialize();

    // Enter the main OpenGL submission loop.
    m_renderer->render();

    // Clean up any OpenGL resources
    m_renderer->releaseGraphicsResources();

    qCDebug(Render::Backend) << "Exiting RenderThread";
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
