/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qt3dwindow.h"

#include <Qt3DRender/qforwardrenderer.h>
#include <Qt3DRender/qframegraph.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DInput/qinputaspect.h>
#include <Qt3DLogic/qlogicaspect.h>

#include <Qt3DCore/qaspectengine.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>

#include <QtGui/qopenglcontext.h>

QT_BEGIN_NAMESPACE

Qt3DWindow::Qt3DWindow(QScreen *screen)
    : QWindow(screen)
    , m_aspectEngine(new Qt3DCore::QAspectEngine)
    , m_renderAspect(new Qt3DRender::QRenderAspect)
    , m_inputAspect(new Qt3DInput::QInputAspect)
    , m_logicAspect(new Qt3DLogic::QLogicAspect)
    , m_frameGraph(nullptr)
    , m_defaultCamera(new Qt3DRender::QCamera)
    , m_root(new Qt3DCore::QEntity)
    , m_userRoot(nullptr)
    , m_initialized(false)
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

    m_aspectEngine->registerAspect(m_renderAspect);
    m_aspectEngine->registerAspect(m_inputAspect);
    m_aspectEngine->registerAspect(m_logicAspect);
}

Qt3DWindow::Qt3DWindow(QWindow *parent)
    : QWindow(parent)
    , m_aspectEngine(new Qt3DCore::QAspectEngine)
    , m_renderAspect(new Qt3DRender::QRenderAspect)
    , m_inputAspect(new Qt3DInput::QInputAspect)
    , m_logicAspect(new Qt3DLogic::QLogicAspect)
    , m_frameGraph(nullptr)
    , m_defaultCamera(new Qt3DRender::QCamera)
    , m_root(new Qt3DCore::QEntity)
    , m_userRoot(nullptr)
    , m_initialized(false)
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

    m_aspectEngine->registerAspect(m_renderAspect);
    m_aspectEngine->registerAspect(m_inputAspect);
    m_aspectEngine->registerAspect(m_logicAspect);}

Qt3DWindow::~Qt3DWindow()
{
}

void Qt3DWindow::registerAspect(Qt3DCore::QAbstractAspect *aspect)
{
    Q_ASSERT(!isVisible());
    m_aspectEngine->registerAspect(aspect);
}

void Qt3DWindow::registerAspect(const QString &name)
{
    Q_ASSERT(!isVisible());
    m_aspectEngine->registerAspect(name);
}

void Qt3DWindow::setRootEntity(Qt3DCore::QEntity *root)
{
    Q_ASSERT(!isVisible());
    m_userRoot = root;
}

void Qt3DWindow::setFrameGraph(Qt3DRender::QFrameGraph *frameGraph)
{
    m_frameGraph = frameGraph;
}

Qt3DRender::QFrameGraph *Qt3DWindow::frameGraph() const
{
    return m_frameGraph;
}

Qt3DRender::QCamera *Qt3DWindow::camera() const
{
    return m_defaultCamera;
}

void Qt3DWindow::showEvent(QShowEvent *e)
{
    if (!m_initialized) {
        if (m_userRoot != nullptr)
            m_userRoot->setParent(m_root);

        if (m_frameGraph == nullptr) {
            m_frameGraph = new Qt3DRender::QFrameGraph;
            Qt3DRender::QForwardRenderer *forwardRenderer = new Qt3DRender::QForwardRenderer;
            forwardRenderer->setCamera(m_defaultCamera);
            forwardRenderer->setSurface(this);
            m_frameGraph->setActiveFrameGraph(forwardRenderer);
        }

        // TODO: Get rid of this
        QVariantMap data;
        data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(this)));
        data.insert(QStringLiteral("eventSource"), QVariant::fromValue(this));
        m_aspectEngine->setData(data);

        m_root->addComponent(m_frameGraph);
        m_aspectEngine->setRootEntity(m_root);

        m_initialized = true;
    }

    QWindow::showEvent(e);
}

void Qt3DWindow::resizeEvent(QResizeEvent *)
{
    m_defaultCamera->setAspectRatio(float(width()) / float(height()));
}

QT_END_NAMESPACE
