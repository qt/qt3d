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

#include "rendertransform_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/rendereraspect.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderTransform::RenderTransform()
    : m_renderer(Q_NULLPTR)
{
}

RenderTransform::~RenderTransform()
{
    cleanup();
}

void RenderTransform::cleanup()
{
    m_transformMatrix.setToIdentity();
    if (!m_peerUuid.isNull())
        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_peerUuid);
}

void RenderTransform::setPeer(QTransform *peer)
{
    QUuid peerUuid;
    if (peer != Q_NULLPTR)
        peerUuid = peer->uuid();
    if (peerUuid != m_peerUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_peerUuid.isNull())
            arbiter->unregisterObserver(this, m_peerUuid);
        m_peerUuid = peerUuid;
        if (peer) {
            arbiter->registerObserver(this, m_peerUuid, NodeUpdated);
            m_transformMatrix = peer->matrix();
        }
    }
}

void RenderTransform::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

QUuid RenderTransform::transformUuid() const
{
    return m_peerUuid;
}

QMatrix4x4 RenderTransform::transformMatrix() const
{
    return m_transformMatrix;
}

void RenderTransform::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (e->type() == NodeUpdated && propertyChange->propertyName() == QByteArrayLiteral("matrix"))
        m_transformMatrix = propertyChange->value().value<QMatrix4x4>();
}

} // Render

} // Qt3D

QT_END_NAMESPACE
