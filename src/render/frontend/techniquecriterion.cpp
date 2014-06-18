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

#include "techniquecriterion.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

TechniqueCriterion::TechniqueCriterion(QObject *parent)
    : QObject(parent)
    , d_ptr(new TechniqueCriterionPrivate(this))
{

}

bool TechniqueCriterion::operator ==(const TechniqueCriterion &other)
{
    if (&other == this)
        return true;
    if (other.criterionType() == criterionType()) {
        if (other.criterionType() == CustomType)
            return (other.criterionCustomType() == criterionCustomType())
                    && (other.criterionValue() == criterionValue());
        else
            return other.criterionValue() == criterionValue();
    }
    return false;
}

bool TechniqueCriterion::operator !=(const TechniqueCriterion &other)
{
    return !operator ==(other);
}

void TechniqueCriterion::setCriterionType(TechniqueCriterion::CriterionType type)
{
    Q_D(TechniqueCriterion);
    if (type != d->m_criterionType) {
        d->m_criterionType = type;
        emit criterionTypeChanged();
    }
}

void TechniqueCriterion::setCriterionValue(const QVariant &value)
{
    Q_D(TechniqueCriterion);
    if (value != d->m_criterionValue) {
        d->m_criterionValue = value;
        emit criterionValueChanged();
    }
}

void TechniqueCriterion::setCriterionCustomType(const QString &customType)
{
    Q_D(TechniqueCriterion);
    if (customType != d->m_criterionCustomType) {
        d->m_criterionCustomType = customType;
        emit criterionCustomTypeChanged();
    }
}

TechniqueCriterion::CriterionType TechniqueCriterion::criterionType() const
{
    Q_D(const TechniqueCriterion);
    return d->m_criterionType;
}

QVariant TechniqueCriterion::criterionValue() const
{
    Q_D(const TechniqueCriterion);
    return d->m_criterionValue;
}

QString TechniqueCriterion::criterionCustomType() const
{
    Q_D(const TechniqueCriterion);
    return d->m_criterionCustomType;
}

} // Qt3D

QT_END_NAMESPACE
