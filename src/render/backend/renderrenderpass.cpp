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

#include "renderrenderpass.h"
#include "renderer.h"
#include "rendereraspect.h"
#include "rendercriterion.h"
#include "criterionmanager.h"
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qabstractshader.h>
#include <Qt3DRenderer/qparametermapper.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderRenderPass::RenderRenderPass()
    : m_renderer(Q_NULLPTR)
    , m_peer(Q_NULLPTR)
    , m_shader(Q_NULLPTR)
{
}

RenderRenderPass::~RenderRenderPass()
{
    cleanup();
}

void RenderRenderPass::cleanup()
{
    if (m_renderer != Q_NULLPTR && m_peer != Q_NULLPTR)
        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_peer);
}

void RenderRenderPass::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderRenderPass::setPeer(QRenderPass *peer)
{
    if (m_peer != peer) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (m_peer)
            arbiter->unregisterObserver(this, m_peer);
        m_peer = peer;
        m_shader = Q_NULLPTR;
        if (m_peer) {
            arbiter->registerObserver(this, m_peer);
            m_shader = m_peer->shaderProgram();
            // TO DO -> Have backend classes for Bindings and Parameters so that we can easily monitor for updates
            m_bindings = m_peer->bindings();
        }
    }
}

void RenderRenderPass::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {
    case ComponentUpdated: {
        if (propertyChange->propertyName() == QByteArrayLiteral("shaderProgram"))
            ;// TO DO: to be completed
        break;
    }

    case ComponentAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("criterion")) {
            QCriterion *crit = propertyChange->value().value<QCriterion *>();
            HCriterion critHandle = m_renderer->criterionManager()->lookupHandle(crit->uuid());
            if (critHandle.isNull()) {
                critHandle = m_renderer->criterionManager()->getOrAcquireHandle(crit->uuid());
                RenderCriterion *renderCriterion = m_renderer->criterionManager()->data(critHandle);
                renderCriterion->setRenderer(m_renderer);
                renderCriterion->setPeer(crit);
            }
            if (!m_criteriaList.contains(critHandle))
                m_criteriaList.append(critHandle);
        }
        break;
    }

    case ComponentRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("criterion")) {
            m_criteriaList.removeOne(m_renderer->criterionManager()->lookupHandle(propertyChange->value().toUuid()));
        }
        break;
    }

    default:
        break;
    }

}

QAbstractShader *RenderRenderPass::shaderProgram() const
{
    return m_shader;
}

QList<QParameterMapper *> RenderRenderPass::bindings() const
{
    return m_bindings;
}

QList<HCriterion> RenderRenderPass::criteria() const
{
    return m_criteriaList;
}

} // Render

} // Qt3D

QT_END_NAMESPACE
