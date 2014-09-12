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

#include "qsortcriterion.h"
#include "qsortcriterion_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QSortCriterionPrivate::QSortCriterionPrivate(QSortCriterion *qq)
    : QNodePrivate(qq)
    , m_sort(QSortCriterion::StateChangeCost)
{
}

QSortCriterion::QSortCriterion(QNode *parent)
    : QNode(*new QSortCriterionPrivate(this), parent)
{
}

void QSortCriterion::copy(const QNode *ref)
{
    Q_D(QSortCriterion);
    QNode::copy(ref);
    const QSortCriterion *s = qobject_cast<const QSortCriterion *>(ref);
    if (s != Q_NULLPTR)
        d->m_sort = s->sort();
}

QSortCriterion::SortType QSortCriterion::sort() const
{
    Q_D(const QSortCriterion);
    return d->m_sort;
}

void QSortCriterion::setSort(QSortCriterion::SortType &sort)
{
    Q_D(QSortCriterion);
    if (d->m_sort != sort) {

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeUpdated, this));
            propertyChange->setPropertyName(QByteArrayLiteral("sort"));
            propertyChange->setValue(d->m_sort);
            notifyObservers(propertyChange);
        }

        d->m_sort = sort;
        emit sortChanged();

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr propertyChange(new QScenePropertyChange(NodeUpdated, this));
            propertyChange->setPropertyName(QByteArrayLiteral("sort"));
            propertyChange->setValue(d->m_sort);
            notifyObservers(propertyChange);
        }
    }
}

QSortCriterion::QSortCriterion(QSortCriterionPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QSortCriterion *QSortCriterion::doClone(QNode *clonedParent) const
{
    return new QSortCriterion(clonedParent);
}

} // Qt3D

QT_END_NAMESPACE
