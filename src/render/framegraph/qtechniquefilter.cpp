
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

#include "qtechniquefilter.h"
#include "qtechniquefilter_p.h"
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DCore/qnodepropertychange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QTechniqueFilterPrivate::QTechniqueFilterPrivate()
    : QFrameGraphNodePrivate()
{
}

void QTechniqueFilter::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QTechniqueFilter *other = static_cast<const QTechniqueFilter*>(ref);

    Q_FOREACH (QFilterKey *crit, other->d_func()->m_matchList)
        addMatch(qobject_cast<QFilterKey *>(QNode::clone(crit)));
    for (QParameter *p : other->d_func()->m_parameters)
        addParameter(qobject_cast<QParameter *>(QNode::clone(p)));
}

QTechniqueFilter::QTechniqueFilter(QNode *parent)
    : QFrameGraphNode(*new QTechniqueFilterPrivate, parent)
{
}

/*! \internal */
QTechniqueFilter::QTechniqueFilter(QTechniqueFilterPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

QVector<QFilterKey *> QTechniqueFilter::matchAll() const
{
    Q_D(const QTechniqueFilter);
    return d->m_matchList;
}

void QTechniqueFilter::addMatch(QFilterKey *criterion)
{
    Q_D(QTechniqueFilter);
    if (!d->m_matchList.contains(criterion)) {
        d->m_matchList.append(criterion);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!criterion->parent())
            criterion->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QNodePropertyChangePtr propertyChange(new QNodePropertyChange(NodeAdded, QSceneChange::Node, id()));
            propertyChange->setPropertyName("require");
            propertyChange->setValue(QVariant::fromValue(criterion->id()));
            d->notifyObservers(propertyChange);
        }
    }
}

void QTechniqueFilter::removeMatch(QFilterKey *criterion)
{
    Q_D(QTechniqueFilter);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QNodePropertyChangePtr propertyChange(new QNodePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        propertyChange->setPropertyName("require");
        propertyChange->setValue(QVariant::fromValue(criterion->id()));
        d->notifyObservers(propertyChange);
    }
    d->m_matchList.removeOne(criterion);
}

void QTechniqueFilter::addParameter(QParameter *parameter)
{
    Q_D(QTechniqueFilter);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, the child parameters get destroyed as well
        if (!parameter->parent())
            parameter->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QNodePropertyChangePtr change(new QNodePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("parameter");
            change->setValue(QVariant::fromValue(parameter->id()));
            d->notifyObservers(change);
        }
    }
}

void QTechniqueFilter::removeParameter(QParameter *parameter)
{
    Q_D(QTechniqueFilter);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QNodePropertyChangePtr change(new QNodePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("parameter");
        change->setValue(QVariant::fromValue(parameter->id()));
        d->notifyObservers(change);
    }
    d->m_parameters.removeOne(parameter);
}

QVector<QParameter *> QTechniqueFilter::parameters() const
{
    Q_D(const QTechniqueFilter);
    return d->m_parameters;
}

Qt3DCore::QNodeCreatedChangeBasePtr QTechniqueFilter::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QTechniqueFilterData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QTechniqueFilter);
    data.matchIds = qIdsForNodes(d->m_matchList);
    data.parameterIds = qIdsForNodes(d->m_parameters);
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
