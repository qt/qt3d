/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qwindow.h"
#include "qwindow_p.h"

#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcamera.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DRenderer/qforwardrenderer.h>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QWindowPrivate::QWindowPrivate()
    : ::QWindowPrivate()
    , m_root(new QEntity())
    , m_userRoot(Q_NULLPTR)
    , m_defaultCamera(new QCamera())
    , m_frameGraph(Q_NULLPTR)
    , m_engine(new QAspectEngine())
    , m_renderAspect(new QRenderAspect())
{
    m_engine->registerAspect(m_renderAspect);

    // TO DO: Find a nice way to set the camera on the controller which is in
    // the input aspect
}

QWindow::QWindow(::QWindow *parent)
    : ::QWindow(*new QWindowPrivate(), parent)
{
    setSurfaceType(QSurface::OpenGLSurface);

    resize(1024, 768);

    QSurfaceFormat format;
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    setFormat(format);
    create();
}

QWindow::QWindow(QWindowPrivate &dd, ::QWindow *parent)
    : ::QWindow(dd, parent)
{
    setSurfaceType(QSurface::OpenGLSurface);

    resize(1024, 768);

    QSurfaceFormat format;
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    setFormat(format);
    create();
}

void QWindow::resizeEvent(QResizeEvent *)
{
    Q_D(QWindow);
    d->m_defaultCamera->setAspectRatio(float(width()) / float(height()));
}

QWindow::~QWindow()
{
}

void QWindow::setFrameGraph(QFrameGraph *frameGraph)
{
    Q_ASSERT(!isVisible());
    Q_D(QWindow);
    d->m_frameGraph = frameGraph;
}

QFrameGraph *QWindow::frameGraph() const
{
    Q_D(const QWindow);
    return d->m_frameGraph;
}

QCamera *QWindow::defaultCamera()
{
    Q_D(const QWindow);
    return d->m_defaultCamera;
}

void QWindow::registerAspect(QAbstractAspect *aspect)
{
    Q_ASSERT(!isVisible());
    Q_D(QWindow);
    d->m_engine->registerAspect(aspect);
}

void QWindow::registerAspect(const QString &name)
{
    Q_ASSERT(!isVisible());
    Q_D(QWindow);
    d->m_engine->registerAspect(name);
}

void QWindow::setRootEntity(QEntity *root)
{
    Q_ASSERT(!isVisible());
    Q_D(QWindow);
    d->m_userRoot = root;
}

void QWindow::show()
{
    Q_D(QWindow);

    if (d->m_userRoot != Q_NULLPTR)
        d->m_userRoot->setParent(d->m_root);

    if (d->m_frameGraph == Q_NULLPTR) {
        d->m_frameGraph = new QFrameGraph();
        QForwardRenderer *forwardRenderer = new QForwardRenderer();
        forwardRenderer->setCamera(d->m_defaultCamera);
        d->m_frameGraph->setActiveFrameGraph(forwardRenderer);
    }

    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(this)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(this));
    d->m_engine->setData(data);

    d->m_engine->initialize();
    d->m_root->addComponent(d->m_frameGraph);
    d->m_engine->setRootEntity(d->m_root);

    ::QWindow::show();
}

} // Qt3D

QT_END_NAMESPACE
