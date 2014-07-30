
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

#include "qtechniquefilter.h"
#include "qtechniquefilter_p.h"

#include <Qt3DRenderer/qcriterion.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QTechniqueFilterPrivate::QTechniqueFilterPrivate(QTechniqueFilter *qq)
    : QFrameGraphItemPrivate(qq)
{
}

QTechniqueFilter::QTechniqueFilter(QNode *parent)
    : QFrameGraphItem(*new QTechniqueFilterPrivate(this), parent)
{
}

QTechniqueFilter::QTechniqueFilter(QTechniqueFilterPrivate &dd, QNode *parent)
    : QFrameGraphItem(dd, parent)
{
}

QTechniqueFilter *QTechniqueFilter::doClone(QNode *clonedParent) const
{
    Q_D(const QTechniqueFilter);
    QTechniqueFilter *clone = new QTechniqueFilter(clonedParent);

    Q_FOREACH (QFrameGraphItem *fgChild, d->m_fgChildren)
        clone->appendFrameGraphItem(qobject_cast<QFrameGraphItem *>(fgChild->clone(clone)));
    Q_FOREACH (QCriterion *crit, d->m_criteriaList)
        clone->addCriterion(qobject_cast<QCriterion *>(crit->clone(clonedParent)));

    return clone;
}

QList<QCriterion *> QTechniqueFilter::criteria() const
{
    Q_D(const QTechniqueFilter);
    return d->m_criteriaList;
}

void QTechniqueFilter::addCriterion(QCriterion *criterion)
{
    Q_D(QTechniqueFilter);
    d->m_criteriaList.append(criterion);

    // We need to add it as a child of the current node if it has been declared inline
    // Or not previously added as a child of the current node so that
    // 1) The backend gets notified about it's creation
    // 2) When the current node is destroyed, it gets destroyed as well
    if (!criterion->parent() || criterion->parent() == this)
        QNode::addChild(criterion);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeAdded, this));
        propertyChange->setPropertyName(QByteArrayLiteral("techniqueCriterion"));
        propertyChange->setValue(QVariant::fromValue(criterion));
        notifyObservers(propertyChange);
    }
}

void QTechniqueFilter::removeCriterion(QCriterion *criterion)
{
    Q_D(QTechniqueFilter);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeRemoved, this));
        propertyChange->setPropertyName(QByteArrayLiteral("techniqueCriterion"));
        propertyChange->setValue(QVariant::fromValue(criterion->uuid()));
        notifyObservers(propertyChange);
    }
    d->m_criteriaList.removeOne(criterion);
}

} // Qt3D

QT_END_NAMESPACE
