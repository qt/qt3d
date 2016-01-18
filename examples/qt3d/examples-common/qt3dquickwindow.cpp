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

#include "qt3dquickwindow.h"
#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DInput/qinputaspect.h>
#include <Qt3DLogic/qlogicaspect.h>

#include <QQmlContext>

#include <QtGui/qopenglcontext.h>

QT_BEGIN_NAMESPACE

Qt3DQuickWindow::Qt3DQuickWindow(QWindow *parent)
    : QQuickWindow(parent)
    , m_engine(new Qt3DCore::Quick::QQmlAspectEngine)
    , m_renderAspect(new Qt3DRender::QRenderAspect)
    , m_inputAspect(new Qt3DInput::QInputAspect)
    , m_logicAspect(new Qt3DLogic::QLogicAspect)
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

    m_engine->aspectEngine()->registerAspect(m_renderAspect);
    m_engine->aspectEngine()->registerAspect(m_inputAspect);
    m_engine->aspectEngine()->registerAspect(m_logicAspect);
}

Qt3DQuickWindow::~Qt3DQuickWindow()
{
}

void Qt3DQuickWindow::registerAspect(Qt3DCore::QAbstractAspect *aspect)
{
    Q_ASSERT(!isVisible());
    m_engine->aspectEngine()->registerAspect(aspect);
}

void Qt3DQuickWindow::registerAspect(const QString &name)
{
    Q_ASSERT(!isVisible());
    m_engine->aspectEngine()->registerAspect(name);
}

void Qt3DQuickWindow::setSource(const QUrl &source)
{
    m_source = source;
}

Qt3DCore::Quick::QQmlAspectEngine *Qt3DQuickWindow::engine() const
{
    return m_engine.data();
}

void Qt3DQuickWindow::showEvent(QShowEvent *e)
{
    if (!m_initialized) {

        // TODO: Get rid of this
        QVariantMap data;
        data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(this)));
        data.insert(QStringLiteral("eventSource"), QVariant::fromValue(this));
        m_engine->aspectEngine()->setData(data);

        // TODO: Remove this hack once we can automagically set the window on the framegraph
        m_engine->qmlEngine()->rootContext()->setContextProperty("_q_window", this);

        m_engine->setSource(m_source);

        // Set the QQmlIncubationController on the window
        // to benefit from asynchronous incubation
        m_engine->qmlEngine()->setIncubationController(QQuickWindow::incubationController());

        m_initialized = true;
    }
    QQuickWindow::showEvent(e);
}

QT_END_NAMESPACE
