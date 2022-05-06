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
******************************************************************************/

#include "sortcriterion_p.h"
#include <Qt3DCore/qnodepropertychange.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

SortCriterion::SortCriterion()
    : BackendNode()
    , m_type(QSortCriterion::StateChangeCost)
{
}

void SortCriterion::cleanup()
{
    QBackendNode::setEnabled(false);
}

void SortCriterion::updateFromPeer(Qt3DCore::QNode *peer)
{
    QSortCriterion *criterion = static_cast<QSortCriterion *>(peer);
    m_type = criterion->sort();
}

QSortCriterion::SortType SortCriterion::sortType() const
{
    return m_type;
}

void SortCriterion::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QNodePropertyChangePtr propertyChange = qSharedPointerCast<QNodePropertyChange>(e);
    if (e->type() == NodeUpdated && propertyChange->propertyName() == QByteArrayLiteral("sort")) {
        m_type = static_cast<QSortCriterion::SortType>(propertyChange->value().toInt());
    }
    markDirty(AbstractRenderer::AllDirty);

    BackendNode::sceneChangeEvent(e);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
