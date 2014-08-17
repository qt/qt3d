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

#include "techniquefilternode_p.h"
#include "qcriterion.h"
#include "qtechniquefilter.h"
#include "rendereraspect.h"
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/criterionmanager_p.h>
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

TechniqueFilter::TechniqueFilter()
    : FrameGraphNode(FrameGraphNode::TechniqueFilter)
{
}

void TechniqueFilter::setPeer(Qt3D::QTechniqueFilter *peer)
{
    QUuid peerUuid;
    if (peer != Q_NULLPTR)
        peerUuid = peer->uuid();
    if (m_frontendUuid != peerUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_frontendUuid.isNull()) {
            arbiter->unregisterObserver(this, m_frontendUuid);
            m_filters.clear();
        }
        m_frontendUuid = peerUuid;
        if (!m_frontendUuid.isNull()) {
            arbiter->registerObserver(this, m_frontendUuid, NodeAdded|NodeRemoved);
            Q_FOREACH (QCriterion *criterion, peer->criteria())
                appendFilter(criterion);
        }
    }
}

QList<QUuid> TechniqueFilter::filters() const
{
    return m_filters;
}

void TechniqueFilter::appendFilter(QCriterion *criterion)
{
    if (!m_filters.contains(criterion->uuid()))
        m_filters.append(criterion->uuid());
}

void TechniqueFilter::removeFilter(const QUuid &criterionId)
{
    m_filters.removeOne(criterionId);
}

void TechniqueFilter::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->type()) {
    case NodeAdded: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("techniqueCriterion"))
            appendFilter(propertyChange->value().value<QCriterion *>());
    }
        break;
    case NodeRemoved: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("techniqueCriterion"))
            removeFilter(propertyChange->value().toUuid());
    }
        break;
    default:
        break;
    }
}

} // Render

} // Qt3D

QT_END_NAMESPACE
