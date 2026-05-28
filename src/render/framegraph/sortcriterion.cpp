// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "sortcriterion_p.h"
#include <Qt3DCore/qnodepropertychange.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

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
