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

#include "renderscene_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qsceneinterface.h>
#include <Qt3DCore/qbackendscenepropertychange.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DRenderer/qabstractsceneloader.h>
#include <Qt3DRenderer/private/scenemanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderScene::RenderScene()
    : QBackendObservable()
    , m_renderer(Q_NULLPTR)
{
}

void RenderScene::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderScene::setPeer(QAbstractSceneLoader *peer)
{
    QUuid peerUuid;
    if (peer)
        peerUuid = peer->uuid();
    if (peerUuid != m_peerUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_peerUuid.isNull()) {
            arbiter->unregisterObserver(this, m_peerUuid);
            arbiter->scene()->removeObservable(this, m_peerUuid);
            m_source.clear();
        }
        m_peerUuid = peerUuid;
        if (!m_peerUuid.isNull()) {
            arbiter->registerObserver(this, m_peerUuid, NodeUpdated);
            arbiter->scene()->addObservable(this, m_peerUuid);
            m_source = peer->source();
            m_renderer->sceneManager()->addSceneData(m_source, sceneUuid());
        }
    }
}

void RenderScene::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (propertyChange->propertyName() == QByteArrayLiteral("source")) {
        m_source = propertyChange->value().toString();
        m_renderer->sceneManager()->addSceneData(m_source, sceneUuid());
    }
}

QUuid RenderScene::sceneUuid() const
{
    return m_peerUuid;
}

QString RenderScene::source() const
{
    return m_source;
}

void RenderScene::setSceneSubtree(QEntity *subTree)
{
    QBackendScenePropertyChangePtr e(new QBackendScenePropertyChange(NodeUpdated, this));
    e->setPropertyName(QByteArrayLiteral("scene"));
    // The Frontend element has to perform the clone
    // So that the objects are created in the main thread
    e->setValue(QVariant::fromValue(subTree));
    e->setTargetNode(m_peerUuid);
    notifyObservers(e);
    QBackendScenePropertyChangePtr e2(new QBackendScenePropertyChange(NodeUpdated, this));
    e2->setPropertyName(QByteArrayLiteral("status"));
    e2->setValue(subTree != Q_NULLPTR ? QAbstractSceneLoader::Loaded : QAbstractSceneLoader::Error);
    e2->setTargetNode(m_peerUuid);
    notifyObservers(e2);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
