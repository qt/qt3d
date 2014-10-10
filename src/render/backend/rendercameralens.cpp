/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rendercameralens_p.h"
#include "rendereraspect.h"
#include "renderlogging.h"

#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>

#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderCameraLens::RenderCameraLens()
    : m_renderer(Q_NULLPTR)
{
    m_clearColor = QVector4D(0.5, 0.5, 1.0, 1.0);
}

RenderCameraLens::~RenderCameraLens()
{
    cleanup();
}

void RenderCameraLens::cleanup()
{
    if (!m_lensUuid.isNull()) {
        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_lensUuid);
    }
}

void RenderCameraLens::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderCameraLens::setPeer(QCameraLens *peer)
{
    QUuid peerUuid;
    if (peer != Q_NULLPTR)
        peerUuid = peer->uuid();
    if (peerUuid != m_lensUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_lensUuid.isNull()) {
            arbiter->unregisterObserver(this, m_lensUuid);
            m_lensUuid = QUuid();
        }
        m_lensUuid = peerUuid;
        if (!m_lensUuid.isNull()) {
            // Register for changes
            arbiter->registerObserver(this, m_lensUuid, NodeUpdated);
            setProjection(peer->projectionMatrix());
        }
    }
}

// TO DO : Move that else where
unsigned int RenderCameraLens::clearMask() const
{
    return GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}

void RenderCameraLens::setProjection(const QMatrix4x4 &projection)
{
    m_projection = projection;
}

void RenderCameraLens::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->type()) {
    case NodeUpdated: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        QMatrix4x4 projectionMatrix = propertyChange->value().value<QMatrix4x4>();
        m_projection = projectionMatrix;
        }
        break;

    default:
        break;
    }
}

QUuid RenderCameraLens::lensUuid() const
{
    return m_lensUuid;
}

} // Render
} // Qt3D

QT_END_NAMESPACE
