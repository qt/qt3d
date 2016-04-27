/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsortpolicy_p.h"
#include <Qt3DCore/qnodepropertychange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QSortPolicyPrivate::QSortPolicyPrivate()
    : QFrameGraphNodePrivate()
{
}

void QSortPolicy::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QSortPolicy *other = static_cast<const QSortPolicy*>(ref);
    Q_FOREACH (const QSortPolicy::SortType c, other->d_func()->m_sortTypes)
        addSortType(c);
}

QSortPolicy::QSortPolicy(QNode *parent)
    : QFrameGraphNode(*new QSortPolicyPrivate, parent)
{
}

/*! \internal */
QSortPolicy::QSortPolicy(QSortPolicyPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

void QSortPolicy::addSortType(Qt3DRender::QSortPolicy::SortType sortType)
{
    Q_D(QSortPolicy);
    if (!d->m_sortTypes.contains(sortType)) {
        d->m_sortTypes.append(sortType);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QNodePropertyChangePtr propertyChange(new QNodePropertyChange(NodeAdded, QSceneChange::Node, id()));
            propertyChange->setPropertyName("sortType");
            propertyChange->setValue(QVariant::fromValue(sortType));
            d->notifyObservers(propertyChange);
        }
    }
}

void QSortPolicy::removeSortType(SortType sortType)
{
    Q_D(QSortPolicy);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QNodePropertyChangePtr propertyChange(new QNodePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        propertyChange->setPropertyName("sortType");
        propertyChange->setValue(QVariant::fromValue(sortType));
        d->notifyObservers(propertyChange);
    }
    d->m_sortTypes.removeOne(sortType);
}

QVector<QSortPolicy::SortType> QSortPolicy::sortTypes() const
{
    Q_D(const QSortPolicy);
    return d->m_sortTypes;
}

QVariantList QSortPolicy::sortTypeList() const
{
    Q_D(const QSortPolicy);
    QVariantList ret;
    Q_FOREACH (const auto type, d->m_sortTypes)
        ret.append(QVariant(type));

    return ret;
}

void QSortPolicy::setSortTypes(QVector<QSortPolicy::SortType> sortTypes)
{
    Q_D(QSortPolicy);
    d->m_sortTypes = std::move(sortTypes);
}

void QSortPolicy::setSortTypes(const QVariantList &sortTypes)
{
    Q_D(QSortPolicy);
    d->m_sortTypes.clear();
    Q_FOREACH (const auto &typeVariant, sortTypes)
        d->m_sortTypes.append(static_cast<QSortPolicy::SortType>(typeVariant.toInt()));
}

} // namespace Qt3DRender

QT_END_NAMESPACE
