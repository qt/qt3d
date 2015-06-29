/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsortcriterion.h"
#include "qsortcriterion_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QSortCriterionPrivate
    \internal
*/
QSortCriterionPrivate::QSortCriterionPrivate()
    : QNodePrivate()
    , m_sort(QSortCriterion::StateChangeCost)
{
}

void QSortCriterion::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QSortCriterion *s = static_cast<const QSortCriterion*>(ref);
    d_func()->m_sort = s->d_func()->m_sort;
}

QSortCriterion::QSortCriterion(QNode *parent)
    : QNode(*new QSortCriterionPrivate, parent)
{
}

QSortCriterion::~QSortCriterion()
{
    QNode::cleanup();
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

        d->m_sort = sort;
        emit sortChanged();
    }
}

/*! \internal */
QSortCriterion::QSortCriterion(QSortCriterionPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

} // Qt3D

QT_END_NAMESPACE
