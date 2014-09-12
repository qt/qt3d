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

#include "quick3dsortmethod.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DSortMethod::Quick3DSortMethod(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QSortCriterion> Quick3DSortMethod::criteriaList()
{
    return QQmlListProperty<QSortCriterion>(this, 0,
                                            &Quick3DSortMethod::appendCriterion,
                                            &Quick3DSortMethod::criteriaCount,
                                            &Quick3DSortMethod::criterionAt,
                                            &Quick3DSortMethod::clearCriteria);
}

void Quick3DSortMethod::appendCriterion(QQmlListProperty<QSortCriterion> *list, QSortCriterion *criterion)
{
    Quick3DSortMethod *sM = qobject_cast<Quick3DSortMethod *>(list->object);
    if (sM != Q_NULLPTR)
        sM->parentSortMethod()->addCriterion(criterion);
}

QSortCriterion *Quick3DSortMethod::criterionAt(QQmlListProperty<QSortCriterion> *list, int index)
{
    Quick3DSortMethod *sM = qobject_cast<Quick3DSortMethod *>(list->object);
    if (sM != Q_NULLPTR)
        return sM->parentSortMethod()->criteria().at(index);
    return Q_NULLPTR;
}

int Quick3DSortMethod::criteriaCount(QQmlListProperty<QSortCriterion> *list)
{
    Quick3DSortMethod *sM = qobject_cast<Quick3DSortMethod *>(list->object);
    if (sM != Q_NULLPTR)
        return sM->parentSortMethod()->criteria().count();
    return -1;
}

void Quick3DSortMethod::clearCriteria(QQmlListProperty<QSortCriterion> *list)
{
    Quick3DSortMethod *sM = qobject_cast<Quick3DSortMethod *>(list->object);
    if (sM != Q_NULLPTR) {
        Q_FOREACH (QSortCriterion *c, sM->parentSortMethod()->criteria())
            sM->parentSortMethod()->removeCriterion(c);
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
