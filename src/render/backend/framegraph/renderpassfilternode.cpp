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

#include "renderpassfilternode.h"
#include "rendereraspect.h"
#include "renderer.h"
#include "techniquecriterionmanager.h"
#include "qtechniquecriterion.h"
#include "qrenderpassfilter.h"
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderPassFilter::RenderPassFilter(FrameGraphNode *parent)
    : FrameGraphNode(FrameGraphNode::RenderPassFilter, parent)
    , m_renderer(Q_NULLPTR)
    , m_peer(Q_NULLPTR)
{
}

void RenderPassFilter::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderPassFilter::setPeer(Qt3D::QRenderPassFilter *peer)
{
    if (m_peer != peer) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (m_peer)
            arbiter->unregisterObserver(this, m_peer);
        m_peer = peer;
        if (m_peer) {
            arbiter->registerObserver(this, m_peer);
            Q_FOREACH (QTechniqueCriterion *criterion, m_peer->criteria())
                appendFilter(criterion);
        }
    }
}

QList<HTechniqueCriterion> RenderPassFilter::filters() const
{
    return m_filters;
}

void RenderPassFilter::appendFilter(QTechniqueCriterion *criterion)
{
    HTechniqueCriterion critHandle = m_renderer->techniqueCriterionManager()->lookupHandle(criterion->uuid());
    if (critHandle.isNull()) {
        critHandle = m_renderer->techniqueCriterionManager()->getOrAcquireHandle(criterion->uuid());
        RenderCriterion *rCrit = m_renderer->techniqueCriterionManager()->data(critHandle);
        rCrit->setRenderer(m_renderer);
        rCrit->setPeer(criterion);
    }
    if (!m_filters.contains(critHandle))
        m_filters.append(critHandle);
}

void RenderPassFilter::removeFilter(const QUuid &criterionId)
{
    m_filters.removeOne(m_renderer->techniqueCriterionManager()->lookupHandle(criterionId));
}

void RenderPassFilter::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->type()) {
    case ComponentAdded: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("renderPassCriterion"))
            appendFilter(propertyChange->value().value<QTechniqueCriterion*>());
    }
        break;
    case ComponentRemoved: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("renderPassCriterion"))
            removeFilter(propertyChange->value().toUuid());
    }
        break;
    default:
        break;
    }
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
